#include "viterbiglobal_impl.h"

#define DEBUG_OPENCV
#define DEBUG

constexpr auto CLUSTER_WIDTH{ "ClusterWidth" };
constexpr auto CLUSTER_HEIGHT{ "ClusterHeight" };
constexpr auto SHIFT_WIDTH{ "ShiftWidth" };
constexpr auto SHIFT_HEIGHT{ "ShiftHeight" };
constexpr auto VELOCITY_FILTER{ "VelocityFilter" };


namespace viterbi
{
	ViterbiGlobal_impl::ViterbiGlobal_impl(QJsonObject const &a_config)
	: Viterbi_impl(a_config)
	, m_velocityFilter(a_config[VELOCITY_FILTER].toBool())
	{
		#ifdef DEBUG
		//Logger->debug("m_velocityFilter:{:f}", m_velocityFilter);
		#endif
	}

	ViterbiGlobal_impl::~ViterbiGlobal_impl()
	{
		#ifdef DEBUG
		//Logger->debug("{}()", __FUNCTION__);
		#endif
	}

	void ViterbiGlobal_impl::nextIteration(std::vector<_data> &_data)
	{
		mCounter++;
		#ifdef DEBUG
		//Logger->debug("{}() mCounter:{}", __FUNCTION__, mCounter);
		#endif
		if(mCounter < 30)
		{
			m_inputs.push_back(_data[0].processing.clone());
			m_gt.push_back(_data[1].processing.clone());
			//Logger->debug("{}() push images to deque", __FUNCTION__);

			_data[0].processing = m_inputs[0];
			_data[1].processing = m_gt[0];
		}
		else
		{
			
			m_inputs.pop_front();
			m_inputs.push_back(_data[0].processing.clone());
			m_gt.pop_front();
			m_gt.push_back(_data[1].processing.clone());

			_data[0].processing = m_inputs[0];
			_data[1].processing = m_gt[0];

			//Logger->debug("{}() push/pop images to deque", __FUNCTION__);
		}

	}
	
