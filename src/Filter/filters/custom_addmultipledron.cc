#include "custom_addmultipledron.h"
#include <QJsonObject>
/*
*   MarkerType:
		MARKER_CROSS = 0,            //!< A crosshair marker shape
		MARKER_TILTED_CROSS = 1, //!< A 45 degree tilted crosshair marker shape
		MARKER_STAR = 2,         //!< A star marker shape, combination of cross and tilted cross
		MARKER_DIAMOND = 3,      //!< A diamond marker shape
		MARKER_SQUARE = 4,       //!< A square marker shape
		MARKER_TRIANGLE_UP = 5,  //!< An upwards pointing triangle marker shape
		MARKER_TRIANGLE_DOWN = 6 //!< A downwards pointing triangle marker shape
*/
//#define DEBUG
//#define DEBUG_OPENCV

constexpr auto DRON_SIZE_MIN{ "SizeMin" };
constexpr auto DRON_SIZE_MAX{ "SizeMax" };
constexpr auto DRON_COLOR{ "Color" };
constexpr auto DRON_ROTATE{ "Rotate" };
constexpr auto DRON_VELOCITY_MIN{ "VelocityMin" };
constexpr auto DRON_VELOCITY_MAX{ "VelocityMax" };
constexpr auto DRON_PROB_OF_CHANGE_SIZE{ "ProbabilityOfChangeSize" };
constexpr auto DRON_PROB_OF_CHANGE_Velocity{ "ProbabilityOfChangeVelocity" };
constexpr auto DRON_PROB_OF_ROTATE{ "ProbabilityOfRotate" };
constexpr auto DRON_MARKER_TYPE{ "MarkerType" };
constexpr auto DRON_START_GT{ "StartGT" };
constexpr auto DRON_UNKNOWN_GT_COLOR{ "UnknownGTColor" };
constexpr auto DRON_RANDSEED{ "RandSeed" };
constexpr auto DRON_NOISE{ "Noise" };
constexpr auto DRON_CONTRAST{ "Contrast" };

constexpr auto DRON_TYPE{ "DronType" };

constexpr auto CLUSTER_WIDTH{ "ClusterWidth" };
constexpr auto CLUSTER_HEIGHT{ "ClusterHeight" };
constexpr auto IMAGE_OFFSET{ "ImageOffset" };
constexpr auto DRON_THICKNESS{ "DronThickness" };
constexpr auto GLOBAL_OFFSET{ "GlobalOffset" };

constexpr auto DRON_NOISE_START{ "DronNoiseStart" };
constexpr auto DRON_NOISE_STOP{ "DronNoiseStop" };
constexpr auto DRON_NOISE_DELTA{ "DronNoiseDelta" };
constexpr auto DRON_CONTRAST_START{ "DronContrastStart" };
constexpr auto DRON_CONTRAST_STOP{ "DronContrastStop" };
constexpr auto DRON_CONTRAST_DELTA{ "DronContrastDelta" };


#ifdef DEBUG
std::string type2strDron(int type)
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
#endif

Filters::AddMultipleDron::AddMultipleDron(QJsonObject const &a_config)
	: m_sizeMin{ a_config[DRON_SIZE_MIN].toInt() }
	, m_sizeMax{ a_config[DRON_SIZE_MAX].toInt() }
	, m_singleMarkerType{ a_config[DRON_MARKER_TYPE].toInt() }
	, m_randSeed{ a_config[DRON_RANDSEED].toInt() }
	, m_noiseInt{ a_config[DRON_NOISE].toInt() }
	, m_contrastInt{ a_config[DRON_CONTRAST].toInt() }
	, m_firstTime{ true }
	, m_clusterWidth{ a_config[CLUSTER_WIDTH].toInt() }
	, m_clusterHeight{ a_config[CLUSTER_HEIGHT].toInt() }
	, m_imageOffset{ a_config[IMAGE_OFFSET].toInt() }
	, m_dronThickness{ a_config[DRON_THICKNESS].toInt() }
	, m_dronNoiseStart{ a_config[DRON_NOISE_START].toInt() }
	, m_dronNoiseStop{ a_config[DRON_NOISE_STOP].toInt() }
	, m_dronNoiseDelta{ a_config[DRON_NOISE_DELTA].toInt() }
	, m_dronContrastStart{ a_config[DRON_CONTRAST_START].toInt() }
	, m_dronContrastStop{ a_config[DRON_CONTRAST_STOP].toInt() }
	, m_dronContrastDelta{ a_config[DRON_CONTRAST_DELTA].toInt() }
	, m_dronWhiteBlack{ a_config[DRON_TYPE].toString() }
	, m_whiteDronActive(false)
	, m_blackDronActive(false)
{
	#ifdef DEBUG
	Logger->debug("Filters::AddMultipleDron::AddMultipleDron()");
	#endif

	Filters::AddMultipleDron::checkDronList(m_dronWhiteBlack, m_whiteDronActive, m_blackDronActive);

	// Init randseed's:
	m_randomGenerator = new QRandomGenerator(m_randSeed);
	cv::theRNG().state = m_randSeed;
	m_noiseDouble = m_noiseInt / 1.0;

	m_addDronImpl1.configure(a_config);
	m_addDronImpl2.configure(a_config);

	if (m_noiseInt >= 0)
	{
		m_dronNoiseStart = m_noiseInt;
		m_dronNoiseStop = m_noiseInt;
		m_dronNoiseDelta = 1;

	}
	if (m_contrastInt >= 0)
	{
		m_dronContrastStart = m_contrastInt;
		m_dronContrastStop = m_contrastInt;
		m_dronContrastDelta = 1;
	}
}

