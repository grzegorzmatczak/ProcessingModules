#include <QTest>
#include "../../src/structures.h"
#include "../ProcessingModules/include/processing.h"

class TestModules : public QObject {
  Q_OBJECT

 private:
  void testModule(QJsonObject config, cv::Mat input, cv::Mat ref, Processing *dataProcess);

 private slots:
  void t1() { QVERIFY(true); }
  void t2();
};

void TestModules::t2()
{
  cv::Mat testImage = cv::imread("test.png", cv::IMREAD_GRAYSCALE);
  cv::Mat testImageClear = testImage.clone();
  if (testImage.empty()) {
    QFAIL("test.png is empty");
  }

  Processing *_filter = Processing::make("Filter");
  Processing *_estimator = Processing::make("Estimator");
  Processing *_adder = Processing::make("Adder");

  std::vector<QJsonObject> testCases;
  testCases.push_back(QJsonObject({ { "Name", "MedianBlur" }, { "KernelSize", 5 } }));
  testCases.push_back(
      QJsonObject({ { "Name", "Blur" }, { "KernelSizeX", 7 }, { "KernelSizeY", 3 }, { "BorderType", 0 } }));
  testCases.push_back(QJsonObject({ { "Name", "GaussianBlur" },
                                    { "KernelSizeX", 7 },
                                    { "KernelSizeY", 3 },
                                    { "SigmaX", 7 },
                                    { "SigmaY", 3 },
                                    { "BorderType", 3 } }));
  testCases.push_back(QJsonObject(
      { { "Name", "Threshold" }, { "ThresholdDecision", 124 }, { "ThresholdTypes", 0 }, { "ThresholdValue", 123 } }));
  testCases.push_back(QJsonObject({ { "Name", "Sobel" },
                                    { "OrderDerivativeX", 3 },
                                    { "OrderDerivativeY", 3 },
                                    { "KernelSize", 5 },
                                    { "BorderType", 0 } }));
  testCases.push_back(QJsonObject({ { "Name", "BilateralFilter" },
                                    { "Diameter", 3 },
                                    { "SigmaColor", 0.3 },
                                    { "SigmaSpace", 0.5 },
                                    { "BorderType", 0 } }));
  testCases.push_back(QJsonObject(
      { { "Name", "MorphologyOperation" }, { "MorphOperator", 1 }, { "MorphSize", 3 }, { "MorphElement", 1 } }));
  for (int i = 0; i < testCases.size(); i++) {
    testModule(testCases[i], testImage, testImageClear, _filter);
  }
}

void TestModules::testModule(QJsonObject config, cv::Mat input, cv::Mat ref, Processing *dataProcess)
{
  cv::Mat tempImage = ref.clone();
  _data data{ input.clone() };
  std::vector<_data> dataVec;
  dataVec.push_back(data);

  if (config["Name"].toString() == "MedianBlur") {
    cv::medianBlur(tempImage, tempImage, config["KernelSize"].toInt());
  }
  if (config["Name"].toString() == "Blur") {
    cv::blur(tempImage, tempImage, cv::Size(config["KernelSizeX"].toInt(), config["KernelSizeY"].toInt()),
             cv::Point(-1, -1), config["BorderType"].toInt());
  }
  if (config["Name"].toString() == "GaussianBlur") {
    cv::GaussianBlur(tempImage, tempImage, cv::Size(config["KernelSizeX"].toInt(), config["KernelSizeY"].toInt()),
                     config["SigmaX"].toDouble(), config["SigmaY"].toDouble(), config["BorderType"].toInt());
  }
  if (config["Name"].toString() == "Threshold") {
    cv::threshold(tempImage, tempImage, config["ThresholdDecision"].toInt(), config["ThresholdValue"].toInt(),
                  config["ThresholdTypes"].toInt());
  }
  if (config["Name"].toString() == "Sobel") {
    cv::Mat dst = tempImage.clone();
    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;
    cv::Sobel(dst, grad_x, -1, config["OrderDerivativeX"].toInt(), 0, config["KernelSize"].toInt(), 1, 0,
              config["BorderType"].toInt());
    cv::Sobel(dst, grad_y, -1, 0, config["OrderDerivativeY"].toInt(), config["KernelSize"].toInt(), 1, 0,
              config["BorderType"].toInt());

    cv::convertScaleAbs(grad_x, abs_grad_x);
    cv::convertScaleAbs(grad_y, abs_grad_y);

    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, tempImage);
  }
  if (config["Name"].toString() == "BilateralFilter") {
    cv::Mat dst = tempImage.clone();
    cv::bilateralFilter(dst, tempImage, config["Diameter"].toInt(), config["SigmaColor"].toDouble(),
                        config["SigmaSpace"].toDouble(), config["BorderType"].toInt());
  }
  if (config["Name"].toString() == "MorphologyOperation") {
    cv::Mat m_Element =
        cv::getStructuringElement(config["MorphElement"].toInt(),
                                  cv::Size(2 * config["MorphSize"].toInt() + 1, 2 * config["MorphSize"].toInt() + 1),
                                  cv::Point(config["MorphSize"].toInt(), config["MorphSize"].toInt()));
    morphologyEx(tempImage, tempImage, config["MorphOperator"].toInt(), m_Element);
  }

  dataProcess->configure(config);

  dataProcess->process(dataVec);
  cv::Mat result;

  cv::compare(dataVec[0].processing, tempImage, result, cv::CMP_EQ);

  int percentage = cv::countNonZero(result);
  QCOMPARE(percentage, 1992640);
}

QTEST_MAIN(TestModules)
#include "test.moc"