	void ViterbiGlobal_impl::backwardStep()
	{
		#ifdef TIMER
		m_timerBackward.start();
		#endif
		//std::vector<cv::Point> min;
		//std::vector<cv::Point> max;

		double minVal;
		double maxVal;
		cv::Point minLoc;
		cv::Point maxLoc;
		cv::Rect rect(0, 0, m_clusterWidth, m_clusterHeight);
		cv::Mat roi = m_kernelsVAL.back().clone();
		cv::Mat roiThresh;
		cv::Mat mVelocityFilterOutputThresh;
		cv::Mat colorPreview = cv::Mat::zeros(roi.size(), CV_8UC3);
		
		double maxValInt = 255.0 * m_kernelsVAL.size();
		
		//if (m_normalize)
		//{
		cv::normalize(roi, mVelocityFilterOutput, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		cv::threshold(mVelocityFilterOutput, mVelocityFilterOutputThresh, 250, 255, 0);
		//}
		//if (roi.type() != CV_8UC1)
		//{
		//	roi.convertTo(mVelocityFilterOutput, CV_8UC1);
		//}
	
		cv::minMaxLoc(roi, &minVal, &maxVal, &minLoc, &maxLoc);
		double tresh = maxVal - 200;
		cv::threshold(roi, roiThresh, int(tresh), 255, 0);
		//roiThresh.convertTo(mVelocityFilterOutput, CV_8UC1);
		//min.push_back(minLoc);
		//max.push_back(maxLoc);

		//Logger->debug("{}() maxVal:{:f}, point({},{})", __FUNCTION__, maxVal, maxLoc.x, maxLoc.y);
		//}
		cv::Vec3b color{0,255,0};
		colorPreview.at<cv::Vec3b>(maxLoc) = color;
		#ifdef DEBUG_OPENCV
			cv::Mat preview_input;
			cv::resize(colorPreview, preview_input, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
			cv::imshow("colorPreview", preview_input);	

			cv::Mat velocityFilterOutputPreview;
			cv::resize(mVelocityFilterOutput, velocityFilterOutputPreview, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
			cv::imshow("mVelocityFilterOutput", velocityFilterOutputPreview);	

			cv::Mat velocityFilterOutputThreshPreview;
			cv::resize(mVelocityFilterOutputThresh, velocityFilterOutputThreshPreview, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
			cv::imshow("mVelocityFilterOutputThresh", velocityFilterOutputThreshPreview);

			cv::waitKey(0);
		#endif


		#ifdef TIMER
		m_timerBackward.stop();
		double time = m_timerBackward.getTimeMilli();
		//Logger->debug("ViterbiGlobal_impl() m_timerBackward {:f}", time);
		#endif
	}

	cv::Mat ViterbiGlobal_impl::getOutput()
	{
/*
		#ifdef DEBUG
		//Logger->debug("{}()", __FUNCTION__);
		#endif

		cv::Mat ViterbiOutGlobal = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(0));

		if (m_kernels.size() > m_minimumKernelSize && m_kernelsVAL.size() > m_minimumKernelSize)
		{
			int iter = 0;
			for (int i = 0; i <= m_height - m_clusterHeight; i = i + m_iShift)
			{
				for (int j = 0; j <= m_width - m_clusterWidth; j = j + m_jShift)
				{
					cv::Rect rect(j, i, m_clusterWidth, m_clusterHeight);
					cv::Mat kernelOut = ViterbiOutGlobal(rect);

					if (iter >= m_viterbiOut.size())
					{
						break;
					}
					if (kernelOut.cols == m_viterbiOut[iter].cols)
					{
						cv::add(kernelOut, m_viterbiOut[iter], kernelOut);
						kernelOut.copyTo(ViterbiOutGlobal(rect));
					}
					iter++;
					if (iter >= m_viterbiOut.size())
					{
						break;
					}
				}
			}
		}
		
		if(m_bitwisenot)
		{
			cv::bitwise_not(ViterbiOutGlobal, ViterbiOutGlobal);
		}

		m_viterbiOut.clear();
		#ifdef DEBUG_OPENCV
		cv::Mat preview_input;
		cv::resize(ViterbiOutGlobal, preview_input, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
		cv::imshow("ViterbiOutGlobal", preview_input);	
		#endif
		
		*/
		return mVelocityFilterOutput;
	}

	void ViterbiGlobal_impl::absFilter(cv::Mat& kernel, int height, int width, double mean)
	{
		for (int ii = 0; ii < height; ii++)
		{
			for (int jj = 0; jj < width; jj++)
			{
				auto meanInt = static_cast<unsigned short>(std::round(mean));
				if (kernel.at<unsigned char>(cv::Point(jj, ii)) > meanInt)
					kernel.at<unsigned char>(cv::Point(jj, ii)) = kernel.at<unsigned char>(cv::Point(jj, ii)) - meanInt;
				else
					kernel.at<unsigned char>(cv::Point(jj, ii)) = meanInt - kernel.at<unsigned char>(cv::Point(jj, ii));
			}
		}
	}

	void ViterbiGlobal_impl::addKernelsToDeque(int range, std::deque<cv::Mat>& kernelsDeque, cv::Mat& kernels)
	{
		if (kernelsDeque.size() > range)
		{
			#ifdef DEBUG
			//Logger->debug("{}() pop/push", __FUNCTION__);
			#endif
			kernelsDeque.pop_front();
			kernelsDeque.push_back(kernels);
		}
		else
		{
			#ifdef DEBUG
			//Logger->debug("{}() push", __FUNCTION__);
			#endif
			kernelsDeque.push_back(kernels);
			
		}
	}

	void ViterbiGlobal_impl::opencvPreviewForwardKernel(cv::Mat& input, cv::Mat& kernel)
	{
		cv::Mat previewKernel, previewKernelAfterAbsFilter;
		cv::resize(input, previewKernel, cv::Size(500, 500), 0, 0, cv::INTER_NEAREST);
		cv::resize(kernel, previewKernelAfterAbsFilter, cv::Size(500, 500), 0, 0, cv::INTER_NEAREST);
		cv::Mat previewImage;
		cv::hconcat(previewKernel, previewKernelAfterAbsFilter, previewImage);
		cv::imshow("previewKernel + AfterAbsFilter", previewImage);	
	}

	void ViterbiGlobal_impl::checkSizes(cv::Mat& input, int width, int height)
	{
		#ifdef DEBUG
		//if(input.cols != width)
			//Logger->error("cols != {}", width);
		//if(input.rows != height)
			//Logger->error("rows != {}", height);
		#endif
	}

	void ViterbiGlobal_impl::forwardStep()
	{
		#ifdef DEBUG
		//Logger->debug("{}() ", __FUNCTION__);
		#endif
		#ifdef TIMER
		m_timerForward.start();
		m_timer.start();
		#endif
		
		cv::Mat lastImage = m_inputs.back();
		cv::Mat cloneOfLastImage = (m_inputs.back()).clone();
		if (m_firstTime)
			firstTime(cloneOfLastImage);

		#ifdef DEBUG_OPENCV
		cv::Mat previewRGB;
		cv::Mat preview = (m_inputs.back()).clone();
		cv::cvtColor(cloneOfLastImage, previewRGB, cv::COLOR_GRAY2BGR);
		#endif

		cv::Mat VAL = cv::Mat(m_height, m_width, CV_16UC1, cv::Scalar(0));
		cv::Mat kernel = cloneOfLastImage.clone();
		if (kernel.channels() == 3)
			cv::cvtColor(kernel, kernel, cv::COLOR_BGR2GRAY);
		cv::Scalar mean = cv::mean(kernel);

		absFilter(kernel, m_height, m_width, mean[0]);

		addKernelsToDeque(m_range, m_kernels, kernel);
		addKernelsToDeque(m_range, m_kernelsVAL, VAL);

		#ifdef TIMER
		m_timer.stop();
		double time = m_timer.getTimeMilli();
		//Logger->debug("ViterbiFilter_impl::forwardStep() Init time:{:f}", time);
		m_timer.reset();
		#endif
		#ifdef TIMER
		m_timer.start();
		#endif
		if (m_kernelsVAL.size() > 5 && m_kernels.size() > 5)
		{
			for (int z = 0; z < m_kernelsVAL.size(); z++)
			{
				if (z == 0)
				{
					#ifdef DEBUG
					//Logger->debug("VelocityFilter_impl::forwardStep() z=0");
					#endif
					for (int i = 0; i < m_height; i++)
					{
						for (int j = 0; j < m_width; j++)
						{
							m_kernelsVAL[z].at<unsigned char>(cv::Point(j, i)) =
									(unsigned short)(m_kernels[z].at<unsigned char>(cv::Point(j, i)));
						}
					}
					#ifdef DEBUG
					////Logger->debug("m_kernelsVAL[z] = m_kernels[z].clone();");
					#endif
				}
				else
				{
					for (int i = m_treck; i < m_height - m_treck; i++)
					{
						for (int j = m_treck; j < m_width - m_treck; j++)
						{
							std::vector<int> temp;

							for (int ii = -m_treck; ii <= m_treck; ii++)
							{
								for (int jj = -m_treck; jj <= m_treck; jj++)
								{
									temp.push_back(m_kernelsVAL[z - 1].at<unsigned char>(cv::Point(j + jj, i + ii)));
								}
							}

							std::vector<int>::iterator result;
							result = std::max_element(temp.begin(), temp.end());
							int distance = std::distance(temp.begin(), result);
							
							m_kernelsVAL[z].at<unsigned char>(cv::Point(j, i)) =
									(unsigned short)((m_kernels[z].at<unsigned char>(cv::Point(j, i)) + 
									temp[distance]));
							//if(i==10 && j==10)
							//	//Logger->debug("{}() {}", __FUNCTION__, m_kernelsVAL[z].at<unsigned char>(cv::Point(j, i)));
						}
					}
				}
			}
		}

		#ifdef DEBUG_OPENCV
			cv::Mat preview_input;
			cv::resize(preview, preview_input, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
			cv::imshow("preview_input", preview_input);	
			cv::waitKey(0);
		#endif
		
		#ifdef TIMER
		m_timer.stop();
		time = m_timer.getTimeMilli();
		//Logger->debug("ViterbiFilter_impl() range:{}:    {:f}", z, time);
		#endif


	}

} // namespace viterbi