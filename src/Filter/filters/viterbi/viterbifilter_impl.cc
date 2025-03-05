#include "viterbifilter_impl.h"

#define DEBUG_OPENCV
#define DEBUG

constexpr auto CLUSTER_WIDTH{ "ClusterWidth" };
constexpr auto CLUSTER_HEIGHT{ "ClusterHeight" };
constexpr auto SHIFT_WIDTH{ "ShiftWidth" };
constexpr auto SHIFT_HEIGHT{ "ShiftHeight" };
constexpr auto VELOCITY_FILTER{ "VelocityFilter" };


namespace viterbi
{
	ViterbiFilter_impl::ViterbiFilter_impl(QJsonObject const &a_config)
	: Viterbi_impl(a_config)
	, m_clusterWidth(a_config[CLUSTER_WIDTH].toInt())
	, m_clusterHeight(a_config[CLUSTER_HEIGHT].toInt())
	, m_iShift(a_config[SHIFT_WIDTH].toInt())
	, m_jShift(a_config[SHIFT_HEIGHT].toInt())
	, m_velocityFilter(a_config[VELOCITY_FILTER].toBool())
	{
		if (m_clusterWidth < 5 || m_clusterHeight < 5 )
		{
			m_clusterWidth = 10;
			m_clusterHeight = 10;
		}
		if (m_iShift < 1 || m_jShift < 1 )
		{
			m_iShift = m_clusterWidth;
			m_jShift = m_clusterHeight;
		}
		
		#ifdef DEBUG
		//Logger->debug("ViterbiFilter_impl::ViterbiFilter_impl()");
		//Logger->debug("m_clusterWidth:{}", m_clusterWidth);
		//Logger->debug("m_clusterHeight:{}", m_clusterHeight);
		//Logger->debug("m_iShift:{}", m_iShift);
		//Logger->debug("m_jShift:{}", m_jShift);
		//Logger->debug("m_velocityFilter:{:f}", m_velocityFilter);
		#endif
	}

	ViterbiFilter_impl::~ViterbiFilter_impl()
	{
		#ifdef DEBUG
		//Logger->debug("{}()", __FUNCTION__);
		#endif
	}

	void ViterbiFilter_impl::nextIteration(std::vector<_data> &_data)
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
	
