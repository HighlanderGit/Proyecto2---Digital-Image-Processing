#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <global_variables.h>


bool Enhancement=false;
bool PIP=false;
bool multiTracking=false;
int xposition = 0;
int yposition = 0;
bool updateXY= false;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    myPlayer = new Player();
    QObject::connect(myPlayer, SIGNAL(processedImage(QImage)),
                              this, SLOT(updatePlayerUI(QImage)));
    ui->setupUi(this);
    ui->pushButton_2->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
}

void MainWindow::updatePlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->label->setAlignment(Qt::AlignCenter);
        ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(),
                                           Qt::KeepAspectRatio, Qt::FastTransformation));
        ui->horizontalSlider->setValue(myPlayer->getCurrentFrame());
        ui->label_2->setText( getFormattedTime( (int)myPlayer->getCurrentFrame()/(int)myPlayer->getFrameRate()) );

        //ui->Yvalue_num->setText(QString::number(150));
    }
}

MainWindow::~MainWindow()
{
    delete myPlayer;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                          tr("Open Video"), ".",
                                          tr("Video Files (*.avi *.mpg *.mp4)"));
    QFileInfo name = filename;

    if (!filename.isEmpty()){
        if (!myPlayer->loadVideo(filename.toLatin1().data()))
        {
            QMessageBox msgBox;
            msgBox.setText("The selected video could not be opened!");
            msgBox.exec();
        }
        else{
            this->setWindowTitle(name.fileName());
            ui->pushButton_2->setEnabled(true);
            ui->horizontalSlider->setEnabled(true);
            ui->horizontalSlider->setMaximum(myPlayer->getNumberOfFrames());
            ui->label_3->setText( getFormattedTime( (int)myPlayer->getNumberOfFrames()/(int)myPlayer->getFrameRate()) );
        }
    }
}
void MainWindow::on_pushButton_2_clicked()
{
    if (myPlayer->isStopped())
    {
        myPlayer->Play();
        ui->pushButton_2->setText(tr("Stop"));
    }else
    {
        myPlayer->Stop();
        ui->pushButton_2->setText(tr("Play"));
    }
}


QString MainWindow::getFormattedTime(int timeInSeconds){

    int seconds = (int) (timeInSeconds) % 60 ;
    int minutes = (int) ((timeInSeconds / 60) % 60);
    int hours   = (int) ((timeInSeconds / (60*60)) % 24);

    QTime t(hours, minutes, seconds);
    if (hours == 0 )
        return t.toString("mm:ss");
    else
        return t.toString("h:mm:ss");
}

void MainWindow::on_horizontalSlider_sliderPressed()
{
    myPlayer->Stop();
}

void MainWindow::on_horizontalSlider_sliderReleased()
{
    myPlayer->Play();
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    myPlayer->setCurrentFrame(position);
    ui->label_2->setText( getFormattedTime( position/(int)myPlayer->getFrameRate()) );
}

void MainWindow::on_enhancement_clicked()
{
    if (Enhancement==true){
        Enhancement=false;
    }else{
        Enhancement=true;
    }

}

void MainWindow::on_PIP_clicked()
{
    if (PIP==true){
        PIP=false;
    }else{
        PIP=true;
    }
}

void MainWindow::on_TrackPlayers_clicked()
{
    if (multiTracking==true){
        multiTracking=false;
    }else{
        multiTracking=true;
    }
}

void MainWindow::on_Xvalue_sliderMoved(int position)
{
    xposition=position;
    ui->Xvalue_num->setText(QString::number(position));
    updateXY=true;
}

void MainWindow::on_Yvalue_sliderMoved(int position)
{
    yposition=position;
    ui->Yvalue_num->setText(QString::number(position));
    updateXY=true;

}
