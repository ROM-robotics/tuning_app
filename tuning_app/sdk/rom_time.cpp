
#include "rom_time.hpp"

namespace rom_dynamics::data_types
{


    RomTime::RomTime() : sec_(0), nsec_(0) {}

    RomTime::RomTime(int sec, unsigned int nsec) : sec_(sec), nsec_(nsec) {}

    int RomTime::seconds() const 
    {
        return sec_;
    }

    unsigned int RomTime::nanoseconds() const 
    {
        return nsec_;
    }

    // Convert to milliseconds
    long RomTime::toMilliseconds() const 
    {
        return static_cast<long>(sec_) * 1000 + nsec_ / 1000000;
    }

    // Convert to microseconds
    long RomTime::toMicroseconds() const 
    {
        return static_cast<long>(sec_) * 1000000 + nsec_ / 1000;
    }

    // Static conversion helpers
    long RomTime::secondsToMilliseconds(double sec) 
    {
        return static_cast<long>(sec * 1000.0);
    }
    long RomTime::secondsToMicroseconds(double sec) 
    {
        return static_cast<long>(sec * 1000000.0);
    }

}


