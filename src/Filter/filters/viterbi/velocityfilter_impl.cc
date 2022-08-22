#include "velocityfilter_impl.h"

//#define DEBUG_OPENCV

namespace viterbi
{
	VelocityFilter_impl::VelocityFilter_impl(QJsonObject const &a_config)
	: Viterbi_impl(a_config)
	{
		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::VelocityFilter_impl()");
		#endif
	}

	VelocityFilter_impl::~VelocityFilter_impl()
	{
		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::~VelocityFilter_impl()");
		#endif
	}
	
	cv::Mat VelocityFilter_impl::getOutput()
	{
		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::getOutput() m_height:{}, m_width:{}", m_height, m_width);
		Logger->debug("VelocityFilter_impl::getOutput() [m_VAL.size() - 1]:{}", m_VAL.size() - 1);
		#endif
		//cv::Mat ViterbiOutGlobal = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(0));
		cv::Mat ViterbiOutGlobal = m_VAL[m_VAL.size() - 1];
		
		//double minVal;
		//double maxVal;
		//cv::Point minLoc;
		//cv::Point maxLoc;
		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::getOutput() minMaxLoc:");
		#endif
		//cv::minMaxLoc(ViterbiOutGlobal, &minVal, &maxVal, &minLoc, &maxLoc);
		double tresh = 255 * m_threshold;
		cv::threshold(ViterbiOutGlobal, ViterbiOutGlobal, int(tresh), 255, 0);

		if (m_normalize)
		{
			#ifdef DEBUG
			Logger->debug("VelocityFilter_impl::getOutput() m_normalize:");
			#endif
			cv::normalize(ViterbiOutGlobal, ViterbiOutGlobal, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		}
		if(m_bitwisenot)
		{
			cv::bitwise_not(ViterbiOutGlobal, ViterbiOutGlobal);
		}
		if (ViterbiOutGlobal.type() != CV_8UC1)
		{
			ViterbiOutGlobal.convertTo(ViterbiOutGlobal, CV_8UC1);
		}
		
		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::getOutput() return:");
		#endif

		return ViterbiOutGlobal;
	}


	void VelocityFilter_impl::forwardStep()
	{
		/*
		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::forwardStep()");
		#endif
		#ifdef TIMER
		m_timerForward.start();
		m_timer.start();
		#endif
		if (m_firstTime)
			firstTime(input);

		cv::Mat VAL = cv::Mat(m_height, m_width, CV_16UC1, cv::Scalar(0));
		cv::Mat kernel = input.clone();
		if (kernel.channels() == 3)
		{
			cv::cvtColor(kernel, kernel, cv::COLOR_BGR2GRAY);
		}
		cv::Scalar mean = cv::mean(kernel);

		#ifdef DEBUG
		std::string str = type2str(input.type());
		std::string str2 = type2str(kernel.type());
		Logger->debug("VelocityFilter_impl::getOutput() input:{}", str);
		Logger->debug("VelocityFilter_impl::getOutput() kernel:{}", str2);
		Logger->debug("VelocityFilter_impl::forwardStep() mean[0]:{}",mean[0]);
		Logger->debug("VelocityFilter_impl::forwardStep() width:{}, height:{}", m_width, m_height);
		#endif

		for (int i = 0; i < m_height; i++)
		{
			for (int j = 0; j < m_width; j++)
			{
				#ifdef DEBUG
				if(kernel.cols != m_width)
				{
					Logger->error("kernel.cols != {}", m_width);
				}
				if(kernel.rows != m_height)
				{
					Logger->error("kernel.rows != {}", m_height);
				}
				#endif
				if (kernel.at<unsigned char>(cv::Point(j, i)) > mean[0])
				{
					kernel.at<unsigned char>(cv::Point(j, i)) = kernel.at<unsigned char>(cv::Point(j, i)) - mean[0];
				}
				else
				{
					kernel.at<unsigned char>(cv::Point(j, i)) = mean[0] - kernel.at<unsigned char>(cv::Point(j, i));
				}
			}
		}


		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::forwardStep() m_images.size():{}", m_images.size());
		Logger->debug("VelocityFilter_impl::forwardStep() m_VAL.size():{}", m_VAL.size());
		#endif
		if (m_images.size() > m_range)
		{
			#ifdef DEBUG
			Logger->debug("VelocityFilter_impl::forwardStep() pop_front & push_back");
			#endif
			m_images.pop_front();
			m_images.push_back(kernel.clone());

			m_VAL.pop_front();
			m_VAL.push_back(VAL.clone());
		}
		else
		{
			#ifdef DEBUG
			Logger->debug("VelocityFilter_impl::forwardStep() push_back");
			#endif
			m_images.push_back(kernel);
			#ifdef DEBUG
			Logger->debug("VelocityFilter_impl::forwardStep() push_back VAL");
			#endif
			m_VAL.push_back(VAL);
		}
		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::forwardStep() m_images.size():{}, width:{}, height:{}", m_images.size(), m_width, m_height);
		#endif

		if (m_VAL.size() > 5 && m_images.size()>5)
		{
			#ifdef DEBUG
			Logger->debug("VelocityFilter_impl::forwardStep() m_VAL.size() > 5 && m_images.size()>5");
			#endif
			for (int z = 0; z < m_VAL.size(); z++)
			{
				if (z == 0)
				{
					//m_VAL[z] = m_images[z].clone();
					#ifdef DEBUG
					Logger->debug("VelocityFilter_impl::forwardStep() z=0");
					#endif
					for (int i = 0; i < m_height; i++)
					{
						for (int j = 0; j < m_width; j++)
						{
							m_VAL[z].at<unsigned short>(cv::Point(j, i)) =
									(unsigned short)(m_images[z].at<unsigned char>(cv::Point(j, i)));
						}
					}
					#ifdef DEBUG
					//Logger->debug("m_VAL[z] = m_images[z].clone();");
					#endif
				}
				else
				{
					for (int i = m_treck; i < m_height - m_treck; i++)
					{
						for (int j = m_treck; j < m_width - m_treck; j++)
						{
							std::vector<unsigned char> temp;
							#ifdef DEBUG
							//Logger->debug("create temp, z:{}, ixj:{}x{}",z,i,j);
							#endif

							for (int ii = -m_treck; ii <= m_treck; ii++)
							{
								for (int jj = -m_treck; jj <= m_treck; jj++)
								{
									#ifdef DEBUG
									//std::string ty = type2str(m_VAL[z - 1].type());
									//printf("Matrix: %s %dx%d \n", ty.c_str(), m_VAL[z - 1].cols, m_VAL[z - 1].rows);
									// Logger->critical("Viterbi::process() m_VAL type:{}", ty);
									//Logger->critical("Viterbi::process() temp.push_back:{}  ",m_VAL[z - 1].at<int>(cv::Point(j + jj, i + ii)));
									#endif
									temp.push_back(m_VAL[z - 1].at<unsigned short>(cv::Point(j + jj, i + ii)));

								}
							}

							std::vector<unsigned char>::iterator result;
							result = std::max_element(temp.begin(), temp.end());
							int distance = std::distance(temp.begin(), result);
							#ifdef DEBUG

							#endif
							
							m_VAL[z].at<unsigned short>(cv::Point(j, i)) =
									(unsigned short)((m_images[z].at<unsigned char>(cv::Point(j, i)) + 
									temp[distance]))/m_range;
						}
					}
				}
			}
		}

		#ifdef DEBUG_OPENCV
		cv::Mat m_images_CAT;
		cv::Mat m_images_CAT_thresh;

		for (int i = 0; i < m_images.size(); i++)
		{
			cv::Mat m_images_temp;
			cv::Mat m_images_temp_color;
			m_images[i].convertTo(m_images_temp, CV_8UC1,1,0);
	
			cv::cvtColor(m_images_temp, m_images_temp_color, cv::COLOR_GRAY2BGR);

			if (i == 0)
			{
				cv::hconcat(m_images_temp_color, m_images_temp_color, m_images_CAT);
				cv::threshold(m_images_CAT, m_images_CAT_thresh, mean[0]/2, int(255), 0);
			} else
			{
				cv::hconcat(m_images_CAT, m_images_temp_color, m_images_CAT);
				cv::threshold(m_images_CAT, m_images_CAT_thresh, mean[0]/2, int(255), 0);
			}
		}
		cv::resize(m_images_CAT, m_images_CAT, cv::Size(2800,300), 0, 0,
							cv::INTER_NEAREST);
		cv::imshow("input", m_images_CAT);

		cv::resize(m_images_CAT_thresh, m_images_CAT_thresh, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
		cv::imshow("input_thresh", m_images_CAT_thresh);

		cv::Mat m_VAL_CAT;
		cv::Mat m_VAL_CAT_Thresh;
		for (int i = 0; i < m_VAL.size(); i++)
		{

			cv::Mat m_VAL_temp;
			cv::Mat m_VAL_temp_color;
			//m_VAL[i].convertTo(m_VAL_temp, CV_8UC1, 1, 0);
			cv::normalize(m_VAL[i], m_VAL_temp, 0, 255, cv::NORM_MINMAX, CV_8UC1);
			cv::cvtColor(m_VAL_temp, m_VAL_temp_color, cv::COLOR_GRAY2BGR);

			if (i==0) {
				cv::hconcat(m_VAL_temp_color, m_VAL_temp_color, m_VAL_CAT);
				cv::threshold(m_VAL_CAT, m_VAL_CAT_Thresh, mean[0], int(255), 0);
			} else {
				cv::hconcat(m_VAL_CAT, m_VAL_temp_color, m_VAL_CAT);
				cv::threshold(m_VAL_CAT, m_VAL_CAT_Thresh, mean[0], int(255), 0);
			}
		}

		cv::resize(m_VAL_CAT, m_VAL_CAT, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
		cv::imshow("VAL", m_VAL_CAT);
		cv::resize(m_VAL_CAT_Thresh, m_VAL_CAT_Thresh, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
		cv::imshow("VAL_Thresh", m_VAL_CAT_Thresh);

		cv::Mat inputResized;
		cv::resize(input, inputResized, cv::Size(400, 300), 0, 0, cv::INTER_NEAREST);
		cv::imshow("input", inputResized);

		cv::Mat VALResized;
		cv::normalize(m_VAL[m_VAL.size() - 1], VALResized, 0, 255, cv::NORM_MINMAX, CV_8UC1);
		cv::resize(VALResized, VALResized, cv::Size(400, 300), 0, 0, cv::INTER_NEAREST);
		cv::imshow("VALResized", VALResized);

		cv::Mat InputResized2;
		cv::resize(m_images[m_images.size() - 1], InputResized2, cv::Size(400, 300), 0, 0, cv::INTER_NEAREST);
		cv::imshow("InputABS", InputResized2);

		cv::waitKey(0);
		#endif
		#ifdef DEBUG
		Logger->debug("VelocityFilter_impl::forwardStep() done");
		#endif

		#ifdef TIMER
		m_timer.stop();
		m_timerForward.stop();
		double time = m_timer.getTimeMilli();
		Logger->debug("VelocityFilter_impl::forwardStep() {:f}", time);
		m_timer.reset();
		#endif
		*/
	}
	

} // namespace viterbi