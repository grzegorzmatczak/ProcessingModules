#pragma once

#include "basesubtractor.h"

class QJsonObject;


namespace Subtractors
{
	class ABL : public BaseSubtractor 
	{

		private:
			double alpha;
			int limit;
			long counter;
			double minVal;
			double maxVal;
			bool enableThreshold;
			int threshold;

			cv::Mat img_background;
			cv::Mat img_foreground;

		public:
			ABL(QJsonObject const &a_config);
			~ABL();

		void process(std::vector<_data> &_data);
	};
} // namespace Subtractors