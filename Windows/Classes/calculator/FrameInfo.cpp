#include "FrameInfo.h"

sb::FrameInfo::FrameInfo()
	: _topLeftPoint( 0, 0 ) {}

void sb::FrameInfo::create( const sb::Params& params ) { }

int sb::FrameInfo::create( const cv::Mat& colorImage,
                           const cv::Mat& depthImage,
                           const sb::Formatter& formatter,
                           const sb::EdgeDetector& edgeDetector,
                           const sb::LineDetector& lineDetector )
{
	// 1) reference to new frame
	_colorImage.release();

	if ( formatter.crop( colorImage, _colorImage ) < 0 ) {
		std::cerr << "Crop image failed." << std::endl;
		return -1;
	}

	_depthImage.release();

	_depthImage = depthImage;

	// 2) generate edges-frame
	cv::Mat edgesFrame;

	cv::cvtColor( _colorImage, edgesFrame, cv::COLOR_BGR2GRAY );

	edgeDetector.apply( edgesFrame );

	// 3) generate lines in whole frame
	lineDetector.apply( edgesFrame, _colorImage, _lines );

	// 4) generate warped lines
	if ( formatter.warp( _lines, _warpedLines, _topLeftPoint ) < 0 ) {
		std::cerr << "Warp lines failed." << std::endl;
		return -1;
	}

	// 5) generate sections
	if ( formatter.split( _warpedLines, _colorImage.rows, _sections ) < 0 ) {
		std::cerr << "Split sections failed." << std::endl;
		return -1;
	}

	return 0;
}

const cv::Mat& sb::FrameInfo::getColorImage() const { return _colorImage; }

const cv::Mat& sb::FrameInfo::getDepthImage() const { return _depthImage; }

const cv::Point2d& sb::FrameInfo::getTopLeftPoint() const { return _topLeftPoint; }

const std::vector<sb::LineInfo>& sb::FrameInfo::getLines() const { return _lines; }

const std::vector<sb::LineInfo>& sb::FrameInfo::getWarpedLines() const { return _warpedLines; }

const std::vector<sb::SectionInfo>& sb::FrameInfo::getSections() const { return _sections; }
