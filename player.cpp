#include "player.h"
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

#include <QDebug>

//#include <QThread>

//#include <opencv2/imgproc/imgproc.hpp>

//#include <iostream>
//#include <string>
//#include <vector>
//#include <opencv2/opencv.hpp>
//#include <cmath>

Player::Player(QObject *parent)
 : QThread(parent)
{
    stop = true;
}

bool Player::loadVideo(string filename) {

    capture  =  new cv::VideoCapture(filename);

    if (capture->isOpened())
    {
        frameRate = (int) capture->get(CV_CAP_PROP_FPS);
        return true;
    }
    else
        return false;
}

void Player::Play()
{
    if (!isRunning()) {
        if (isStopped()){
            stop = false;
        }
        start(LowPriority);
    }
}

void Player::run()
{
    int delay = (1000/frameRate);
    while(!stop){
        if (!capture->read(frame))
        {
            stop = true;
        }
        if (frame.channels()== 3){

            //-----------------------------------------------------------------------
            //-----------------------------------------------------------------------
            //-----------------------------------------------------------------------

            //From this point backward the image should be in the format BGR

            //Section 1  --------Extract hue plane


            //Conver image to HSV
            cv::Mat frame_hsv;
            cv::cvtColor(frame, frame_hsv, CV_BGR2HSV);

            //creates an array of planes in order to split the planes
            std::vector<cv::Mat> ColorPlanes;

            //Split the images into the different planes
            cv::split(frame_hsv,ColorPlanes);


            //Saves hue plane on the image hue_plane
            cv::Mat hue_plane = ColorPlanes[0].clone();

            //Section 2  --------Extract the soccer field

            //Section 2.a threshold of the image
            //Threshold ranges and the image for the threshold from the hue_plane
            double maxValue =75;
            double minValue =26;
            cv::Mat threshold_image;

            cv::inRange(hue_plane, minValue, maxValue, threshold_image);

            //Section 2.b threshold image filling the holes

            //Create vector of contours in order to find all the blobs
            std::vector<std::vector<cv::Point> > contours;

            //Creates a temporary image or buffer that is going to contain all the contours, for some reason the findContours image modifies the image this is why this image is needed.
            cv::Mat Contour_image = threshold_image.clone();

            //Finds all the blobs or particles
            findContours(Contour_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE , cv::Point(0,0));

            //Calculates all the Moments, the goal is to calculate the centroid in order to fill all the blobs of the image
            std::vector<cv::Moments> mu(contours.size() );
            for( int i = 0; i < contours.size(); i++ )
             { mu[i] = moments( contours[i], false ); }

            //Calculates all the centroids with the moments information,the goal is to fill all the holes of the image
            std::vector<cv::Point2f> mc (contours.size());
            for(int i =0 ; i< contours.size(); i++){

                mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

            }

            //Fill all the holes wiht the centroid information that was previously calculated
            for(int i=0; i < mc.size(); i++){

                //floodFill(threshold_image, mc[i], cv::Scalar(255));
                cv::drawContours(threshold_image, contours, i, cv::Scalar(255), CV_FILLED, 8   );

            }

            //Section 2.c threshold image, holes filled remove small contours

            cv::bitwise_not(threshold_image,threshold_image);

            findContours(Contour_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE , cv::Point(0,0));

            for (int i = 0; i< contours.size(); i++){

                if(cv::contourArea(contours[i]) < 500){

                    cv::drawContours(threshold_image, contours, i, cv::Scalar(255), CV_FILLED, 8   );
                }

            }

            //Section 3 generate a new image from the hue_plane applying the threshold image as a mask

            cv::bitwise_not(threshold_image,threshold_image);

            cv::Mat dst1;
            hue_plane.copyTo(dst1,threshold_image);

            cv::Mat dst2;
            cv::bitwise_not(threshold_image,threshold_image);
            dst1.copyTo(dst2,threshold_image);

            hue_plane = dst1 + dst2;

            //Section 4 identify players

            //Threshold of the hue_plane the brightes objects belongs to the color red at least in this case
            cv::inRange(hue_plane,150, 255, hue_plane);

            int erosion_size =3;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size) );

            //Dilates the image we do this in case the player has been split into two sections
            cv::dilate(hue_plane, hue_plane, element);

            //finds all the blobs and particles in order to classify as players
            findContours(hue_plane, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE , cv::Point(0,0));

            //Section 5 Generates the bounding rectacles for the blob and particles in order to draw on top of the original frame
            cv::Rect boundRect;
            std::vector<cv::Point> contours_poly ;
            // Section 6 Prepares to calculate the closest rectangle to the position of the mouse click.
            cv::Rect closest = cv::boundingRect( cv::Mat(contours_poly )); // Instead of this, the mouse position should be put here.
            cv::Rect position_past = cv::boundingRect( cv::Mat(contours_poly ));
            
            float distance_current = 0.0;
            float distance_past = 0.0;

            //Calculates the areas of the blobs and chooses if the blob is gargabe or if it a player
            for (int i = 0; i< contours.size(); i++){

                if(cv::contourArea(contours[i]) > 60 && cv::contourArea(contours[i]) < 300 ){

                    cv::approxPolyDP( cv::Mat(contours[i]), contours_poly, 3, true );
                    boundRect = cv::boundingRect( cv::Mat(contours_poly ));
                    distance_current = sqrt(pow(boundRect.x-position_past.x,2)-pow(boundRect.y-position_past.y,2));
                    if (distance_current < distance_past) {
                        closest = boundRect;
                    }
                    distance_past=distance_current;


                    if (multiTracking = true){

                        cv::rectangle( frame, boundRect.tl(), boundRect.br(), cv::Scalar(255), 2, 8, 0 );
                    }
                }


            }

            if(pipActive && contours.size()!=0){ // bool pipActive should be user modifiable.
                //----------  Picture in Picture    ----------------

                cv::Rect roi1 = cv::Rect(closest.x,closest.y,50,50);
                cv::Rect pip = cv::Rect(frame.cols-2*roi1.width-50,50,2*roi1.width,2*roi1.height);
                cv::Mat small = frame(roi1);
                cv::Mat subView = frame(pip);
                cv::resize(small, subView, subView.size(), 0, 0, INTER_LINEAR);
            }
            //-----------------------------------------------------------------------
            //-----------------------------------------------------------------------
            //-----------------------------------------------------------------------

            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
        }
        else
        {
            img = QImage((const unsigned char*)(frame.data),
                                 frame.cols,frame.rows,QImage::Format_Indexed8);
        }
        emit processedImage(img);
        this->msleep(delay);
    }
}

Player::~Player()
{
    mutex.lock();
    stop = true;
    capture->release();
    delete capture;
    condition.wakeOne();
    mutex.unlock();
    wait();
}
void Player::Stop()
{
    stop = true;
}
void Player::msleep(int ms){
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}
bool Player::isStopped() const{
    return this->stop;
}

double Player::getCurrentFrame(){

    return capture->get(CV_CAP_PROP_POS_FRAMES);
}

double Player::getNumberOfFrames(){

    return capture->get(CV_CAP_PROP_FRAME_COUNT);
}

double Player::getFrameRate(){
    return frameRate;
}

void Player::setCurrentFrame( int frameNumber )
{
    capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);
}
