#ifndef PROCESSING_H
#define PROCESSING_H

#include <memory>

#include <QObject>

#include "processing_global.h"

struct _data
{
    cv::Mat processing;
    QString testStr;
};

class Filter;
class Subtractor;
class Adder;

class Processing : public QObject
{
    Q_OBJECT

public:

    explicit Processing(QObject* parent = nullptr);
    ~Processing();
    static Processing* make(QString model);
    virtual void configure(QJsonObject const& a_config) = 0;
    virtual void process(std::vector<_data>& _data) = 0;
    virtual double getElapsedTime() = 0;
    std::unique_ptr<logger::Logger> mLogger;
};

#endif // PROCESSING_H
