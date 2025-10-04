#include "HeartRateReader.h"
#include <opencv2/opencv.hpp>
#include <fftw3.h>

HeartRateReader::HeartRateReader()
{}

HeartRateReader::~HeartRateReader()
{}

double HeartRateReader::readHeartRate(std::string moviePath)
{
    // open the video
    cv::VideoCapture vidCap(moviePath);
    if(!vidCap.isOpened())
    {
        std::cout << "Error opening video\n";
        return -1;
    }
    // save the frames per second (fps)
    double fps = vidCap.get(cv::CAP_PROP_FPS);
   
    // configure the face classifier
    cv::CascadeClassifier faceClassifier;
    if(!faceClassifier.load("C:/opencv/sources/data/haarcascades/haarcascade_frontalface_default.xml"))
    {
        std::cout << "Could not load face classifier\n";
        return -1;
    }

    cv::Mat frame, grayFrame;
    std::vector<cv::Rect> faces;
    std::vector<double> greenChannels;

    // read in the video frame by frame
    while(vidCap.read(frame))
    {

        // if frame is empty then break
        if(frame.empty())
        {
            break;
        }

        // convert color to greyscale to use face classifier
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(grayFrame, grayFrame);

        // detect face
        faceClassifier.detectMultiScale(grayFrame, faces);
        
        if(faces.empty())
        {
            std::cout << "No faces were found\n";
            break;
        }

        // create forehead ROI
        cv::Rect forehead(faces[0].x, faces[0].y, faces[0].width, faces[0].height / 4);
        cv::rectangle(frame, forehead, cv::Scalar(0, 255, 0), 2);

        // displays portion of face that is being analyzed (forehead)
        cv::imshow("Video frame", frame);
        if(cv::waitKey(30) == 'q')
        {
            break;
        }

        if(forehead.area() == 0)
        {
            std::cout << "Could not find forehead\n";
            break;
        }
        cv::Mat foreheadRoi = frame(forehead);

        // calculate mean of each color channel
        cv::Scalar meanColor = cv::mean(foreheadRoi);
        
        // save the mean of the green color channel
        greenChannels.push_back(meanColor[1]);

    }
    // release the video capture and destroy windows
    vidCap.release();
    cv::destroyAllWindows();

    if(greenChannels.size() > 0)
    {
        // get the max frequency index
        int maxFreqIdx = getMaxFreqIdx(greenChannels, fps);
        if(maxFreqIdx == -1)
        {
            std::cout << "Could not get max frequency\n";
            return -1;
        }

        // convert to Hz
        double dominantFreqHz = static_cast<double>(maxFreqIdx) * fps / greenChannels.size();
        // return heart rate in BPM
        return dominantFreqHz * 60;
    }
    else
    {
        std::cout << "Not enough data to calculate heart rate\n";
        return -1;
    }
}

int HeartRateReader::getMaxFreqIdx(std::vector<double>& greenChannels, double fps)
{
    int N = greenChannels.size();
    // allocate memory for the FFTW inputs and outputs
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) *N);
    fftw_complex* in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

    // populate the fft input with the green channel signal
    for(int i = 0; i < N; ++i)
    {
        in[i][0] = greenChannels[i];
        in[i][1] = 0.0;
    }

    // execute the FFTW
    fftw_plan p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    // find dominant frequency in the heart rate range
    double maxPower = 0.0;
    int maxFreqIdx = 0;
    // only necessary to process half of the fft output
    for(int i = 1; i < N / 2; ++i)
    {
        double power = out[i][0] * out[i][0] + out[i][1] * out[i][1];
        double freqHz = static_cast<double>(i) * fps / N;
        double freqBpm = freqHz * 60.0;
        // filter out unrealistic heart rate freqs
        // heart rate is typically between 40 and 180 bpm
        if(freqBpm >= 40 && freqBpm <= 180 && power > maxPower)
        {
            maxPower = power;
            maxFreqIdx = i;
        }
    }

    // free the FFTW memory
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    if(maxPower > 0)
    {
        return maxFreqIdx;
    }
    return -1;
}