#ifndef PROCESSING_H
#define PROCESSING_H

#include <memory>

#include <QObject>

#include "processing_global.h"

#include <opencv2/core/core.hpp>
#include "logger.hpp"
struct _data
{
    _data()
    {
        mLogger =
            std::make_unique<logger::Logger>(logger::LogType::CONFIG, logger::LogLevel::MEDIUM, logger::LogFunction::YES);
        mLogger->print(QString("_data()"), logger::LogLevel::MEDIUM, __FUNCTION__);
    }
    _data(cv::Mat& frame, const QString& str)
        : processing(frame)
        , testStr(str)

    {
        mLogger =
            std::make_unique<logger::Logger>(logger::LogType::CONFIG, logger::LogLevel::MEDIUM, logger::LogFunction::YES);
        mLogger->print(QString("_data(cv::Mat& frame, const QString& str)"), logger::LogLevel::MEDIUM, __FUNCTION__);
    }
    ~_data()
    {}
    _data(const _data& other)
        : processing(other.processing)
        , testStr(other.testStr)
    {
        mLogger =
            std::make_unique<logger::Logger>(logger::LogType::CONFIG, logger::LogLevel::MEDIUM, logger::LogFunction::YES);
        mLogger->print(QString("_data(const _data& other)"), logger::LogLevel::MEDIUM, __FUNCTION__);
    }
    _data& operator=(const _data& other)
    {
        mLogger =
            std::make_unique<logger::Logger>(logger::LogType::CONFIG, logger::LogLevel::MEDIUM, logger::LogFunction::YES);
        mLogger->print(QString("_data& operator=(const _data& other)"), logger::LogLevel::MEDIUM, __FUNCTION__);
        if(this==&other)
            return *this;
        return *this;
    }
    _data(_data&& other) noexcept
        : mLogger(std::move(other.mLogger))
        , processing(std::move(other.processing))
    {
        mLogger->print(QString("_data(_data&& other) noexcept"), logger::LogLevel::MEDIUM, __FUNCTION__);
    }
    _data& operator=(_data&& other) noexcept
    {
        mLogger = std::move(other.mLogger);
        
        mLogger->print(QString("_data& operator=(_data&& other) noexcept"), logger::LogLevel::MEDIUM, __FUNCTION__);
        return *this;
    }
    cv::Mat processing;
    QString testStr;
    std::unique_ptr<logger::Logger> mLogger;
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
    Processing(const Processing& other); // II. copy constructor
    Processing& operator=(const Processing& other); // III. copy assignment
    Processing(Processing&& other) noexcept; // IV. move constructor
    Processing& operator=(Processing&& other) noexcept; // V. move assignment

    static Processing* make(QString model);
    virtual void configure(QJsonObject const& a_config) = 0;
    virtual void process(std::vector<_data>& _data) = 0;
    virtual double getElapsedTime() = 0;

protected:
    std::unique_ptr<logger::Logger> mLogger;
};

#endif // PROCESSING_H
