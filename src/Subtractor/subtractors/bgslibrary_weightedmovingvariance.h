#pragma once

#include "basesubtractor.h"

class QJsonObject;


namespace Subtractors
{
	class WeightedMovingVariance : public BaseSubtractor 
	{

		private:
			cv::Mat img_background;
			cv::Mat img_foreground;

			cv::Mat img_input_prev_1;
			cv::Mat img_input_prev_2;
			bool enableWeight;
			bool enableThreshold;
			int threshold;

			cv::Mat computeWeightedVariance(const cv::Mat &img_input_f, const cv::Mat &img_mean_f, const double weight);

		public:
			WeightedMovingVariance(QJsonObject const &a_config);
			~WeightedMovingVariance();

		void process(std::vector<_data> &_data);
	};
} // namespace Subtractors