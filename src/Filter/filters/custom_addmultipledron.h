#ifndef FILTER_ADD_MULTIPLE_DRON_H
#define FILTER_ADD_MULTIPLE_DRON_H

#include "basefilter.h"
#include "utils/includespdlog.h"
#include "custom_adddron_impl.h"


/* Paste in configuration:
[{
	"name": "AddDron_Average",
	"min": 1,
	"max": 23,
	"isOdd": true
},{
	"name": "AddDron_StandardDeviation",
	"min": 1,
	"max": 23,
	"isOdd": true
}],
*/

class QJsonObject;

namespace Filters {
class AddMultipleDron : public BaseFilter {
 public:
	AddMultipleDron(QJsonObject const &a_config);
	void process(std::vector<_data> &_data);
	void checkDronList(const QString & dronWhiteBlack, bool & whiteDronActive, bool & blackDronActive);
	void checkBoundies(const qint32 &offset, qint32 &x, qint32 &y, const struct bounds &b);
	void addGaussianNoise(cv::Mat &image, double average, double standard_deviation, cv::Mat &noise);
	cv::Mat prepareDron(cv::Mat & processing, AddDronImpl &addDronImpl);
private:
	

 private:
	int m_sizeMin{};
	int m_sizeMax{};
	int m_singleMarkerType{};
	int m_randSeed{};
	int m_noiseInt{};
	int m_contrastInt{};

	int m_iterator{};
	
	int m_imageOffset;
	int m_dronThickness;
	int m_clusterWidth;
	int m_clusterHeight;

	int m_dronNoiseStart;
	int m_dronNoiseStop;
	int m_dronNoiseDelta;
	int m_dronContrastStart;
	int m_dronContrastStop;
	int m_dronContrastDelta;

	double m_noiseDouble{};
	
	bool m_firstTime{};
	bool m_whiteDronActive{};
	bool m_blackDronActive{};

	QString m_dronWhiteBlack;


	int m_width{};
	int m_height{};

 private:
	QRandomGenerator *m_randomGenerator;
	AddDronImpl m_addDronImpl1;
	AddDronImpl m_addDronImpl2;
	struct bounds m_bounds;

 private:
	

};
} // namespace Filters

#endif // FILTER_ADD_MULTIPLE_DRON_H
