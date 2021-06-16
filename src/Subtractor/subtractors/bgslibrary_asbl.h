#pragma once

#include "basesubtractor.h"

class QJsonObject;


namespace Subtractors
{
	class ASBL : public BaseSubtractor 
	{

		private:
			double alphaLearn;
      		double alphaDetection;
     	 	int learningFrames;
			long counter;
			double minVal;
			double maxVal;
			
			int threshold;

			cv::Mat img_background;
			cv::Mat img_foreground;

		public:
			ASBL(QJsonObject const &a_config);
			~ASBL();

		void process(std::vector<_data> &_data);
	};
} // namespace Subtractors