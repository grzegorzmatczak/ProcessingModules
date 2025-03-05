#include "viterbi.h"

#define DEBUG

constexpr auto TRECK{ "Treck" };
constexpr auto RANGE{ "Range" };
constexpr auto ABS_FILTER{ "AbsFilter" };
constexpr auto THRESHOLD{ "Threshold" };
constexpr auto NORMALIZE{ "Normalize" };
constexpr auto BITWISE_NOT{ "BitwiseNot" };

namespace viterbi
{
    Viterbi_impl::Viterbi_impl(QJsonObject const &a_config)
    : m_firstTime(true)
	, m_width(0)
	, m_height(0)
	, m_treck(a_config[TRECK].toInt())
	, m_range(a_config[RANGE].toInt())
	, m_absFilter(a_config[ABS_FILTER].toBool())
    , m_threshold(a_config[THRESHOLD].toDouble())
    , m_normalize(a_config[NORMALIZE].toBool())
    , m_bitwisenot(a_config[BITWISE_NOT].toBool())
	{
		#ifdef DEBUG
        //Logger->debug("Viterbi_impl::Viterbi_impl()");
		//Logger->debug("m_treck:{}", m_treck);
		//Logger->debug("m_range:{}", m_range);
		//Logger->debug("m_absFilter:{}", m_absFilter);
		#endif
	}

    Viterbi_impl::~Viterbi_impl()
    {
        #ifdef DEBUG
		//Logger->debug("Viterbi_impl::~Viterbi_impl()");
		#endif
    }

    void Viterbi_impl::firstTime(cv::Mat& input)
    {
        m_firstTime = false;
        #ifdef DEBUG
            ////Logger->debug("Viterbi_impl::firstTime() size:{}x{}", input.cols, input.rows);
        #endif
        #ifdef DEBUG_OPENCV
            cv::imshow("input", input);
        #endif
        m_width = input.cols;
        m_height = input.rows;
    }

    std::string Viterbi_impl::type2str(int type)
    {
        std::string r;

        uchar depth = type & CV_MAT_DEPTH_MASK;
        uchar chans = 1 + (type >> CV_CN_SHIFT);

        switch (depth) {
            case CV_8U: r = "8U"; break;
            case CV_8S: r = "8S"; break;
            case CV_16U: r = "16U"; break;
            case CV_16S: r = "16S"; break;
            case CV_32S: r = "32S"; break;
            case CV_32F: r = "32F"; break;
            case CV_64F: r = "64F"; break;
            default: r = "User"; break;
        }

        r += "C";
        r += (chans + '0');

        return r;
    }

    void Viterbi_impl::showDebugImages(int z, int kernel)
	{
        /*
		//Logger->debug("ViterbiFilter_impl::showDebugImages()");

		int histSize = 256;
		float range[] = { 0, 256 };
		const float *histRange = { range };
		bool uniform = true;
		bool accumulate = false;
		cv::Mat grey_hist;
		// Compute the histograms:
		cv::calcHist(&m_kernelsVAL[z][kernel], 1, 0, cv::Mat(), grey_hist, 1, &histSize, &histRange, uniform,
								accumulate);
		// Draw the histograms
		int hist_w = 512; 	
		int hist_h = 400;
		int bin_w = cvRound((double)hist_w / histSize);
		cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));
		// Normalize the result to [ 0, histImage.rows ]
		normalize(grey_hist, grey_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
		// Draw for each channel
		for (int i = 1; i < histSize; i++)
		{
			line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(grey_hist.at<float>(i - 1))),
					cv::Point(bin_w * (i), hist_h - cvRound(grey_hist.at<float>(i))), cv::Scalar(120, 120, 120), 1, 8, 0);
		}
		imshow("Hist", histImage);
		//std::string type = type2str(roiThresh.type());
		////Logger->debug("ViterbiFilter_impl::showDebugImages() type:{}", type);
        */
	}

} // namespace viterbi
