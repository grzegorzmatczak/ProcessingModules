#pragma once

namespace cv
{
    class Mat;
}

struct _data;

class QString;
class QJsonObject;
class QRandomGenerator;
class QJsonArray;

namespace logger
{
    class Logger;
}

constexpr auto GRAPH{"Graph"};
constexpr auto NAME{"Name"};
constexpr auto ID{"Id"};
constexpr auto COPY_SIGNAL{"Signal"};
constexpr auto TYPE{"Type"};
constexpr auto NEXT{"Next"};
constexpr auto PREV{"Prev"};
constexpr auto CONFIG{"Config"};
constexpr auto WIDTH{"Width"};
constexpr auto HEIGHT{"Height"};
constexpr auto X{"X"};
constexpr auto Y{"Y"};