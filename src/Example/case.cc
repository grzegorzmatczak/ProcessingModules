#include "case.h"

Case::Case(QJsonObject const &a_config, QObject *parent) : QObject(parent) {
  cv::Mat input1 = cv::imread("input.jpg");
  cv::Mat input2 = cv::imread("input.jpg");

  // COLOR_BGR2HSV      = 40

  std::vector<cv::Mat> inputs;
  inputs.push_back(input1);
  inputs.push_back(input2);

  cv::Mat output(input1.rows, input1.cols, CV_8UC1, cv::Scalar(255));
  m_timer.start();
  std::vector<Block *> m_block;
  std::vector<std::vector<_data>> m_data;
  H_Logger->info("Case::Case() m_data:");
  auto m_graph = a_config[GRAPH].toArray();
  // H_Logger->info("Case::Case() block.size:{}", m_graph.size());

  H_Logger->info("Case::Case() m_graph:");
  for (int i = 0; i < m_graph.size(); i++) {
    QJsonObject _obj = m_graph[i].toObject();
    // qDebug() << "m_graph[" << i << "]:" << _obj;
    QString _type = _obj[TYPE].toString();
    QJsonArray _prevActive = _obj[PREV].toArray();
    // qDebug() << "prevActive:" << _prevActive;
    // qDebug() << "type:" << _type;
    Block *_block = Block::make(_type);
    _block->configure(_obj[CONFIG].toObject());
    // qDebug() << "Config:" << _obj[CONFIG].toObject();
    m_block.push_back(_block);
    for (int j = 0; j < _prevActive.size(); j++) {
      if (_prevActive[j].toObject()[ACTIVE].toInt() == -1) {
        qDebug() << "inputs[" << i << "]";
        _data data{inputs[i].clone(), inputs[i].clone(), i};
        std::vector<_data> dataVec;
        dataVec.push_back(data);
        m_data.push_back(dataVec);
        // qDebug() << "add dataVec to m_data:" << m_data;
      }
    }
  }
  m_timer.stop();
  qDebug() << "Time config:" << m_timer.getTimeMilli();
  m_timer.reset();
  // qDebug() << "All m_block:" << m_block;
  // qDebug() << "All m_data:" << m_data;

  H_Logger->info("Case::Case() m_data:");
  for (int i = 0; i < m_data.size(); i++) {
    for (int j = 0; j < m_data[i].size(); j++) {
      qDebug() << "All m_data[" << i << "][" << j
               << "].test:" << m_data[i][j].test;
    }
    if (i == 0) {
      m_data[i][0].test = 5555;
    }
  }

  for (int i = 0; i < m_data.size(); i++) {
    for (int j = 0; j < m_data[i].size(); j++) {
      qDebug() << "All m_data[" << i << "][" << j
               << "].test:" << m_data[i][j].test;
    }
    if (i == 0) {
      m_data[i][0].test = 5555;
    }
  }
  H_Logger->info("Case::Case() m_graph:");
  qDebug() << "Main loop:";
  double time = 0;
  m_timer.start();
  for (int i = 0; i < m_graph.size(); i++) {
    const QJsonObject _obj = m_graph[i].toObject();
    const QJsonArray _prevActive = _obj[PREV].toArray();
    // qDebug() << "m_graph[" << i << "]:" << _obj;
    // qDebug() << "m_block[" << i << "]:" << m_block[i];

    bool _flagNotStart = true;
    for (int j = 0; j < _prevActive.size(); j++) {
      if (_prevActive[j].toObject()[ACTIVE].toInt() == -1) {
        _flagNotStart = false;
      }
    }

    if (_flagNotStart) {
      // qDebug() << "Copy data:";

      if (_prevActive.size() == 1) {
        qDebug() << "_prevActive.size() == 1";
        qint32 _prevIterator = _prevActive[0].toObject()[ACTIVE].toInt();
        if (m_graph[_prevIterator].toObject()[TYPE] == "Adder") {
          // std::vector<_data> dataVec;
          // dataVec.push_back(m_data[_prevIterator][0]);
          // m_data.push_back(dataVec);

          std::vector<_data> dataVec;
          dataVec.push_back(std::move(m_data[_prevIterator][0]));
          m_data.push_back(std::move(dataVec));

        } else {
          // m_data.push_back(m_data[_prevIterator]);
          m_data.push_back(std::move(m_data[_prevIterator]));
        }
      }
      if (_prevActive.size() == 2) {
        qDebug() << "_prevActive.size() == 2";
        std::vector<_data> _data;
        for (int j = 0; j < _prevActive.size(); j++) {
          //_data.push_back(m_data[_prevActive[j].toObject()[ACTIVE].toInt()][0]);
          _data.push_back(
              std::move(m_data[_prevActive[j].toObject()[ACTIVE].toInt()][0]));
        }

        // m_data.push_back(_data);
        m_data.push_back(std::move(_data));
      }
    }

    qDebug() << "Processing";
    for (int i = 0; i < m_data.size(); i++) {
      for (int j = 0; j < m_data[i].size(); j++) {
        qDebug() << "All m_data[" << i << "][" << j
                 << "].test:" << m_data[i][j].test;
      }
    }
    m_block[i]->process((m_data[i]));
    time += m_block[i]->getElapsedTime();
    // qDebug() << "Time;" << time;
    cv::imwrite((QString::number(i) + ".png").toStdString(),
                m_data[i][0].processing);
    // cv::imshow(QString::number(i).toStdString(), m_data[i][0].processing);
  }
  // cv::waitKey(0);
  m_timer.stop();
  qDebug() << "Time:" << time;
  qDebug() << "Time all" << m_timer.getTimeMilli();

  emit(quit());
}
