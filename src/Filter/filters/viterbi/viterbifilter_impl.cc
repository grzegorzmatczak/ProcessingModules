#include "viterbifilter_impl.h"

//#define DEBUG_OPENCV

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
		if (m_iShift < 2 || m_jShift < 2 )
		{
			m_iShift = m_clusterWidth;
			m_jShift = m_clusterHeight;
		}
		
		#ifdef DEBUG
		Logger->debug("ViterbiFilter_impl::ViterbiFilter_impl()");
		Logger->debug("m_clusterWidth:{}", m_clusterWidth);
		Logger->debug("m_clusterHeight:{}", m_clusterHeight);
		Logger->debug("m_iShift:{}", m_iShift);
		Logger->debug("m_jShift:{}", m_jShift);
		Logger->debug("m_velocityFilter:{:f}", m_velocityFilter);
		#endif
	}

	ViterbiFilter_impl::~ViterbiFilter_impl()
	{
		#ifdef DEBUG
		Logger->debug("ViterbiFilter_impl::~ViterbiFilter_impl()");
		#endif
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
				//Logger->debug("ViterbiFilter_impl::backwardStep() kernel:{}", z);
				// wielokosc kolejnki
				#ifdef TIMER
				m_timer.start();
				#endif
				#ifdef DEBUG
				Logger->debug("ViterbiFilter_impl::backwardStep() m_kernels[z].size():{}", m_kernels[z].size());
				#endif
				
				for (int kernel = 0; kernel < m_kernels[z].size(); kernel++)
				{
					if (z == (m_kernelsSize - 1))
					{
						//Logger->debug("ViterbiFilter_impl::backwardStep() kernel[{}]:{}", z, kernel);
						double minVal;
						double maxVal;
						cv::Point minLoc;
						cv::Point maxLoc;
						cv::Rect rect(0, 0, m_clusterWidth, m_clusterHeight);
						cv::Mat roi = m_kernelsVAL[z][kernel](rect);
						cv::Mat roiThresh;

						if (m_normalize)
						{
							cv::normalize(roi, roi, 0, 255, cv::NORM_MINMAX, CV_8UC1);
						}

						if(m_velocityFilter)
						{
							m_viterbiOut.push_back(roi);
						}
						else
						{
							cv::minMaxLoc(roi, &minVal, &maxVal, &minLoc, &maxLoc);
							double tresh = maxVal * m_threshold;
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
							Logger->debug("minVal:{:f}", minVal);
							Logger->debug("maxVal:{:f}", maxVal);
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
				Logger->debug("ViterbiFilter_impl() backwardStep range:{}:    {:f}", z, time);
				m_timer.reset();
				#endif
			}
		}
		
		#ifdef TIMER
		m_timerBackward.stop();
		double time = m_timerBackward.getTimeMilli();
		Logger->debug("ViterbiFilter_impl() m_timerBackward {:f}", time);
		#endif
	}

	cv::Mat ViterbiFilter_impl::getOutput()
	{

		#ifdef DEBUG
		Logger->debug("ViterbiFilter_impl::getOutput()");
		#endif

		cv::Mat ViterbiOutGlobal = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(0));

		

		if (m_kernels.size() > m_minimumKernelSize && m_kernelsVAL.size() > m_minimumKernelSize)
		{
			int iter = 0;
			for (int i = 0; i <= m_width - m_clusterWidth; i = i + m_iShift)
			{
				for (int j = 0; j <= m_height - m_clusterHeight; j = j + m_jShift)
				{
					cv::Rect rect(i, j, m_clusterWidth, m_clusterHeight);
					cv::Mat kernelOut = ViterbiOutGlobal(rect);

					if (iter >= m_viterbiOut.size())
					{
						break;
					}
					if (m_normalize)
					{
						#ifdef DEBUG
						Logger->debug("VelocityFilter_impl::getOutput() m_normalize:");
						#endif
						cv::normalize(m_viterbiOut[iter], m_viterbiOut[iter], 0, 255, cv::NORM_MINMAX, CV_8UC1);
					}
					if (m_viterbiOut[iter].type() != CV_8UC1)
					{
						m_viterbiOut[iter].convertTo(m_viterbiOut[iter], CV_8UC1);
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
		m_viterbiOut.clear();
		return ViterbiOutGlobal;
	}

	void ViterbiFilter_impl::forwardStep(cv::Mat& input)
	{
		#ifdef DEBUG
		Logger->debug("ViterbiFilter_impl::forwardStep()");
		#endif
		#ifdef TIMER
		m_timerForward.start();
		m_timer.start();
		#endif
		if (m_firstTime)
			firstTime(input);

		cv::Mat clone = input.clone();
		#ifdef DEBUG_OPENCV
		cv::Mat preview;
		cv::cvtColor(input, preview, cv::COLOR_GRAY2BGR);
		#endif

		std::vector<cv::Mat> kernels;
		std::vector<cv::Mat> kernelsVAL;
		std::vector<cv::Scalar> vectorMean;

		for (int i = 0; i <= m_height - m_clusterHeight; i = i + m_iShift)
		{
			#ifdef DEBUG
			Logger->debug("VelocityFilter_impl::forwardStep() width:{}, height:{}", m_width, m_height);
			Logger->debug("ViterbiFilter_impl::forwardStep() i:{}", i);
			#endif
			
			for (int j = 0; j <= m_width - m_clusterWidth; j = j + m_jShift)
			{
				#ifdef DEBUG
				Logger->debug("ViterbiFilter_impl::forwardStep() j:{}", j);
				#endif
				cv::Rect rect(j, i, m_clusterWidth, m_clusterHeight);
				cv::Mat kernel = clone(rect);
				#ifdef DEBUG_OPENCV
				if(i==0 && j==0)
				{
					cv::Mat kernel2 = clone(rect);
					cv::Mat preview_kernel;
					cv::resize(kernel2, preview_kernel, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
					cv::imshow("preview_kernel", preview_kernel);	
				}
				#endif
				cv::Scalar mean = cv::mean(kernel);
				vectorMean.push_back(mean);

				if (m_absFilter)
				{
					
					for (int ii = 0; ii < m_clusterHeight; ii++)
					{
						for (int jj = 0; jj < m_clusterWidth; jj++)
						{
							if (kernel.at<unsigned char>(cv::Point(jj, ii)) > mean[0])
							{
								kernel.at<unsigned char>(cv::Point(jj, ii)) = kernel.at<unsigned char>(cv::Point(jj, ii)) - mean[0];
							}
							else
							{
								kernel.at<unsigned char>(cv::Point(jj, ii)) = mean[0] - kernel.at<unsigned char>(cv::Point(jj, ii));
							}
						}
					}
				}
				#ifdef DEBUG_OPENCV
				if(i==0 && j==0)
				{
					cv::Mat preview_kernelafter_abs;
					cv::resize(kernel, preview_kernelafter_abs, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
					cv::imshow("preview_kernelafter_abs", preview_kernelafter_abs);	
				}
				#endif
				kernels.push_back(kernel.clone());
				cv::Mat VAL = cv::Mat(m_clusterHeight, m_clusterWidth, CV_16UC1, cv::Scalar(0));
				kernelsVAL.push_back(VAL.clone());
			}
		}


		if (m_kernels.size() > m_range)
		{
			m_kernels.pop_front();
			m_kernels.push_back(kernels);
			m_kernelsVAL.pop_front();
			m_kernelsVAL.push_back(kernelsVAL);
			#ifdef DEBUG
			Logger->debug("ViterbiFilter_impl::forwardStep() pop/push");
			#endif
			
		}
		else
		{
			m_kernels.push_back(kernels);
			m_kernelsVAL.push_back(kernelsVAL);
			#ifdef DEBUG
			Logger->debug("ViterbiFilter_impl::forwardStep() push");
			#endif
			
		}
		m_kernelsSize = m_kernels.size();
		m_kernelsSizeVAL = m_kernelsVAL.size();
		m_minimumKernelSize = 5;
		#ifdef DEBUG
		Logger->debug("ViterbiFilter_impl::forwardStep() kernelsSize:{}", m_kernelsSize);
		Logger->debug("ViterbiFilter_impl::forwardStep() kernelsSizeVAL:{}", m_kernelsSizeVAL);
		Logger->debug("ViterbiFilter_impl::forwardStep() minimumKernelSize:{}", m_minimumKernelSize);
		#endif
		
		
		std::vector<unsigned char> temp;
		temp.reserve(m_treck * 2 + 1);

		#ifdef TIMER
		m_timer.stop();
		double time = m_timer.getTimeMilli();
		Logger->debug("ViterbiFilter_impl::forwardStep() Init time:{:f}", time);
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
				//Logger->debug("ViterbiFilter_impl::forwardStep() m_kernels[z].size():{}", m_kernels[z].size());
				#endif
				for (int kernel = 0; kernel < m_kernels[z].size(); kernel++)
				{
					// ilosc kerneli
					#ifdef DEBUG
					//Logger->debug("ViterbiFilter_impl::forwardStep() kernel[{}]:{}", z, kernel);
					#endif
					if (z == 0)
					{
						for (int i = 0; i < m_clusterHeight; i++)
						{
							for (int j = 0; j < m_clusterWidth; j++)
							{
								#ifdef DEBUG
								if(m_kernelsVAL[z][kernel].cols != m_clusterWidth)
								{
									Logger->error("m_kernelsVAL[{}][{}].cols != {}", z, kernel, m_clusterWidth);
								}
								if(m_kernelsVAL[z][kernel].rows != m_clusterHeight)
								{
									Logger->error("m_kernelsVAL[{}][{}].rows != {}", z, kernel, m_clusterHeight);
								}
								#endif
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
										temp.push_back(
											m_kernelsVAL[z - 1][kernel].at<unsigned short>(cv::Point(j + jj, i + ii)));
									}
								}

								std::vector<unsigned char>::iterator result;
								result = std::max_element(temp.begin(), temp.end());
								int distance = std::distance(temp.begin(), result);
								m_kernelsVAL[z][kernel].at<unsigned short>(cv::Point(j, i)) =
									( unsigned short)((m_kernels[z][kernel].at<unsigned char>(cv::Point(j, i)) + 
									//	temp[distance];
									temp[distance]));
							}
						}
					}
				}
				#ifdef TIMER
				m_timer.stop();
				time = m_timer.getTimeMilli();
				Logger->debug("ViterbiFilter_impl() range:{}:    {:f}", z, time);
				#endif
			}
		}

		
		#ifdef DEBUG_OPENCV
			cv::Mat preview_input;
			cv::resize(input, preview_input, cv::Size(1000, 1000), 0, 0, cv::INTER_NEAREST);
			cv::imshow("preview_input", preview_input);	
			cv::waitKey(0);
		#endif

		#ifdef TIMER
			m_timerForward.stop();
			time = m_timerForward.getTimeMilli();
			Logger->debug("ViterbiFilter_impl() m_timerForward {:f}", time);
		#endif
	}

} // namespace viterbi