void Filters::AddMultipleDron::checkDronList(const QString & dronWhiteBlack, bool & whiteDronActive, bool & blackDronActive)
{
	QStringList activeDronColorList = dronWhiteBlack.split("_");
	whiteDronActive = false;
	blackDronActive = false;
	for (qint32 i = 0; i < activeDronColorList.size(); i++)
	{
		if (activeDronColorList[i] == QString::fromStdString("WHITE"))
		{
			whiteDronActive = true;
		}
		if (activeDronColorList[i] == QString::fromStdString("BLACK"))
		{
			blackDronActive = true;
		}
	}
}

cv::Mat Filters::AddMultipleDron::prepareDron(cv::Mat & processing, AddDronImpl &addDronImpl)
{
	cv::Mat dron;
	if (m_clusterWidth > m_width || m_clusterHeight > m_height)
	{
		cv::resize(processing, dron, cv::Size(m_width, m_height));
	}
	else
	{
		dron = processing(cv::Rect(0, 0, m_clusterWidth, m_clusterHeight));
	}

	cv::Mat cleanDron = dron.clone();



	cv::Point positionDron = cv::Point(addDronImpl.x, addDronImpl.y);

	#ifdef DEBUG
		Logger->debug("Filters::AddMultipleDron::prepareDron() positionDron:({},{})",positionDron.x,positionDron.y);
	#endif

	cv::drawMarker(cleanDron, positionDron, 255, m_singleMarkerType, m_addDronImpl1.dronSize, m_dronThickness, 8);
	return cleanDron;
}


