#include "mainloop.h"

MainLoop::MainLoop()
{
  H_Logger->trace("MainLoop::MainLoop()");
  /*
  QJsonObject filterConfig2{
    { "Name", "GaussianBlur" }, { "KernelSizeX", 3 }, { "KernelSizeY", 3 }, { "SigmaX", 3 }, { "SigmaY", 3 },
    { "BorderType", 4 }
  };
  QJsonObject filterConfig3{ { "Name", "Color" }, { "ColorCode", 6 } };
  QJsonObject filterConfig{ { "Name", "Resize" }, { "Width", 200 }, { "Height", 200 } };
  QJsonObject estimatorConfig{
    { "Name", "None" }, { "LearningRate", -1 }, { "History", 10 }, { "VarThreshold", 100 }, { "DetectShadow", false }
  };

  QJsonObject estimatorConfig6{
    { "Name", "Median" }, { "History", 1 }, { "HistoryDelta", 1 }, { "DecisionThreshold", 110 }
  };

  Block *a = Block::make("Filter");
  a->configure(filterConfig);
  Block *b = Block::make("Estimator");
  b->configure(estimatorConfig);
  Block *c = Block::make("Filter");
  c->configure(filterConfig2);

  // cv::Mat input(100, 100, CV_8UC1, cv::Scalar(255));

  cv::Mat input = cv::imread("1.png", cv::IMREAD_GRAYSCALE);
  // namedWindow("input", cv::WINDOW_AUTOSIZE);
  // cv::imshow("input", input);
  // cv::Mat output(100, 100, CV_8UC1, cv::Scalar(255));
  cv::Mat output(input.rows, input.cols, CV_8UC1, cv::Scalar(255));

  _data data{ input, input, 1 };
  std::vector<_data *> m_data0;
  std::vector<_data *> m_data1;
  std::vector<_data *> m_data2;
  m_data0.push_back(&data);

  std::vector<Block *> m_block;
  m_block.push_back(a);
  m_block.push_back(b);
  m_block.push_back(c);

  std::vector<qint32> prev;
  std::vector<qint32> active;
  std::vector<qint32> next;

  std::vector<qint32> nodeBreak{ -1 };
  std::vector<qint32> node0{ 0 };
  std::vector<qint32> node1{ 1 };
  std::vector<qint32> node2{ 2 };

  // cv::imshow("m_data[0]", m_data0[0]->processing);
  m_block[0]->process(*m_data0[0]);
  m_data1.push_back(m_data0[0]);

  // cv::imshow("m_data[1]", m_data1[0]->processing);
  m_block[1]->process(*m_data1[0]);
  m_data2.push_back(m_data1[0]);

  // cv::imshow("m_data[2]", m_data2[0]->processing);
  m_block[2]->process(*m_data2[0]);
  // cv::imshow("m_data[2]a", m_data2[0]->processing);

  _list list1{ nodeBreak, m_data0, node1 };
  _list list2{ node0, m_data1, node2 };
  _list list3{ node1, m_data2, nodeBreak };
  std::vector<_list *> m_list;
  m_list.push_back(&list1);
  m_list.push_back(&list2);
  m_list.push_back(&list3);

  qDebug() << "m_list:" << m_list;
  for (int i = 0; i < m_list.size(); i++)
  {
    qDebug() << "m_list:" << m_list;
  }
*/
  QString configPathWithName = "graph.json";

  H_Logger->set_level(static_cast<spdlog::level::level_enum>(1));

  ConfigReader *configReader = new ConfigReader();
  QString possibleConfigReaderError;
  QJsonObject jObject;
  if (!configReader->readConfig(configPathWithName, jObject, possibleConfigReaderError))
  {
    H_Logger->error("{}", possibleConfigReaderError.toStdString());
  }
  delete configReader;

  Case *m_case = new Case(jObject, nullptr);
  QObject::connect(m_case, &Case::quit, this, &MainLoop::onQuit);
  H_Logger->error(" emit(quit())");
  emit(quit());
}

void MainLoop::onUpdate() {}

void MainLoop::onQuit()
{
  qDebug() << "onQuit:";
  H_Logger->error("MainLoop::onQuit()");
  emit(quit());
}
