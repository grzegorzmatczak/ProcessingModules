#include "custom_adddron_impl.h"

#include <QJsonObject>
#include <QRandomGenerator>

//#define DEBUG

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
constexpr auto CLUSTER_WIDTH{ "ClusterWidth" };
constexpr auto CLUSTER_HEIGHT{ "ClusterHeight" };
constexpr auto IMAGE_OFFSET{ "ImageOffset" };
constexpr auto DRON_THICKNESS{ "DronThickness" };
constexpr auto GLOBAL_OFFSET{ "GlobalOffset" };

Filters::AddDronImpl::AddDronImpl()
{
	#ifdef DEBUG
	Logger->debug("Filters::AddDronImpl::AddDronImpl()");
	#endif
	m_offset = 5;
	m_randomGenerator = new QRandomGenerator(1);
}

void Filters::AddDronImpl::configure(int width, int height, int clusterWidth, int clusterHeight)
{
	#ifdef DEBUG
	Logger->debug("Filters::AddDronImpl::configure(width, height, clusterWidth, clusterHeight)");
	#endif
	m_width = width;
	m_height = height;
	m_clusterWidth = clusterWidth;
	m_clusterHeight = clusterHeight;

	int deltaX{0};
	int deltaY{0};

	x = (deltaX + m_clusterWidth / 2);
	y = (deltaY + m_clusterHeight / 2);
	m_oldX = (deltaX + m_clusterWidth / 2);
	m_oldY = (deltaY + m_clusterHeight / 2);

	struct bounds tmp
	{
		deltaX, deltaX + m_clusterWidth, deltaY, deltaY + m_clusterHeight
	};
	m_bounds = (tmp);
	//m_markerType = (m_singleMarkerType);
}

void Filters::AddDronImpl::configure(QJsonObject const &a_config, int randSeed)
{
	#ifdef DEBUG
		Logger->debug("Filters::AddDronImpl::configure(config)");
	#endif
	m_randomGenerator->seed(randSeed);
	#ifdef DEBUG
	Logger->debug("Filters::AddDronImpl::configure() randSeed:{}", randSeed);
	#endif
	m_sizeMin = (a_config[DRON_SIZE_MIN].toInt());
	m_sizeMax = (a_config[DRON_SIZE_MAX].toInt());
	m_velocityMin = (a_config[DRON_VELOCITY_MIN].toInt());
	m_velocityMax = (a_config[DRON_VELOCITY_MAX].toInt());
	m_probabilityOfChangeSize =  (a_config[DRON_PROB_OF_CHANGE_SIZE].toDouble());
	m_probabilityOfChangeVelocity = (a_config[DRON_PROB_OF_CHANGE_Velocity].toDouble());
	m_probabilityOfRotate = (a_config[DRON_PROB_OF_ROTATE].toDouble());
	m_markerType = (a_config[DRON_MARKER_TYPE].toInt());
	m_singleMarkerType = (a_config[DRON_MARKER_TYPE].toInt());
	m_globalOffset = (a_config[GLOBAL_OFFSET].toBool());
	m_imageOffset = (a_config[IMAGE_OFFSET].toInt());
	m_randSeed = (a_config[DRON_RANDSEED].toInt());
}

void Filters::AddDronImpl::configure( int randSeed)
{
	m_randomGenerator->seed(randSeed);
}

void Filters::AddDronImpl::process()
{
	#ifdef DEBUG
	Logger->debug("Filters::AddDronImpl::process()");
	#endif

	double _chanceOfChangeVelocity = m_randomGenerator->bounded(0, 100) / 100.0;
	if (_chanceOfChangeVelocity < m_probabilityOfChangeVelocity)
	{
		m_velocityX = m_randomGenerator->bounded(m_velocityMin, m_velocityMax + 1);
		m_velocityY = m_randomGenerator->bounded(m_velocityMin, m_velocityMax + 1);
		double tempProb = m_randomGenerator->bounded(0, 10) / 10.0;
		if (tempProb < 0.5)
		{
			m_velocityX = -m_velocityX;
		}
		tempProb = m_randomGenerator->bounded(0, 10) / 10.0;
		if (tempProb < 0.5)
		{
			m_velocityY = -m_velocityY;
		}
	}

	// lottery change size:
	double _chanceOfChangeSize = m_randomGenerator->bounded(0, 100) / 100.0;
	if (_chanceOfChangeSize < m_probabilityOfChangeSize)
	{
		dronSize = m_randomGenerator->bounded(m_sizeMin, m_sizeMax + 1);
	}
	// lottery rotate:
 	double _chanceOfRotate = m_randomGenerator->bounded(0, 100) / 100.0;
	if (_chanceOfRotate < m_probabilityOfRotate)
	{
	 markerType = m_randomGenerator->bounded(0, m_markerType);
	}

	// update x,y
	x = m_oldX + m_velocityX;
	y = m_oldY + m_velocityY;
	
  	checkBoundies(m_imageOffset, x, y, m_bounds);

	Filters::AddDronImpl::endProcess();
}

void Filters::AddDronImpl::endProcess()
{
	m_oldX = x;
	m_oldY = y;
}

void Filters::AddDronImpl::checkBoundies(qint32 offset)
{
	#ifdef DEBUG
	Logger->debug("Filters::AddDronImpl::checkBoundies()");
	#endif
	if (x < offset) {
		x = offset;
	}
	if (x > (m_width - offset)) {
		x = (m_width - offset);
	}

	if (y < offset) {
		y = offset;
	}
	if (y > (m_height - offset)) {
		y = (m_height - offset);
	}
}

void Filters::AddDronImpl::checkBoundies(const qint32 &offset, qint32 &x, qint32 &y, const struct bounds &b)
{
	#ifdef DEBUG
	Logger->debug("Filters::AddDronImpl::checkBoundies()");
	#endif
	if (x < b.x1 + offset)
	{
		x = b.x1 + offset;
	}
	if (y < b.y1 + offset)
	{
		y = b.y1 + offset;
	}

	if (y > (b.y2 - offset))
	{
		y = (b.y2 - offset);
	}
	if (x > (b.x2 - offset))
	{
		x = (b.x2 - offset);
	}
}

void Filters::AddDronImpl::addGaussianNoise(cv::Mat &image, double average, double standard_deviation, cv::Mat &noise)
{
	#ifdef DEBUG
	Logger->debug("Filters::AddDronImpl::addGaussianNoise()");
	#endif
	cv::Mat noise_image(image.size(), CV_16SC1);
	randn(noise_image, cv::Scalar::all(average), cv::Scalar::all(standard_deviation));
	cv::Mat temp_image;
	image.convertTo(temp_image, CV_16SC1);
	addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
	temp_image.convertTo(image, image.type());
	noise_image.convertTo(noise, noise.type());
}
