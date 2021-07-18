#pragma once

#include "basesubtractor.h"
#include "PBAS_0.1/PBAS.h"

class QJsonObject;


namespace Subtractors
{
	class PixelBasedAdaptiveSegmenter : public BaseSubtractor 
	{

	private:
		PBAS pbas;

		bool enableInputBlur;
		bool enableOutputBlur;

		double alpha;
		double beta;
		int N;
		int Raute_min;
		double R_incdec;
		int R_lower;
		int R_scale;
		double T_dec;
		int T_inc;
		int T_init;
		int T_lower;
		int T_upper;

		cv::Mat img_foreground;

	public:
		PixelBasedAdaptiveSegmenter(QJsonObject const &a_config);
		~PixelBasedAdaptiveSegmenter();

		void process(std::vector<_data> &_data);

	};

} // namespace Subtractors