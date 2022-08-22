#ifndef VITERBI_GLOBAL_IMPL_H
#define VITERBI_GLOBAL_IMPL_H

#include "viterbi.h"
#include "processing.h"

class QJsonObject;

namespace viterbi
{
	class ViterbiGlobal_impl: public Viterbi_impl
	{
		public:
			ViterbiGlobal_impl(QJsonObject const &a_config);
			~ViterbiGlobal_impl();
			void forwardStep() override; 
			void backwardStep();
			cv::Mat getOutput() override;
			void nextIteration(std::vector<_data> &_data);
			
		private:
			void absFilter(cv::Mat& kernel, int clusterHeight, int clusterWidth, double mean);
			void opencvPreviewForwardKernel(cv::Mat& input, cv::Mat& kernel);
			void addKernelsToDeque(int range, std::deque<cv::Mat>& kernelsDeque, cv::Mat& kernels);
			void checkSizes(cv::Mat& input, int width, int height);

		private:
			std::deque<cv::Mat> m_kernels; // CV_16UC1
			std::deque<cv::Mat> m_kernelsVAL; // CV_16UC1
			cv::Mat mVelocityFilterOutput;
			int m_clusterWidth{50};
			int m_clusterHeight{50};
			int m_iShift{25};
			int m_jShift{25};
			int m_kernelsSize{0};
			int m_kernelsSizeVAL{0};
			int m_minimumKernelSize{2};
			bool m_velocityFilter{false};
			int mCounter{0};
			std::deque<cv::Mat> m_gt;
			std::deque<cv::Mat> m_inputs;
	};
} // namespace viterbi

#endif // VITERBI_GLOBAL_IMPL_H
