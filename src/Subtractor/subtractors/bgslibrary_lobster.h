#pragma once

#include "basesubtractor.h"
#include "LBSP/BackgroundSubtractorLOBSTER.h"

class QJsonObject;


namespace Subtractors
{
	class LOBSTER : public BaseSubtractor 
	{

		private:
			double m_relThreshold;
            size_t m_offsetSimilarityThreshold;
            size_t m_descDistThreshold;
            size_t m_colorDistThreshold;
            size_t m_nbBGSamples;
            size_t m_requiredNbBGSamples;
            size_t m_learnignRate;
            bool m_firstTime;

			cv::Mat img_background;
			cv::Mat img_foreground;

            lbsp::BackgroundSubtractorLOBSTER* pLOBSTER;

		public:
			LOBSTER(QJsonObject const &a_config);
			~LOBSTER();

		void process(std::vector<_data> &_data);
	};
} // namespace Subtractors
