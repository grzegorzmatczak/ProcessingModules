#include "custom_addmultipledron.h"

#include <QJsonObject>
#include <QRandomGenerator>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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
#define DEBUG
#define DEBUG_OPENCV

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
	
	, m_clusterWidth{ a_config[CLUSTER_WIDTH].toInt() }
	, m_clusterHeight{ a_config[CLUSTER_HEIGHT].toInt() }
	, m_imageOffset{ a_config[IMAGE_OFFSET].toInt() }
	, m_dronThickness{ a_config[DRON_THICKNESS].toInt() }
	, m_dronNoiseStart{ a_config[DRON_NOISE_START].toDouble() }
	, m_dronNoiseStop{ a_config[DRON_NOISE_STOP].toDouble() }
	, m_dronNoiseDelta{ a_config[DRON_NOISE_DELTA].toDouble() }
	, m_dronContrastStart{ a_config[DRON_CONTRAST_START].toDouble() }
	, m_dronContrastStop{ a_config[DRON_CONTRAST_STOP].toDouble() }
	, m_dronContrastDelta{ a_config[DRON_CONTRAST_DELTA].toDouble() }
	, m_dronWhiteBlack{ a_config[DRON_TYPE].toString() }
	, m_firstTime( true )
	, m_whiteDronActive(false)
	, m_blackDronActive(false)
{
	#ifdef DEBUG
	//Logger->debug("Filters::AddMultipleDron::AddMultipleDron() m_noiseInt:{}", m_noiseInt); 
	//Logger->debug("Filters::AddMultipleDron::AddMultipleDron() m_contrastInt:{}", m_contrastInt); 
	//Logger->debug("Filters::AddMultipleDron::AddMultipleDron() randSeed:{}", m_randSeed); 
	//Logger->debug("Filters::AddMultipleDron::AddMultipleDron() m_clusterWidth:{}", m_clusterWidth); 
	//Logger->debug("Filters::AddMultipleDron::AddMultipleDron() m_clusterHeight:{}", m_clusterHeight); 
	//Logger->debug("Filters::AddMultipleDron::AddMultipleDron() m_sizeMin:{}", m_sizeMin); 
	//Logger->debug("Filters::AddMultipleDron::AddMultipleDron() m_sizeMax:{}", m_sizeMax); 
	#endif
	// TODO:
	if (m_dronWhiteBlack == "CONTRAST_95" ||
		m_dronWhiteBlack == "CONTRAST_65" ||
		m_dronWhiteBlack == "CONTRAST_35" ||
		m_dronWhiteBlack == "CONTRAST_5" )
	{
		m_dronWhiteBlack = "BLACK_WHITE";
		//Logger->debug("Filters::AddMultipleDron::AddMultipleDron() m_dronWhiteBlack = BLACK_WHITE"); 
	}
	
	Filters::AddMultipleDron::checkDronList(m_dronWhiteBlack, m_whiteDronActive, m_blackDronActive);

	// Init randseed's:
	m_randomGenerator = new QRandomGenerator(m_randSeed);
	cv::theRNG().state = m_randSeed;

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

	for (double i = m_dronNoiseStart; i <= m_dronNoiseStop; i += m_dronNoiseDelta)
	{
		std::vector<AddDronImplVector> m_dronImplVectorTemp;

		for (double j = m_dronContrastStart; j <= m_dronContrastStop; j += m_dronContrastDelta)
		{	
			AddDronImpl dron1;
			AddDronImpl dron2;
			m_dronImplVectorTemp.push_back({dron1,dron1});
		}

		m_dronImplVector.push_back(m_dronImplVectorTemp);
	}
	//Logger->debug("m_dronImplVector.size():{}", m_dronImplVector.size());
	
	for (int it = 0; it < m_dronImplVector.size(); it++)
	{
		for (int jt = 0; jt < m_dronImplVector[it].size(); jt++)
		{
			m_randSeed++;
			m_dronImplVector[it][jt].dron1.configure(a_config, m_randSeed);
			m_dronImplVector[it][jt].dron2.configure(a_config, m_randSeed+1000);
		}
	}

	#ifdef DEBUG
	//Logger->debug("m_dronNoiseStart:{}", m_dronNoiseStart);
	//Logger->debug("m_dronNoiseStop:{}", m_dronNoiseStop); 
	//Logger->debug("m_dronNoiseDelta:{}", m_dronNoiseDelta); 

	//Logger->debug("m_dronContrastStart:{}", m_dronContrastStart); 
	//Logger->debug("m_dronContrastStop:{}", m_dronContrastStop); 
	//Logger->debug("m_dronContrastDelta:{}", m_dronContrastDelta);
	#endif
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
	#ifdef DEBUG
	//Logger->debug("whiteDronActive:{}", whiteDronActive); 
	//Logger->debug("blackDronActive:{}", blackDronActive); 
	#endif
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

	//if(dron.empty())
		//Logger->error("Filters::AddMultipleDron::prepareDron() dron.empty()");

	//Logger->debug("Filters::AddMultipleDron::prepareDron() dron.size:({}x{})" , dron.cols, dron.rows);	
	cv::Mat cleanDron = dron.clone();
	cv::Point positionDron = cv::Point(addDronImpl.x, addDronImpl.y);	

	#ifdef DEBUG
		//Logger->debug("Filters::AddMultipleDron::prepareDron() positionDron:({},{})",positionDron.x,positionDron.y);
	#endif
	
	try
	{
		cv::drawMarker(cleanDron, positionDron, 255, addDronImpl.markerType, addDronImpl.dronSize, m_dronThickness, 8);
	}
	catch (cv::Exception& e)
	{
		const char* err_msg = e.what();
		qDebug() << "exception caught: " << err_msg;
	}
	#ifdef DEBUG
		//Logger->debug("Filters::AddMultipleDron::prepareDron() return");
	#endif
	return cleanDron;
}


