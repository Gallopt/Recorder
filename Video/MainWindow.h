#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QPushButton>
#include <QDateTime>
#include <QDebug>
#include <QWindow>
#include <QScreen>
#include <QDir>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QFileDialog>
#include <QPoint>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QMenu>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libavformat/version.h>
    #include <libavutil/time.h>
    #include <libavutil/mathematics.h>
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    void slotGrabFullScreen();

    void on_btnStart_clicked();
    void on_btnStart2_clicked();
    void on_btnStop_clicked();

    void play();
    bool stop();
    void beforeClose();//退出之前关闭录制，做好处理

    void hideEvent(QHideEvent *event);
//    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::MainWindow *ui;
    QTimer *fpsTimer;//定时器
    QTime starTime;//计时器
    QScreen *screen;
    int count;
    QProcess *recordProcess;
    QString m_fileName;
    //ffmpeg -f gdigrab -i desktop  -pix_fmt yuv420p -vcodec libx264 -acodec aac -s 1440x900 -r 25 -q 10 -ar 44100 -ac 2 -tune zerolatency -preset ultrafast tt.mp4
    //高码率录制
    //ffmpeg -f gdigrab -i desktop  -pix_fmt yuv420p -vcodec libx264 -acodec aac  -r 25 -q 10 -ar 44100 -ac 2 -tune zerolatency -preset ultrafast tt.mp4
    //全屏录制
    QMenu *m_menu;
    QAction *playAction;
    QAction *stopAction;
    QAction *exitAction;
    bool isDrawing = false;
    bool isExit = false;
    bool isRecord = false;
    QPoint startPoint;
    QPoint endpoint;
    QRect rect;

    QSystemTrayIcon* pSystemTray;
private slots:

    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void on_timer_timeout();

};
#endif // MAINWINDOW_H
