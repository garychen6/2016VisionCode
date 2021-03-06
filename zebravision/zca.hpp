#pragma once
#include <vector>
#include "opencv2_3_shim.hpp"
#if CV_MAJOR_VERSION == 2
using cv::gpu::PtrStepSz;
#elif CV_MAJOR_VERSION == 3
using cv::cuda::PtrStepSz;
#endif

class ZCA
{
	public:
		// Given a set of input images, build ZCA weights
		// for a size() x 3channel input image
		ZCA(const std::vector<cv::Mat> &images, const cv::Size &size, float epsilon, bool globalContrastNorm);

		// Init a zca transformer by reading from a file
		ZCA(const char *xmlFilename, size_t batchSize);

		// Copy constructor - needed since some pointers
		// are allocated in constructor
		ZCA(const ZCA &zca);

		~ZCA();

		// Save ZCA state to file
		void Write(const char *xmlFilename) const;

		// Apply ZCA transofrm to a single image in
		// 8UC3 format (normal imread) and 32FC3
		// format (3 channels of float input)
		cv::Mat Transform8UC3 (const cv::Mat &input);
		cv::Mat Transform32FC3(const cv::Mat &input);

		// Batch versions of above - much faster
		// especially if GPU can be used
		std::vector<cv::Mat> Transform8UC3 (const std::vector<cv::Mat> &input);
		std::vector<cv::Mat> Transform32FC3(const std::vector<cv::Mat> &input);
		std::vector<GpuMat> Transform32FC3(const std::vector<GpuMat> &input);
		void Transform32FC3(const std::vector<GpuMat> &input, float *dest);

		// a and b parameters for transforming
		// float pixel values back to 0-255
		// uchar data
		double alpha(int maxPixelVal = 255) const;
		double beta(void) const;

		cv::Size size(void) const;

	private:
		cv::Size size_;

		// The weights, stored in both
		// the CPU and, if available, GPU
		cv::Mat  weights_;
		GpuMat   weightsGPU_;
		// GPU buffers - more efficient to allocate
		// them once gloabally and reuse them
		GpuMat   gm_;
		GpuMat   gmOut_;
		GpuMat   buf_;

		PtrStepSz<float> *dPssIn_;

		float            epsilon_;
		double           overallMin_;
		double           overallMax_;
		bool             globalContrastNorm_;
};
