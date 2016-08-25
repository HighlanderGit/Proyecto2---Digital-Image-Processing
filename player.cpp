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

            //------------------------------------------------------------
            //------------------------------------------------------------


            //-----------------------------------------------------------------------
            //-----------------------------------------------------------------------
            //-----------------------------------------------------------------------

            //Las funciones de procesameinto de imagenes deben de ir aqui, la imagend de entrada es en este punto BGR
            //add the code here


            //Conver image to HSV
            cv::Mat frame_hsv;
            cv::cvtColor(frame, frame_hsv, CV_BGR2HSV);

            //creates an array of planes
            std::vector<cv::Mat> ColorPlanes;

            //Split the images into the different planes
            cv::split(frame_hsv,ColorPlanes);


            //Threshold of the image with the hue plane
            //double thresh =150;
            double maxValue =75;
            double minValue =26;
            cv::Mat threshold_image;

            cv::inRange(ColorPlanes[0],minValue, maxValue, threshold_image);

                 //Create vector of contours
            std::vector<std::vector<cv::Point> > contours;

            cv::Mat Contour_image = threshold_image.clone();

            findContours(Contour_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE , cv::Point(0,0));

            std::vector<cv::Moments> mu(contours.size() );
            for( int i = 0; i < contours.size(); i++ )
             { mu[i] = moments( contours[i], false ); }

            std::vector<cv::Point2f> mc (contours.size());
            for(int i =0 ; i< contours.size(); i++){

                mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

            }

            for(int i=0; i < mc.size(); i++){

                //floodFill(threshold_image, mc[i], cv::Scalar(255));
                cv::drawContours(threshold_image, contours, i, cv::Scalar(255), CV_FILLED, 8   );

            }

            cv::bitwise_not(threshold_image,threshold_image);

            findContours(Contour_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE , cv::Point(0,0));

            for (int i = 0; i< contours.size(); i++){

                if(cv::contourArea(contours[i]) < 500){

                    cv::drawContours(threshold_image, contours, i, cv::Scalar(255), CV_FILLED, 8   );
                }

            }

            cv::bitwise_not(threshold_image,threshold_image);

            cv::Mat dst1;
            ColorPlanes[0].copyTo(dst1,threshold_image);

            cv::Mat dst2;
            cv::bitwise_not(threshold_image,threshold_image);
            dst1.copyTo(dst2,threshold_image);

            ColorPlanes[0] = dst1 + dst2;

            cv::inRange(ColorPlanes[0],150, 255, ColorPlanes[0]);

            int erosion_size =3;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size) );

            cv::dilate(ColorPlanes[0], ColorPlanes[0], element);

            findContours(ColorPlanes[0], contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE , cv::Point(0,0));

            cv::Rect boundRect;
            std::vector<cv::Point> contours_poly ;

            for (int i = 0; i< contours.size(); i++){

                qDebug() << "Area " << i <<" = "<< cv::contourArea(contours[i]) ;
                //if(cv::contourArea(contours[i]) > 60){
                if(cv::contourArea(contours[i]) > 60 && cv::contourArea(contours[i]) < 300 ){

                    cv::approxPolyDP( cv::Mat(contours[i]), contours_poly, 3, true );
                    boundRect = cv::boundingRect( cv::Mat(contours_poly ));
                    cv::rectangle( frame, boundRect.tl(), boundRect.br(), cv::Scalar(255), 2, 8, 0 );
                   // cv::drawContours( frame, cv::approxPolyDP() , i, color, 1, cv::Scalar(0,0,0) , CV_FILLED , 0 );


                }

            }


            //-----------------------------------------------------------------------
            //-----------------------------------------------------------------------
            //-----------------------------------------------------------------------

            /*




            //Las funciones de procesameinto de imagenes deben de ir aqui, la imagend de entrada es en este punto es BGR

            //add the code here for exmple

            std::vector<cv::Mat> ColorPlanes;

            cv::split(frame,ColorPlanes);

            cv::equalizeHist(ColorPlanes[0], ColorPlanes[0]);
            //cv::equalizeHist(ColorPlanes[1], ColorPlanes[1]);
            cv::equalizeHist(ColorPlanes[2], ColorPlanes[2]);

            cv::merge(ColorPlanes,frame);

            //La imagen se espera que este en BGR en este punto para que el video pueda ser reproducido

            */
            //------------------------------------------------------------
            //------------------------------------------------------------


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
