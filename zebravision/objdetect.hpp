#ifndef INC_OBJDETECT_HPP__
#define INC_OBJDETECT_HPP__

#include <iostream>
#include <sys/stat.h>
#include "opencv2_3_shim.hpp"
#include "detect.hpp"
#ifndef GIE
#include "CaffeClassifier.hpp"
#else
#include "GIEClassifier.hpp"
#endif

#include <vector>

// Base class for detector. Doesn't really do much - all 
// of the heavy lifting is in the derived classes
class ObjDetect
{
	public :
		ObjDetect() : init_(false) {} //pass in value of false to cascadeLoadedGPU_CascadeDetect
		virtual ~ObjDetect() {}       //empty destructor
		// virtual void Detect(const cv::Mat &frame, std::vector<cv::Rect> &imageRects) = 0; //pure virtual function, must be defined by CPU and GPU detect
		virtual void Detect(const cv::Mat &frameInput, 
				const cv::Mat &depthIn, 
				std::vector<cv::Rect> &imageRects, 
				std::vector<cv::Rect> &uncalibImageRects)
		{
			(void)frameInput;
			(void)depthIn;
			imageRects.clear();
			uncalibImageRects.clear();
		}
		bool initialized(void)
		{
			return init_;
		}

	protected:
		bool init_;
};

// CPU version of cascade classifier
/*class CPU_CascadeDetect : public ObjDetect
{
   public :
      CPU_CascadeDetect(const char *cascadeName) : ObjDetect() // call default constructor of base class
      {
	 struct stat statbuf;
	 if (stat(cascadeName, &statbuf) != 0)
	 {
	    std::cerr << "Can not open classifier input " << cascadeName << std::endl;
	    std::cerr << "Try to point to a different one with --classifierBase= ?" << std::endl;
	    return;
	 }

	 init_ = classifier_.load(cascadeName);
      }
      ~CPU_CascadeDetect(void)
      {
      }
      void Detect(const cv::Mat &frame, std::vector<cv::Rect> &imageRects); //defined elsewhere

   private :
      cv::CascadeClassifier classifier_;
};
*/

// Class to handle detections for all NNet based
// detectors. Detect code is the same for all of
// them even though the detector and classifier
// types are different.  Code the common Detect call
// here and then create the various classifiers in a
// set of derived classes
template <class MatT, class ClassifierT>
class ObjDetectNNet : public ObjDetect
{
	public:
		ObjDetectNNet(std::vector<std::string> &d12Files,
					  std::vector<std::string> &d24Files,
					  std::vector<std::string> &c12Files,
					  std::vector<std::string> &c24Files,
					  float hfov) :
			ObjDetect(),
			classifier_(d12Files, d24Files, c12Files, c24Files, hfov)
		{
			init_ = classifier_.initialized();
		}
		virtual ~ObjDetectNNet()
		{
		}
		void Detect(const cv::Mat &frameIn, 
					const cv::Mat &depthIn, 
					std::vector<cv::Rect> &imageRects, 
					std::vector<cv::Rect> &uncalibImageRects);
	private :
		NNDetect<MatT, ClassifierT> classifier_;
};

#ifndef GIE
// All-CPU code
class ObjDetectCaffeCPU : public ObjDetectNNet<cv::Mat, CaffeClassifier<cv::Mat>>
{
	public :
		ObjDetectCaffeCPU(std::vector<std::string> &d12Files,
							 std::vector<std::string> &d24Files,
							 std::vector<std::string> &c12Files,
							 std::vector<std::string> &c24Files,
							 float hfov) :
						ObjDetectNNet(d12Files, d24Files, c12Files, c24Files, hfov)
		{
		}

		~ObjDetectCaffeCPU(void)
		{
		}

};

// Both detector and Caffe run on the GPU
class ObjDetectCaffeGPU : public ObjDetectNNet<GpuMat, CaffeClassifier<GpuMat>>
{
	public :
		ObjDetectCaffeGPU(std::vector<std::string> &d12Files,
							 std::vector<std::string> &d24Files,
							 std::vector<std::string> &c12Files,
							 std::vector<std::string> &c24Files,
							 float hfov) :
						ObjDetectNNet(d12Files, d24Files, c12Files, c24Files, hfov)
		{
		}

		~ObjDetectCaffeGPU(void)
		{
		}

};
#else

// Detector does resizing, sliding windows and so on
// in CPU.  GIE run on GPU
class ObjDetectTensorRTCPU : public ObjDetectNNet<cv::Mat, GIEClassifier<cv::Mat>>
{
	public :
		ObjDetectTensorRTCPU(std::vector<std::string> &d12Files,
							 std::vector<std::string> &d24Files,
							 std::vector<std::string> &c12Files,
							 std::vector<std::string> &c24Files,
							 float hfov) :
						ObjDetectNNet(d12Files, d24Files, c12Files, c24Files, hfov)
		{
		}

		~ObjDetectTensorRTCPU(void)
		{
		}

};

// Both detector and GIE run on the GPU
class ObjDetectTensorRTCPU : public ObjDetectNNet<GpuMat, GIEClassifier<GpuMat>>
{
	public :
		ObjDetectTensorRTGPU(std::vector<std::string> &d12Files,
							 std::vector<std::string> &d24Files,
							 std::vector<std::string> &c12Files,
							 std::vector<std::string> &c24Files,
							 float hfov) :
						ObjDetectNNet(d12Files, d24Files, c12Files, c24Files, hfov)
		{
		}

		~ObjDetectTensorRTGPU(void)
		{
		}

};
#endif

// Various globals controlling detection.  
extern int scale;
extern int d12NmsThreshold;
extern int d24NmsThreshold;
extern int minDetectSize;
extern int maxDetectSize;
extern int d12Threshold;
extern int d24Threshold;
extern int c12Threshold;
extern int c24Threshold;

#endif
