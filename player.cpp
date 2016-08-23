#include "player.h"

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

            //Las funciones de procesameinto de imagenes deben de ir aqui, la imagend de entrada es en este punto es BGR

            //add the code here for exmple

            std::vector<cv::Mat> ColorPlanes;

            cv::split(frame,ColorPlanes);

            cv::equalizeHist(ColorPlanes[0], ColorPlanes[0]);
            //cv::equalizeHist(ColorPlanes[1], ColorPlanes[1]);
            cv::equalizeHist(ColorPlanes[2], ColorPlanes[2]);

            cv::merge(ColorPlanes,frame);

            //La imagen se espera que este en BGR en este punto para que el video pueda ser reproducido

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
