#include "mediain.hpp"

MediaIn::MediaIn()
{
}

int MediaIn::frameCount(void) const
{
	return -1;
}

int MediaIn::frameNumber(void) const
{
	return -1;
}

void MediaIn::frameNumber(int frameNumber)
{
	(void)frameNumber;
}

int MediaIn::semValue() {
	return 2;
}

CameraParams MediaIn::getCameraParams(bool left) const
{
	(void)left;
	return CameraParams();
}
