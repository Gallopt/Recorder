
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <opencv2/opencv.hpp>
#include <QMessageBox>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->startBtn->setEnabled(true);
    ui->stopBtn->setEnabled(false);
    ui->lcdNumber->display("00:00:00");
    setWindowFlags(Qt::WindowSystemMenuHint
                   | Qt::WindowCloseButtonHint
                   | Qt::WindowMinimizeButtonHint);//缩小到托盘
    playAction = new QAction;
    playAction->setText(tr("Start"));

    stopAction = new QAction;
    stopAction->setText(tr("Stop"));

    exitAction = new QAction;
    exitAction->setText(tr("Exit"));

    m_menu = new QMenu;
    m_menu->addAction(playAction);
    m_menu->addAction(stopAction);
    m_menu->addSeparator();
    m_menu->addAction(exitAction);
    pSystemTray = new QSystemTrayIcon();
    pSystemTray->setVisible(true);
    pSystemTray->setContextMenu(m_menu);
    if (NULL != pSystemTray)
    {
        pSystemTray->setIcon(QIcon(":/icon.png"));
        pSystemTray->setToolTip("Recorder");
        pSystemTray->show();
    }

    recordProcess = new QProcess;
    fpsTimer = new QTimer(this);

    connect(fpsTimer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
    connect(ui->startBtn, &QPushButton::clicked, [=](){
        on_btnStart_clicked();
        //slotGrabFullScreen();
    });
    //connect(ui->startBtn_2, SIGNAL(clicked()), this, SLOT(on_btnStart2clicked()));
//    connect(ui->startBtn_2, &QPushButton::clicked, [=](){
//        on_btnStart2_clicked();
//        //slotGrabFullScreen();
//    });
    connect(ui->stopBtn, &QPushButton::clicked, [=](){
        on_btnStop_clicked();
    });

   //connect(pSystemTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedSysTrayIcon);
   connect(pSystemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
   connect(playAction, &QAction::triggered, this, &MainWindow::play);
   connect(stopAction, &QAction::triggered, this, &MainWindow::stop);
   connect(exitAction, &QAction::triggered, this, &MainWindow::beforeClose);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete pSystemTray;
}

void MainWindow::on_btnStart_clicked()
{
    play();
}

void MainWindow::on_btnStart2_clicked()
{
    ui->startBtn->setEnabled(false);
    //ui->startBtn_2->setEnabled(false);
    ui->stopBtn->setEnabled(true);
}

void MainWindow::on_btnStop_clicked()
{
    stop();
}

void MainWindow::play()
{
    isRecord = true;
    ui->startBtn->setEnabled(false);
    playAction->setEnabled(false);
    stopAction->setEnabled(true);
    //ui->startBtn_2->setEnabled(false);
    ui->stopBtn->setEnabled(true);
    this->hide();
    auto current_date_time = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");
    starTime = QTime::currentTime();
    ui->lcdNumber->display("00:00:00");
    m_fileName = "video_" +current_date_time+ ".mp4";
    fpsTimer->start();
    fpsTimer->setInterval(1000);
    QStringList args;
    args << "-f"            << "gdigrab"
         << "-framerate"    << "30"
         //<< "-offset_x"     << QString("%1").arg(selected_area.x())
         //<< "-offset_y"     << QString("%1").arg(selected_area.y())
         //<< "-video_size"   << QString("%1x%2").arg(selected_area.width()).arg(selected_area.height())
         << "-i"            << "desktop"
         << "-pix_fmt"      << "yuv420p"
         << "-preset"       << "ultrafast"
         << "tmp.mp4";
    recordProcess->start("ffmpeg", args);
    //ffmpeg -f gdigrab -i desktop -pix_fmt yuv420p -vcodec libx264 -preset ultrafast someone30.mp4
    //qDebug()<<"f"<<avcodec_configuration();

    //unsigned version = avcodec_version();
//    QString ch = QString::number(version,10);
//    qDebug()<<"version"<<version;
#if 0
            QStringList args;
            auto selected_area = selected();
        #ifdef __linux__
            args << "-f"            << "x11grab"
                 << "-framerate"    << QString("%1").arg(framerate_)
                 << "-video_size"   << QString("%1x%2").arg(selected_area.width()).arg(selected_area.height())
                 << "-i"            << QString(":0.0+%1,%2").arg(selected_area.x()).arg(selected_area.y())
                 << "-pix_fmt"      << "yuv420p"
                 << "-vf"           << "scale=trunc(iw/2)*2:trunc(ih/2)*2"
                 << filename_;
        #elif _WIN32
            auto audio_devices = Devices::audioDevices();
            if(!audio_devices.empty() && !mute_) {
                args << "-f" << "dshow"
                     << "-i" << "audio=" + audio_devices.at(0).first;
            }
            args << "-f"            << "gdigrab"
                 << "-framerate"    << QString("%1").arg(framerate_)
                 << "-offset_x"     << QString("%1").arg(selected_area.x())
                 << "-offset_y"     << QString("%1").arg(selected_area.y())
                 << "-video_size"   << QString("%1x%2").arg(selected_area.width()).arg(selected_area.height())
                 << "-i"            << "desktop"
                 << "-pix_fmt"      << "yuv420p"
                 << "-vf"           << "scale=trunc(iw/2)*2:trunc(ih/2)*2"
                 << filename_;

            LOG(INFO) << args;
        #endif
            process_->start("ffmpeg", args);
            process_->write("q\n\r");
#endif
#if 0
    fpsTimer->start();
    fpsTimer->setInterval(10);//ms
    fTimeCounter.start();
    QString path = QDir::currentPath();//当前工作路径
    path += "/images/photoscreen/";

    QWindow *window = windowHandle();
    QScreen *screen = window->screen();
    QPixmap pixmap = screen->grabWindow(0);
    Mat image;

    VideoWriter vWriter;
        vWriter << image;//必须是fileName 文件路径。。
#endif
}

bool MainWindow::stop()
{
    isRecord = false;
    recordProcess->write("q\n\r");
    fpsTimer->stop();
    ui->startBtn->setEnabled(true);
    playAction->setEnabled(true);
    stopAction->setEnabled(false);
    //ui->startBtn_2->setEnabled(true);
    ui->stopBtn->setEnabled(false);
    if(!isExit)
    {
        this->show();
    }
    QFile file("tmp.mp4");
    QString fileName = QFileDialog::getSaveFileName(this, "保存视频",m_fileName,"MP4 (*.mp4)");
    //bool exist = QFile::exists(fileName);
    if(!fileName.isNull())
    {
        if(file.rename(fileName))
        {
            QMessageBox msgBox;
            msgBox.setText(QString("录像已保存至：%1").arg(fileName).toUtf8());
            msgBox.exec();
        }
    }
    else
    {
        file.remove();
    }

}

void MainWindow::beforeClose()
{
    qDebug()<<"isREcord:"<<isRecord;
    if(isRecord)
    {
        isExit = true;//退出程序之前不显示主界面
        if(stop())
        {
            this->close();
        }
    }
    else
    {
        this->close();
    }
    //   --isRecord false--start--isRecord true---stop---isRecord false----start---true----
    //      close                 close                   close
}

void MainWindow::hideEvent(QHideEvent *event)
{
    if(pSystemTray->isVisible())
    {
        this->hide();

        pSystemTray->showMessage("Title","已最小化至托盘");
        event->ignore();
    }
}

//void MainWindow::closeEvent(QCloseEvent *event)
//{
//    if(pSystemTray->isVisible())
//    {
//        hide();
//        pSystemTray->showMessage("Title","已隐藏至托盘");
//        event->ignore();
//    }
//}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isDrawing = true;
        startPoint = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(isDrawing)
    {
        endpoint = event->pos();
        rect = QRect(startPoint, endpoint);

        //双缓冲机制，在临时画布上画虚线矩形框

    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isDrawing = false;
        endpoint = event->pos();
        rect = QRect(startPoint, endpoint);
        //在最终界面上画已确定的虚线矩形框
    }
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        this->show();
        break;
    default:
        break;
    }
}

