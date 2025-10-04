#include "HeartRateReader.h"


int main()
{
    std::cout << "Starting program to read heart rate...\n";
    HeartRateReader* hrReader = new HeartRateReader();
    double heartRate = hrReader->readHeartRate("C:/Users/hac22/Desktop/PresageProject/codingtest.mov");
    std::cout << "Heart rate is " << heartRate << " bpm.\n";
    delete hrReader;

    return 0;
}