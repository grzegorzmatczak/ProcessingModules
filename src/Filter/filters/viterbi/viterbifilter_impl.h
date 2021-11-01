#ifndef VITERBI_FILTER_IMPL_H
#define VITERBI_FILTER_IMPL_H

#include "viterbi.h"


class QJsonObject;

namespace viterbi
{
    class ViterbiFilter_impl: public Viterbi_impl
    {
        public:
            ViterbiFilter_impl(QJsonObject const &a_config);
            ~ViterbiFilter_impl();
            void forwardStep(cv::Mat& input) override; 
            void backwardStep();
            cv::Mat getOutput() override;
        private:
            std::deque<std::vector<cv::Mat>> m_kernels;
            std::deque<std::vector<cv::Mat>> m_kernelsVAL;
            std::vector<cv::Mat> m_viterbiOut;


            int m_clusterWidth{};
            int m_clusterHeight{};

            int m_iShift{};
            int m_jShift{};

            int m_kernelsSize{};
            int m_kernelsSizeVAL{};
            int m_minimumKernelSize{};

            bool m_velocityFilter{};

    };

} // namespace viterbi

#endif // VITERBI_FILTER_IMPL_H
