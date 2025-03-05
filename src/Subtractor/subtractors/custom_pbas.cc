#include "custom_pbas.h"

//#define DEBUG
//#define DEBUG_OPENCV

#include <QJsonObject>

constexpr auto ENABLE_INPUT_BLUR{ "EnableInputBlur" };
constexpr auto ENABLE_OUTPUT_BLUR{ "EnableOutputBlur" };
constexpr auto ALPHA{ "Alpha" };
constexpr auto BETA{ "Beta" };
constexpr auto N_N{ "N_N" };
constexpr auto RAUTE_MIN{ "Raute_min" };
constexpr auto R_INCDEC{ "R_incdec" };
constexpr auto R_LOWER{ "R_lower" };
constexpr auto R_SCALE{ "R_scale" };
constexpr auto T_DEC{ "T_dec" };
constexpr auto T_INC{ "T_inc" };
constexpr auto T_INIT{ "T_init" };
constexpr auto T_LOWER{ "T_lower" };
constexpr auto T_UPPER{ "T_upper" };


Subtractors::PixelBasedAdaptiveSegmenter::PixelBasedAdaptiveSegmenter(QJsonObject const &a_config) :
enableInputBlur{ a_config[ENABLE_INPUT_BLUR].toBool() },
enableOutputBlur{ a_config[ENABLE_OUTPUT_BLUR].toBool() },
alpha{ a_config[ALPHA].toDouble() },
beta{ a_config[BETA].toDouble() },
N{ a_config[N_N].toInt() },
Raute_min{ a_config[RAUTE_MIN].toInt() },
R_incdec{ a_config[R_INCDEC].toDouble() },
R_lower{ a_config[R_LOWER].toInt() },
R_scale{ a_config[R_SCALE].toInt() },
T_dec{ a_config[T_DEC].toDouble() },
T_inc{ a_config[T_INC].toInt() },
T_init{ a_config[T_INIT].toInt() },
T_lower{ a_config[T_LOWER].toInt() },
T_upper{ a_config[T_UPPER].toInt() }
{
	#ifdef DEBUG
	Logger->debug("Subtractors::PixelBasedAdaptiveSegmenter::PixelBasedAdaptiveSegmenter()");
	#endif
}

Subtractors::PixelBasedAdaptiveSegmenter::~PixelBasedAdaptiveSegmenter()
{
	#ifdef DEBUG
	Logger->debug("Subtractors::PixelBasedAdaptiveSegmenter::~PixelBasedAdaptiveSegmenter()");
	#endif
}

void Subtractors::PixelBasedAdaptiveSegmenter::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	Logger->debug("Subtractors::PixelBasedAdaptiveSegmenter::process()");
	#endif
	assert(_data[0].processing.empty() == false);

	if (m_firstTime)
	{
		pbas.setAlpha(alpha);
		pbas.setBeta(beta);
		pbas.setN(N);
		pbas.setRaute_min(Raute_min);
		pbas.setR_incdec(R_incdec);
		pbas.setR_lower(R_lower);
		pbas.setR_scale(R_scale);
		pbas.setT_dec(T_dec);
		pbas.setT_inc(T_inc);
		pbas.setT_init(T_init);
		pbas.setT_lower(T_lower);
		pbas.setT_upper(T_upper);
		m_firstTime = false;
	}
	else
	{
		cv::Mat img_input_new;
		if (enableInputBlur)
		{
			cv::GaussianBlur(_data[0].processing, img_input_new, cv::Size(5, 5), 1.5);
		}
		else
		{
			_data[0].processing.copyTo(img_input_new);
		}

		pbas.process(&img_input_new, &img_foreground);
		if (enableOutputBlur)
		{
			cv::medianBlur(img_foreground, img_foreground, 5);
		}

		#ifdef DEBUG_OPENCV
		cv::imshow("_data[0].processing", _data[0].processing);
		cv::waitKey(1);
		#endif
		img_foreground.copyTo(_data[0].processing);
	}

	#ifdef DEBUG
	Logger->debug("Subtractors::PixelBasedAdaptiveSegmenter::process() done");
	#endif
}