void MainWindow::slotGrabFullScreen()
{
    screen = QGuiApplication::primaryScreen();

    QString filePathName = "I:\\Demo\\Video\\Video\\";
    filePathName += "full-";
    filePathName += QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss-zzz");
    filePathName += ".jpg";
    if(!screen->grabWindow(0).save(filePathName, "jpg"))
    {
        qDebug()<<"save full screen failed";
    }
}

void MainWindow::on_timer_timeout()//
{
#if 0
    //QTime curTime = QTime::currentTime();
    count++;
    screen = QGuiApplication::primaryScreen();

    QString filePathName = "I:\\Demo\\Video\\Video\\";
    filePathName += QString("full-%1-").arg(count);
    //qDebug()<<QDateTime::currentDateTime().toString("ss-zzz");
    //filePathName += QDateTime::currentDateTime().toString("ss-zzz");
    filePathName += ".jpg";
    if(!screen->grabWindow(0).save(filePathName, "jpg"))
    {
        qDebug()<<"save full screen failed";
    }
#endif
    QTime curTime = QTime::currentTime();
    int t = starTime.secsTo(curTime);
    QTime disTime(0,0,0);
    disTime = disTime.addSecs(t);
    QString timm = disTime.toString("hh:mm:ss");
    ui->lcdNumber->display(timm);
}