	void ViterbiFilter_impl::backwardStep()
	{
		#ifdef TIMER
		m_timerBackward.start();
		#endif
		std::vector<cv::Point> min;
		std::vector<cv::Point> max;

		if (m_kernelsSize > m_minimumKernelSize && m_kernelsSizeVAL > m_minimumKernelSize)
		{
			for (int z = 0; z < m_kernelsSize; z++)
			{
				#ifdef TIMER
				m_timer.start();
				#endif
				#ifdef DEBUG
				//Logger->debug("ViterbiFilter_impl::backwardStep() m_kernels[z].size():{}", m_kernels[z].size());
				#endif
				
				for (int kernel = 0; kernel < m_kernels[z].size(); kernel++)
				{
					if (z == (m_kernelsSize - 1))
					{
						double minVal;
						double maxVal;
						cv::Point minLoc;
						cv::Point maxLoc;
						cv::Rect rect(0, 0, m_clusterWidth, m_clusterHeight);
						cv::Mat roi = m_kernelsVAL[z][kernel](rect);
						cv::Mat roiThresh;
						
						double maxValInt = 255.0 * m_range;
						if(m_velocityFilter)
						{
							if (m_normalize)
							{
								cv::normalize(roi, roi, 0, 255, cv::NORM_MINMAX, CV_8UC1);
							}
							if (roi.type() != CV_8UC1)
							{
								roi.convertTo(roi, CV_8UC1);
							}
							m_viterbiOut.push_back(roi);
						}
						else
						{
							cv::minMaxLoc(roi, &minVal, &maxVal, &minLoc, &maxLoc);
							//double tresh = maxValInt * m_threshold;
							double tresh = maxVal-200;
							cv::threshold(roi, roiThresh, int(tresh), 255, 0);
							roiThresh.convertTo(roiThresh, CV_8UC1);
							m_viterbiOut.push_back(roiThresh);
							min.push_back(minLoc);
							max.push_back(maxLoc);
						}
						#ifdef DEBUG_OPENCV
						if(kernel==0)
						{
							#ifdef DEBUG
							//Logger->debug("minVal:{:f}", minVal);
							//Logger->debug("maxVal:{:f}", maxVal);
							//Logger->debug("maxValInt:{:f}", maxValInt);
							#endif

							cv::Mat preview_ViterbiOut;
							cv::resize(m_viterbiOut[0], preview_ViterbiOut, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
							cv::imshow("preview_ViterbiOut", preview_ViterbiOut);	

							cv::Mat preview_m_kernelsVAL;
							cv::resize(m_kernelsVAL[0][0], preview_m_kernelsVAL, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
							preview_m_kernelsVAL.convertTo(preview_m_kernelsVAL, CV_32F, 1.0 / 255, 0);
							cv::imshow("preview_m_kernelsVAL", preview_m_kernelsVAL);	

							cv::Mat preview_roi;
							cv::resize(roi, preview_roi, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
							preview_roi.convertTo(preview_roi, CV_32F, 1.0 / 255, 0);
							cv::imshow("preview_roi", preview_roi);	

							cv::Mat m_VAL_CAT;
							cv::Mat m_VAL_CAT_Thresh;
							for (int i = 0; i < m_kernelsVAL.size(); i++)
							{

								cv::Mat m_VAL_temp;
								cv::Mat m_VAL_temp_color;
								//m_kernelsVAL[i][0].convertTo(m_VAL_temp_color, CV_8UC3, 1, 0);
								cv::normalize(m_kernelsVAL[i][0], m_VAL_temp, 0, 255, cv::NORM_MINMAX, CV_8UC1);
								cv::cvtColor(m_VAL_temp, m_VAL_temp_color, cv::COLOR_GRAY2BGR);

								if (i==0)
								{
									cv::hconcat(m_VAL_temp_color, m_VAL_temp_color, m_VAL_CAT);
									cv::threshold(m_VAL_CAT, m_VAL_CAT_Thresh, maxVal * 0.9, int(255), 0);
								}
								else
								{
									cv::hconcat(m_VAL_CAT, m_VAL_temp_color, m_VAL_CAT);
									cv::threshold(m_VAL_CAT, m_VAL_CAT_Thresh, maxVal * 0.9, int(255), 0);
								}
							}
									

							cv::resize(m_VAL_CAT, m_VAL_CAT, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
							cv::imshow("VAL", m_VAL_CAT);
							cv::resize(m_VAL_CAT_Thresh, m_VAL_CAT_Thresh, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
							cv::imshow("VAL_Thresh", m_VAL_CAT_Thresh);
						}
						#endif
					}
				}
				#ifdef TIMER
				m_timer.stop();
				double time = m_timer.getTimeMilli();
				//Logger->debug("ViterbiFilter_impl() backwardStep range:{}:    {:f}", z, time);
				m_timer.reset();
				#endif
			}
		}
		
		#ifdef TIMER
		m_timerBackward.stop();
		double time = m_timerBackward.getTimeMilli();
		//Logger->debug("ViterbiFilter_impl() m_timerBackward {:f}", time);
		#endif
	}

	cv::Mat ViterbiFilter_impl::getOutput()
	{

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
		return ViterbiOutGlobal;
	}

	void ViterbiFilter_impl::absFilter(cv::Mat& kernel, int clusterHeight, int clusterWidth, double mean)
	{
		for (int ii = 0; ii < clusterHeight; ii++)
		{
			for (int jj = 0; jj < clusterWidth; jj++)
			{
				auto meanInt = static_cast<int>(std::round(mean));
				if (kernel.at<unsigned char>(cv::Point(jj, ii)) > meanInt)
					kernel.at<unsigned char>(cv::Point(jj, ii)) = kernel.at<unsigned char>(cv::Point(jj, ii)) - meanInt;
				else
					kernel.at<unsigned char>(cv::Point(jj, ii)) = meanInt - kernel.at<unsigned char>(cv::Point(jj, ii));
			}
		}
	}

	void ViterbiFilter_impl::addKernelsToDeque(int range, std::deque<std::vector<cv::Mat>>& kernelsDeque, std::vector<cv::Mat>& kernels)
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

	void ViterbiFilter_impl::opencvPreviewForwardKernel(cv::Mat& input, cv::Mat& kernel)
	{
		cv::Mat previewKernel, previewKernelAfterAbsFilter;
		cv::resize(input, previewKernel, cv::Size(500, 500), 0, 0, cv::INTER_NEAREST);
		cv::resize(kernel, previewKernelAfterAbsFilter, cv::Size(500, 500), 0, 0, cv::INTER_NEAREST);
		cv::Mat previewImage;
		cv::hconcat(previewKernel, previewKernelAfterAbsFilter, previewImage);
		cv::imshow("previewKernel + AfterAbsFilter", previewImage);	
	}

	void ViterbiFilter_impl::checkSizes(std::deque<std::vector<cv::Mat>>& kernels, int z, int kernel, int clusterWidth, int clusterHeight)
	{
		#ifdef DEBUG
		if(kernels[z][kernel].cols != clusterWidth)
		{
			//Logger->error("m_kernelsVAL[{}][{}].cols != {}", z, kernel, clusterWidth);
		}
		if(kernels[z][kernel].rows != clusterHeight)
		{
			//Logger->error("m_kernelsVAL[{}][{}].rows != {}", z, kernel, clusterHeight);
		}
		#endif
	}

	void ViterbiFilter_impl::forwardStep()
	{
		#ifdef DEBUG
		//Logger->debug("{}() m_iShift:{}", __FUNCTION__, m_iShift);
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

		std::vector<cv::Mat> kernels;
		std::vector<cv::Mat> kernelsVAL;

		for (int i = 0; i <= m_height - m_clusterHeight; i = i + m_iShift)
		{
			#ifdef DEBUG
			//Logger->debug("{}() width:{}, height:{}", __FUNCTION__, m_width, m_height);
			#endif
			for (int j = 0; j <= m_width - m_clusterWidth; j = j + m_jShift)
			{
				cv::Rect rect(j, i, m_clusterWidth, m_clusterHeight);
				cv::Mat kernel = cloneOfLastImage(rect);
				cv::Scalar mean = cv::mean(kernel);

				if (m_absFilter)
					absFilter(kernel, m_clusterHeight, m_clusterWidth, mean[0]);

				#ifdef DEBUG_OPENCV
				if(i==0 && j==0)
					opencvPreviewForwardKernel(preview(rect), kernel);
				#endif
				
				kernels.push_back(kernel.clone());
				cv::Mat VAL = cv::Mat(m_clusterHeight, m_clusterWidth, CV_16UC1, cv::Scalar(0));
				kernelsVAL.push_back(VAL.clone());
			}
		}
		addKernelsToDeque(m_range, m_kernels, kernels);
		addKernelsToDeque(m_range, m_kernelsVAL, kernelsVAL);

		m_kernelsSize = m_kernels.size();
		m_kernelsSizeVAL = m_kernelsVAL.size();
		m_minimumKernelSize = 5;
		#ifdef DEBUG
		//Logger->debug("{}() m_kernels.size():{}, m_kernelsVAL.size():{}, m_minimumKernelSize:{}", 
		//				__FUNCTION__, m_kernels.size(), m_kernelsVAL.size(), m_minimumKernelSize);
		#endif
		
		std::vector<int> temp;
		temp.reserve(m_treck * 2 + 1);

		#ifdef TIMER
		m_timer.stop();
		double time = m_timer.getTimeMilli();
		//Logger->debug("ViterbiFilter_impl::forwardStep() Init time:{:f}", time);
		m_timer.reset();
		#endif

		if (m_kernelsSize > m_minimumKernelSize && m_kernelsSizeVAL > m_minimumKernelSize)
		{
			for (int z = 0; z < m_kernelsSize; z++)
			{ 
				// wielokosc kolejnki
				#ifdef TIMER
				m_timer.start();
				#endif
				#ifdef DEBUG
				////Logger->debug("ViterbiFilter_impl::forwardStep() m_kernels[z].size():{}", m_kernels[z].size());
				#endif
				for (int kernel = 0; kernel < m_kernels[z].size(); kernel++)
				{
					// ilosc kerneli
					#ifdef DEBUG
					////Logger->debug("ViterbiFilter_impl::forwardStep() kernel[{}]:{}", z, kernel);
					#endif
					if (z == 0)
					{
						for (int i = 0; i < m_clusterHeight; i++)
						{
							for (int j = 0; j < m_clusterWidth; j++)
							{
								checkSizes(m_kernelsVAL, z, kernel, m_clusterWidth, m_clusterHeight);
								m_kernelsVAL[z][kernel].at<unsigned short>(cv::Point(j, i)) =
								(unsigned short)(m_kernels[z][kernel].at<unsigned char>(cv::Point(j, i)));
							}
						}
					}
					else
					{
						for (int i = m_treck; i < m_clusterHeight - m_treck; i++)
						{
							for (int j = m_treck; j < m_clusterWidth - m_treck; j++)
							{
								temp.clear();
								for (int ii = -m_treck; ii <= m_treck; ii++)
								{
									for (int jj = -m_treck; jj <= m_treck; jj++)
									{
										temp.push_back(m_kernelsVAL[z - 1][kernel].at<unsigned short>(cv::Point(j + jj, i + ii)));
									}
								}

								std::vector<int>::iterator result;
								result = std::max_element(temp.begin(), temp.end());
								int distance = std::distance(temp.begin(), result);
								m_kernelsVAL[z][kernel].at<unsigned short>(cv::Point(j, i)) =
									( unsigned short)((m_kernels[z][kernel].at<unsigned char>(cv::Point(j, i)) + 
									temp[distance]));
							}
						}
					}
				}
				#ifdef TIMER
				m_timer.stop();
				time = m_timer.getTimeMilli();
				//Logger->debug("ViterbiFilter_impl() range:{}:    {:f}", z, time);
				#endif
			}
		}

		
		#ifdef DEBUG_OPENCV
			cv::Mat preview_input;
			cv::resize(preview, preview_input, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
			cv::imshow("preview_input", preview_input);	
			cv::waitKey(0);
		#endif

		#ifdef TIMER
			m_timerForward.stop();
			time = m_timerForward.getTimeMilli();
			//Logger->debug("ViterbiFilter_impl() m_timerForward {:f}", time);
		#endif
	}

} // namespace viterbi