void Filters::AddMultipleDron::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	//Logger->debug("Filters::AddMultipleDron::process()");
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
		#ifdef DEBUG
		//Logger->debug("Filters::AddMultipleDron::prepareDron() if (m_firstTime)");
		#endif
		m_firstTime = false;
		m_width = _data[0].processing.cols;
		m_height = _data[0].processing.rows;
		for (int it = 0; it < m_dronImplVector.size(); it++)
		{
			for (int jt = 0; jt < m_dronImplVector[it].size(); jt++)
			{
				m_dronImplVector[it][jt].dron1.configure(m_width, m_height, m_clusterWidth, m_clusterHeight);
				m_dronImplVector[it][jt].dron2.configure(m_width, m_height, m_clusterWidth, m_clusterHeight);
			}
		}
		//qint32 deltaX = 0;
    	//qint32 deltaY = 0;
		#ifdef DEBUG
		//Logger->debug("m_clusterWidth:{}", m_clusterWidth); 
		//Logger->debug("m_clusterHeight:{}", m_clusterHeight); 
		//Logger->debug("m_width:{}", m_width); 
		//Logger->debug("m_height:{}", m_height); 
		//Logger->debug("while:");
		#endif

		int deltaX = 0;
    	int deltaY = 0;
		int clusterIndex{0};
		int clusterIndex_j{0};
		for (double i = m_dronNoiseStart; i <= m_dronNoiseStop; i += m_dronNoiseDelta)
		{
			clusterIndex_j = 0;
			std::vector<cv::Mat> cluster;
			deltaX = 0;
			for (double j = m_dronContrastStart; j <= m_dronContrastStop; j += m_dronContrastDelta)
			{
				cluster.push_back(_data[0].processing(cv::Rect(deltaX, deltaY, m_clusterWidth, m_clusterHeight)).clone());
				deltaX += m_clusterWidth;
				if((deltaX + m_clusterWidth) > m_width)
				{
					deltaX = 0;
				}
			}
			m_processing_clusters.push_back(cluster);

			if ((deltaY + 2* m_clusterHeight) >= m_height)
			{
				deltaY = 0;
			}
			deltaY += m_clusterHeight;
		}
		for (int it = 0; it < m_dronImplVector.size(); it++)
		{
			std::vector<AddDronImageVector>  cleanDron;
			for (int jt = 0; jt < m_dronImplVector[it].size(); jt++)
			{
				AddDronImageVector dron{Filters::AddMultipleDron::prepareDron(_data[1].processing, m_dronImplVector[it][jt].dron1), 
				Filters::AddMultipleDron::prepareDron(_data[1].processing, m_dronImplVector[it][jt].dron2)};
				cleanDron.push_back(dron);
			}
			m_cleanDron.push_back(cleanDron);
		}
		//Logger->debug("Filters::AddMultipleDron::prepareDron() if (m_firstTime) done");
	}
	
	for (int it = 0; it < m_dronImplVector.size(); it++)
	{
		for (int jt = 0; jt < m_dronImplVector[it].size(); jt++)
		{
			m_cleanDron[it][jt].dron1 = Filters::AddMultipleDron::prepareDron(_data[1].processing, m_dronImplVector[it][jt].dron1).clone();
			m_cleanDron[it][jt].dron2 = Filters::AddMultipleDron::prepareDron(_data[1].processing, m_dronImplVector[it][jt].dron2).clone();
		}
	}

	cv::Mat clone;
	if (m_clusterWidth > m_width || m_clusterHeight > m_height)
	{
		cv::resize(_data[0].processing, clone, cv::Size(m_width, m_height));
	}
	else
	{
		clone = _data[0].processing(cv::Rect(0, 0, m_clusterWidth, m_clusterHeight));
	}
	

	std::vector<std::vector<cv::Mat>> images;
	std::vector<std::vector<cv::Mat>> drones;

	int clusterIndex{0};
	int clusterIndex_j{0};
	for (double i = m_dronNoiseStart; i <= m_dronNoiseStop; i += m_dronNoiseDelta)
	{
		clusterIndex_j = 0;
		std::vector<cv::Mat> image;
		std::vector<cv::Mat> drone;
		for (double j = m_dronContrastStart; j <= m_dronContrastStop; j += m_dronContrastDelta)
		{
			m_dronImplVector[clusterIndex][clusterIndex_j].dron1.process();
			m_dronImplVector[clusterIndex][clusterIndex_j].dron2.process();
			cv::Mat tempClone = m_processing_clusters[clusterIndex][clusterIndex_j].clone();

			cv::Mat tempDron1 = m_cleanDron[clusterIndex][clusterIndex_j].dron1.clone();
			cv::Mat tempDron2 = m_cleanDron[clusterIndex][clusterIndex_j].dron2.clone();
			int contrast = int(255.0 * (j/ 100.0));
			cv::Mat tempDronContrast1;
			cv::Mat tempDronContrast2;
			cv::threshold(tempDron1, tempDronContrast1, 1, contrast, 0);
			cv::threshold(tempDron2, tempDronContrast2, 1, contrast, 0);

			int contrastImage;
			int delta;
			cv::Mat tempDronContrastImage1;
			cv::Mat tempDronContrastImage2;
			cv::Scalar mean = cv::mean(tempClone);
			#ifdef DEBUG
			//Logger->debug("mean:{}", mean[0]); 
			#endif

			if(m_whiteDronActive)
			{
				#ifdef DEBUG
				//Logger->debug("Filters::AddMultipleDron::process() dron white:");
				#endif
				// dron white:
				delta = 255 - mean[0];
				contrastImage = delta * (j / 100.0);
				cv::threshold(tempDron1, tempDronContrastImage1, 1, contrastImage, 0);
				cv::add(tempClone, tempDronContrastImage1, tempClone);
				#ifdef DEBUG
				//Logger->debug("Filters::AddMultipleDron::process() m_dronImplVector.size():{}", m_dronImplVector.size());
				#endif
				#ifdef DEBUG
				if(m_dronContrastStart == m_dronContrastStop)
				{
					//Logger->debug("Dron1 delta:{} = {} - {} ", delta, 255, mean[0]);
					//Logger->debug("Dron1 contrastImage:{} = {} * ({} / 100)", contrastImage, delta, j, 100.0);
					//Logger->debug("Dron1 image value:(mean+contrastImage) {}+{}={} \n", mean[0], contrastImage, contrastImage+mean[0]);
				}
				#endif

			}
			if(m_blackDronActive)
			{
				#ifdef DEBUG
				//Logger->debug("Filters::AddMultipleDron::process() dron black:");
				#endif
				// dron black:
				delta = mean[0];
				contrastImage = delta * (j / 100.0);
				cv::threshold(tempDron2, tempDronContrastImage2, 1, contrastImage, 0);
				cv::subtract(tempClone, tempDronContrastImage2, tempClone);

				#ifdef DEBUG
				if(m_dronContrastStart == m_dronContrastStop)
				{
					//Logger->debug("Dron2 delta:{} = {} - {} ", delta, 255, mean[0]);
					//Logger->debug("Dron2 contrastImage:{} = {} * ({} / 100)", contrastImage, delta, j, 100.0);
					//Logger->debug("Dron1 image value:(mean-contrastImage) {}+{}={}\n", mean[0], contrastImage, mean[0] - contrastImage);
				}
				#endif
			}
			
			#ifdef DEBUG
			//Logger->debug("Filters::AddMultipleDron::process() noise:");
			#endif
			cv::Mat noise_image(tempClone.size(), CV_16SC1);
			double noiseDouble = i / 1.0;
			cv::randn(noise_image, cv::Scalar::all(0.0), cv::Scalar::all(noiseDouble));
			cv::Mat temp_image;
			cv::Mat mark2;
			tempClone.convertTo(temp_image, CV_16SC1);
			addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
			temp_image.convertTo(tempClone, tempClone.type());
			
			#ifdef DEBUG
			//Logger->debug("Filters::AddMultipleDron::process() tempDronContrast:");
			#endif
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

			#ifdef DEBUG
			//Logger->debug("Filters::AddMultipleDron::process() endProcess:");
			#endif
			m_dronImplVector[clusterIndex][clusterIndex_j].dron1.endProcess();
			m_dronImplVector[clusterIndex][clusterIndex_j].dron2.endProcess();
			clusterIndex_j++;
		}
		images.push_back(image);
		drones.push_back(drone);
		clusterIndex++;
	}

	cv::Mat all;
	cv::Mat allDron;
	std::vector<cv::Mat> part;
	std::vector<cv::Mat> partDron;
	#ifdef DEBUG
	//Logger->debug("images.size():{}", images.size());
	#endif

	for (int i = 0; i < images.size(); i++)
	{
		#ifdef DEBUG
		//Logger->debug("images[i].size():{}", images[i].size());
		#endif
		cv::Mat partSingle;
		cv::Mat partSingleDron;
		for (int j = 1; j < images[i].size(); j++)
		{
			#ifdef DEBUG
			std::string type = type2strDron(images[i][j].type());
			//Logger->trace("images[{}][{}].type():{}", i, j, type);
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
		//Logger->debug("images.size:{}", images.size());
		//Logger->debug("images[0].size:{}", images[0].size());
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
	for (int it = 0; it < m_dronImplVector.size(); it++)
	{
		
	}

}
