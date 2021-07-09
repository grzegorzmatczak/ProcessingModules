#pragma once

#include "basesubtractor.h"

class QJsonObject;


namespace Subtractors
{
	class WeightedMovingMean : public BaseSubtractor 
	{

		private:
			cv::Mat img_background;
			cv::Mat img_foreground;

			cv::Mat img_input_prev_1;
			cv::Mat img_input_prev_2;
			bool enableWeight;
			bool enableThreshold;
			int threshold;
			
		public:
			WeightedMovingMean(QJsonObject const &a_config);
			~WeightedMovingMean();

		void process(std::vector<_data> &_data);
	};
} // namespace Subtractors