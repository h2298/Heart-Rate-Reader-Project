#include <vector>
#include <iostream>

class HeartRateReader
{
public:
    // constructor
    HeartRateReader();

    // destructor
    ~HeartRateReader();

    // this method returns the estimated heart rate in BPM
    // read from a video input
    double readHeartRate(std::string moviePath);

private:
    // this method returns the max frequency index
    int getMaxFreqIdx(std::vector<double>& greenChannels, double fps);

    

};