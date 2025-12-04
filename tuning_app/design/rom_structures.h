#ifndef ROM_STRUCTURES_H
#define ROM_STRUCTURES_H

#pragma once

#include <QObject>
#include <QtWidgets/QApplication>
#include <QVector3D>
#include <QQuaternion>
#include <string>
#include <stdexcept>

#pragma once

#include "../sdk/rom_time.hpp"

namespace rom_dynamics::data_types 
{
// ------------------------------------------------- STRUCT

struct HackTime {
    int sec;
    unsigned int nanosec;
};

struct RomLaser {
    QString m_frameId;
    float m_angleMin;
    float m_angleMax;
    float m_angleIncrement;
    float m_rangeMin;
    float m_rangeMax;
    QVector<float> m_ranges;
    QVector<float> m_intensities;
    HackTime rom_timestamp;
};

struct RomPose2D {
    double x;
    double y;
    double theta_deg; // yaw in degrees
};

struct TransformStamped {
    QVector3D translation;
    QQuaternion rotation;
    HackTime rom_timestamp;
};

// ------------------------------------------------- ROM MAP CLASS
class RomMap 
{
    public:
        RomMap();
        RomMap(int width, 
            int height, 
            float resolution, 
            float origin_x, 
            float origin_y, 
            const std::vector<int8_t>& data, 
            const RomTime& timestamp = RomTime()
        );

        void setMap(int width, 
            int height, 
            float resolution, 
            float origin_x, 
            float origin_y, 
            const std::vector<int8_t>& data, 
            const RomTime& timestamp = RomTime()
        );
            
        int width() const;
        int height() const;
        float resolution() const;
        float originX() const;
        float originY() const;
        const std::vector<int8_t>& data() const;

        // Time getter/setter
        void setTimestamp(const RomTime& timestamp);
        const RomTime& timestamp() const;

    private:
        int width_;
        int height_;
        float resolution_;
        float origin_x_;
        float origin_y_;
        std::vector<int8_t> data_;
        RomTime timestamp_;
};

inline RomMap::RomMap()
    : width_(0), height_(0), resolution_(0.0f), origin_x_(0.0f), origin_y_(0.0f), data_(), timestamp_() {}

inline RomMap::RomMap(int width, int height, float resolution, float origin_x, float origin_y, const std::vector<int8_t>& data, const RomTime& timestamp)
    : width_(width), height_(height), resolution_(resolution), origin_x_(origin_x), origin_y_(origin_y), data_(data), timestamp_(timestamp) {}

inline void RomMap::setMap(int width, int height, float resolution, float origin_x, float origin_y, const std::vector<int8_t>& data, const RomTime& timestamp) {
    width_ = width;
    height_ = height;
    resolution_ = resolution;
    origin_x_ = origin_x;
    origin_y_ = origin_y;
    data_ = data;
    timestamp_ = timestamp;
}

inline int RomMap::width() const {
    return width_;
}

inline int RomMap::height() const {
    return height_;
}

inline float RomMap::resolution() const {
    return resolution_;
}

inline float RomMap::originX() const {
    return origin_x_;
}

inline float RomMap::originY() const {
    return origin_y_;
}

inline const std::vector<int8_t>& RomMap::data() const {
    return data_;
}

inline void RomMap::setTimestamp(const RomTime& timestamp) {
    timestamp_ = timestamp;
}

inline const RomTime& RomMap::timestamp() const {
    return timestamp_;
}

// ------------------------------------------------- ROM TF CLASS

class RomTF {
public:
    RomTF() = default;

    //QList<TransformStamped> m_transforms;

    TransformStamped map_odom;
    TransformStamped odom_base_footprint;
};

// ------------------------------------------------- COLOR CODES FOR TERMINAL OUTPUT

inline const char* ROM_COLOR_GREEN = "\033[1;32m";  // Bright green
inline const char* ROM_COLOR_RESET = "\033[0m";     // Reset to default
inline const char* ROM_COLOR_RED = "\033[1;31m";    // Bright red
inline const char* ROM_COLOR_YELLOW = "\033[1;33m"; // Bright yellow
inline const char* ROM_COLOR_BLUE = "\033[1;34m";   // Bright blue
inline const char* ROM_COLOR_MAGENTA = "\033[1;35m"; // Bright magenta
inline const char* ROM_COLOR_CYAN = "\033[1;36m";   // Bright cyan
inline const char* ROM_COLOR_WHITE = "\033[1;37m";  // Bright white
inline const char* ROM_COLOR_BLACK = "\033[1;30m";  // Bright black
inline const char* ROM_COLOR_BOLD = "\033[1m";     // Bold text
inline const char* ROM_COLOR_UNDERLINE = "\033[4m"; // Underlined text
inline const char* ROM_COLOR_REVERSED = "\033[7m"; // Reversed text
inline const char* ROM_COLOR_BOLD_GREEN = "\033[1;32m"; // Bold bright green
inline const char* ROM_COLOR_BOLD_RED = "\033[1;31m";   // Bold bright red
inline const char* ROM_COLOR_BOLD_YELLOW = "\033[1;33m"; // Bold bright yellow
inline const char* ROM_COLOR_BOLD_BLUE = "\033[1;34m";   // Bold bright blue
inline const char* ROM_COLOR_BOLD_MAGENTA = "\033[1;35m"; // Bold bright magenta
inline const char* ROM_COLOR_BOLD_CYAN = "\033[1;36m";   // Bold bright cyan

enum class Mode {
    ssh,          // default
    ros2_control,
    ekf,
    carto,
    nav2_1,
    nav2_2,
    nav2_3,
    bt,
    topic,
    log
};


inline std::string ModeToString(Mode mode)
{
    switch (mode) {
        case Mode::ssh:
            return "ssh";
        case Mode::ros2_control:
            return "ros2_control";
        case Mode::ekf:
            return "ekf";
        case Mode::carto:
            return "carto";
        case Mode::nav2_1:
            return "nav2_1";
        case Mode::nav2_2:
            return "nav2_2";
        case Mode::nav2_3:
            return "nav2_3";
        case Mode::bt:
            return "bt";
        case Mode::topic:
            return "topic";
        case Mode::log:
            return "log";
        default:
            // Handle unexpected values (good practice)
            throw std::runtime_error("Unknown Mode value.");
    }
}





}

#endif 
