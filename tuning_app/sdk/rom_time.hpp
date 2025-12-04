#ifndef ROM_TIME_H
#define ROM_TIME_H

namespace rom_dynamics::data_types
{
class RomTime
{
public:
    RomTime();

    RomTime(int sec, unsigned int nsec);

    int seconds() const;
    unsigned int nanoseconds() const;

    // Convert to milliseconds
    long toMilliseconds() const;
    // Convert to microseconds
    long toMicroseconds() const;

    // Static conversion helpers
    long secondsToMilliseconds(double sec);
    long secondsToMicroseconds(double sec);

private:
    int sec_;
    unsigned int nsec_;
};

}

#endif // ROM_TIME_H
