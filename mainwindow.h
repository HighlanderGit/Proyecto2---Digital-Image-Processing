#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <player.h>
#include <QTime>

namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //Display video frame in player UI
    void updatePlayerUI(QImage img);
    //Slot for the load video push button.
    void on_pushButton_clicked();
    // Slot for the play push button.
    void on_pushButton_2_clicked();
    QString getFormattedTime(int timeInSeconds);

    void on_horizontalSlider_sliderPressed();

    void on_horizontalSlider_sliderReleased();

    void on_horizontalSlider_sliderMoved(int position);

    void on_enhancement_clicked();

    void on_PIP_clicked();

    void on_TrackPlayers_clicked();

    void on_Xvalue_sliderMoved(int position);

    void on_Yvalue_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    Player* myPlayer;
};
#endif // MAINWINDOW_H

