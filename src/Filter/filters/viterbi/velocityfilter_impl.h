#ifndef VELOCITY_FILTER_IMPL_H
#define VELOCITY_FILTER_IMPL_H

#include "viterbi.h"

class QJsonObject;

namespace viterbi
{
    class VelocityFilter_impl: public Viterbi_impl
    {
        public:
            VelocityFilter_impl(QJsonObject const &a_config);
            ~VelocityFilter_impl();
            void forwardStep() override;
            //void backwardStep() ;
            cv::Mat getOutput() override;

        private:
            std::deque<cv::Mat> m_images;
            std::deque<cv::Mat> m_VAL;
            
    };

} // namespace viterbi

#endif // VELOCITY_FILTER_IMPL_H