void Filters::AddMultipleDron::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	Logger->debug("Filters::AddMultipleDron::process()");
	#endif
	/*
	0 - generated dron with noise on image
	1 - generated dron - GT
	2 - generated noise
	*/

	#ifdef DEBUG_OPENCV
	cv::imshow("_data[0].processing:", _data[0].processing);
	cv::imshow("_data[1].processing:", _data[1].processing);
	#endif

	if (m_firstTime) 
	{
		m_firstTime = false;
		m_width = _data[0].processing.cols;
		m_height = _data[0].processing.rows;
		m_addDronImpl1.configure(m_width, m_height, m_clusterWidth, m_clusterHeight, 1);
		m_addDronImpl2.configure(m_width, m_height, m_clusterWidth, m_clusterHeight, 2);
	}

	m_addDronImpl1.process();
	m_addDronImpl2.process();

	cv::Mat clone;
	if (m_clusterWidth > m_width || m_clusterHeight > m_height)
	{
		cv::resize(_data[0].processing, clone, cv::Size(m_width, m_height));
	}
	else
	{
		clone = _data[0].processing(cv::Rect(0, 0, m_clusterWidth, m_clusterHeight));
	}
	cv::Scalar mean = cv::mean(clone);

	cv::Mat cleanDron1 = Filters::AddMultipleDron::prepareDron(_data[1].processing, m_addDronImpl1);
	cv::Mat cleanDron2 = Filters::AddMultipleDron::prepareDron(_data[1].processing, m_addDronImpl2);

	
	

	std::vector<std::vector<cv::Mat>> images;
	std::vector<std::vector<cv::Mat>> drones;

	for (int i = m_dronNoiseStart; i <= m_dronNoiseStop; i += m_dronNoiseDelta)
	{
		std::vector<cv::Mat> image;
		std::vector<cv::Mat> drone;
		for (int j = m_dronContrastStart; j <= m_dronContrastStop; j += m_dronContrastDelta)
		{
			cv::Mat tempClone = clone.clone();
			cv::Mat tempDron1 = cleanDron1.clone();
			cv::Mat tempDron2 = cleanDron2.clone();
			int contrast = int(255.0 * (j / 100.0));
			cv::Mat tempDronContrast1;
			cv::Mat tempDronContrast2;
			cv::threshold(tempDron1, tempDronContrast1, 1, contrast, 0);
			cv::threshold(tempDron2, tempDronContrast2, 1, contrast, 0);
			int contrastImage;
			int delta;
			cv::Mat tempDronContrastImage1;
			cv::Mat tempDronContrastImage2;
			if(m_whiteDronActive)
			{
				// dron white:
				delta = 255 - mean[0];
				contrastImage = delta * (j / 100.0);
				cv::threshold(tempDron1, tempDronContrastImage1, 1, contrastImage, 0);
				cv::add(tempClone, tempDronContrastImage1, tempClone);
				#ifdef DEBUG
				if(m_dronContrastStart == m_dronContrastStop)
				{
					Logger->debug("Dron1 delta:{} = {} - {} ", delta, 255, mean[0]);
					Logger->debug("Dron1 contrastImage:{} = {} * ({} / 100)", contrastImage, delta, j, 100.0);
					Logger->debug("Dron1 image value:(mean+contrastImage) {}+{}={} \n", mean[0], contrastImage, contrastImage+mean[0]);
				}
				#endif

			}
			if(m_blackDronActive)
			{
				// dron black:
				delta = mean[0];
				contrastImage = delta * (j / 100.0);
				cv::threshold(tempDron2, tempDronContrastImage2, 1, contrastImage, 0);
				cv::subtract(tempClone, tempDronContrastImage2, tempClone);
				#ifdef DEBUG
				if(m_dronContrastStart == m_dronContrastStop)
				{
					Logger->debug("Dron2 delta:{} = {} - {} ", delta, 255, mean[0]);
					Logger->debug("Dron2 contrastImage:{} = {} * ({} / 100)", contrastImage, delta, j, 100.0);
					Logger->debug("Dron1 image value:(mean-contrastImage) {}+{}={}\n", mean[0], contrastImage, mean[0] - contrastImage);
				}
				#endif
			}
			
			cv::Mat noise_image(tempClone.size(), CV_16SC1);
			double m_noiseDouble = i / 1.0;
			cv::randn(noise_image, cv::Scalar::all(0.0), cv::Scalar::all(m_noiseDouble));
			cv::Mat temp_image;
			cv::Mat mark2;
			tempClone.convertTo(temp_image, CV_16SC1);
			addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
			temp_image.convertTo(tempClone, tempClone.type());

			
			cv::Mat tempDronContrast;

			if(m_blackDronActive && m_whiteDronActive)
			{
				cv::add(tempDronContrast1, tempDronContrast2, tempDronContrast);
				cv::threshold(tempDronContrast, tempDronContrast, 1, 255, 0);
			}
			else if(m_blackDronActive)
			{
				cv::threshold(tempDronContrast2, tempDronContrast, 1, 255, 0);
			}
			else if(m_whiteDronActive)
			{
				cv::threshold(tempDronContrast1, tempDronContrast, 1, 255, 0);
			}

			image.push_back(tempClone);
			drone.push_back(tempDronContrast);
		}
		images.push_back(image);
		drones.push_back(drone);
	}

	cv::Mat all;
	cv::Mat allDron;
	std::vector<cv::Mat> part;
	std::vector<cv::Mat> partDron;

	for (int i = 0; i < images.size(); i++)
	{
		cv::Mat partSingle;
		cv::Mat partSingleDron;
		for (int j = 1; j < images[i].size(); j++)
		{
			#ifdef DEBUG
			std::string type = type2strDron(images[i][j].type());
			Logger->trace("images[{}][{}].type():{}", i, j, type);
			#endif
			if (j == 1)
			{
				cv::hconcat(images[i][0], images[i][1], partSingle);
				cv::hconcat(drones[i][0], drones[i][1], partSingleDron);
			}
			else
			{
				cv::hconcat(partSingle, images[i][j], partSingle);
				cv::hconcat(partSingleDron, drones[i][j], partSingleDron);
			}
		}
		part.push_back(partSingle);
		partDron.push_back(partSingleDron);

		if (part.size() == 2)
		{
			cv::vconcat(part[0], part[1], all);
		}
		else if (part.size() > 2)
		{
			cv::vconcat(all, partSingle, all);
		}

		if (partDron.size() == 2)
		{
			cv::vconcat(partDron[0], partDron[1], allDron);
		}
		else if (partDron.size() > 2)
		{
			cv::vconcat(allDron, partSingleDron, allDron);
		}
	}

	#ifdef DEBUG
		Logger->error("images.size:{}", images.size());
		Logger->error("images[0].size:{}", images[0].size());
	#endif

	if(images.size() == 1)
	{
		all = images[0][0];
		allDron = drones[0][0];
	}

	#ifdef DEBUG
	cv::imshow("all:", all);
	#endif
	// 0:
	_data[0].processing = all.clone();
	// 1:
	_data[1].processing = allDron.clone();
	// 2:
	struct _data data2;
	data2.processing = _data[0].processing.clone();
	_data.push_back(data2);

	#ifdef DEBUG_OPENCV
		cv::imshow("all:", all);
		cv::imshow("allDron:", allDron);
		//cv::imshow("_data[1].processing:", _data[1].processing);
		cv::waitKey(0);
	#endif

	m_addDronImpl1.endProcess();
	m_addDronImpl2.endProcess();
}
