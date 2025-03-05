#ifndef VITERBI_H
#define VITERBI_H

#include <deque>
#include <vector>

#include <QJsonObject>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace viterbi
{

//#define DEBUG
//#define TIMER



    class Viterbi_impl
    {
        public:
            Viterbi_impl(QJsonObject const &a_config);
            ~Viterbi_impl();
            virtual void forwardStep() = 0;
            virtual cv::Mat getOutput() = 0;

        public:
            void firstTime(cv::Mat& input);
            std::string type2str(int type);
            void showDebugImages(int z, int kernel);

        public:
            bool m_firstTime;
            int m_width;
            int m_height;
            int m_treck{};
            int m_range{};
            bool m_absFilter{};
            double m_threshold{};
            bool m_normalize{};
            bool m_bitwisenot{};
            #ifdef TIMER
            cv::TickMeter m_timer;
            cv::TickMeter m_timerForward;
            cv::TickMeter m_timerBackward;
            #endif
    };

} // namespace viterbi

#endif // VITERBI_H
