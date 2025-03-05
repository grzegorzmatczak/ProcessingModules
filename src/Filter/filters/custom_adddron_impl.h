#ifndef FILTER_ADD_DRON_IMPL_H
#define FILTER_ADD_DRON_IMPL_H

#include "basefilter.h"

struct bounds {
  qint32 x1;
  qint32 x2;
  qint32 y1;
  qint32 y2;
};

class QJsonObject;

namespace Filters 
{
class AddDronImpl
{
 public:
	AddDronImpl();
	void configure(int width, int height, int clusterWidth, int clusterHeight);
	void configure(QJsonObject const &a_config, int randSeed);
	void configure( int randSeed);
	void process();
	void checkBoundies(qint32 offset);
	void checkBoundies(const qint32 &offset, qint32 &x, qint32 &y, const struct bounds &b);
	void addGaussianNoise(cv::Mat &image, double average, double standard_deviation, cv::Mat &noise);
	void endProcess();

public:
	int dronSize{};
	int x{};
	int y{};
	int markerType{};

 private:
	int m_sizeMin{};
	int m_sizeMax{};

	int m_color{};
	int m_rotate{};

	int m_velocityMin{};
	int m_velocityMax{};

	double m_probabilityOfChangeSize{};
	double m_probabilityOfChangeVelocity{};
	double m_probabilityOfRotate{};

	int m_oldX{};
	int m_oldY{};
	int m_velocityX{};
	int m_velocityY{};

	int dronVelocity{};
	int velocityX{};
	int velocityY{};

	int m_offset{};
	int m_width{};
	int m_height{};

	bool m_firstTime{};
	int m_iterator{};

private:
	QRandomGenerator *m_randomGenerator;
	int m_singleMarkerType{};
	int m_markerType{};
	int m_clusterWidth;
	int m_clusterHeight;
	bool m_globalOffset{};
	struct bounds m_bounds;
	int m_randSeed{};
	int m_imageOffset;

};

} // namespace Filters

#endif // FILTER_ADD_DRON_IMPL_H
