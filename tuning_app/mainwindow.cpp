#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sdk/rom_map_widget.hpp"
#include "sdk/carto_rom_map_widget.hpp"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QTabBar>
#include <QtGui/QMouseEvent>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QRandomGenerator>
#include <QObject>
#include <QQmlContext>
#include <QQuickItem>
#include <QTimer>
#include <QDateTime>
#include <cmath>

#include "design/rom_design.hpp"

#include <QDir>
#include "design/readmeviewer.h"
//#include "design/covarianceDisplay.hpp"
#include "sdk/rom_map_widget.hpp"

using rom_dynamics::ui::qt::RomMapWidget;
using rom_dynamics::ui::qt::CartoRomMapWidget;
using rom_dynamics::ui::qt::RomPolarHeadingGraph;
using rom_dynamics::ui::qt::RomPositionGraph;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    
    if (ui->tabWidget && ui->tabWidget->tabBar()) 
    {   // tab bar တွေညီအောင်လို့။
        int tabCount = ui->tabWidget->count();
        if (tabCount > 0) 
        {
            int totalW = ui->tabWidget->width() - 30;
            int perTabW = qMax(1, totalW / tabCount);
            QString tabStyle = QString("QTabBar::tab { min-width: %1px; max-width: %1px; }").arg(perTabW);
            ui->tabWidget->tabBar()->setStyleSheet(tabStyle);
        }
    }

    ui->ipLineEdit->setFocus();

    // signal and slots 
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    currentMode = Mode::ssh;
    previousMode = Mode::ssh;
    qDebug() << " currentMode = " << ModeToString(currentMode).c_str();

    initRos2ControlTab();
    initEkfTab();
    initCartoTab();
    initNav2_1Tab();
    initNav2_2Tab();
    initNav2_3Tab();
    initGoalTab();
    initBtTab();
    initTopicTab();
    initLogTab();
}

void MainWindow::setConnectionParams(const QString &ip, const QString &password, const QString &ns)
{
    ui->ipLineEdit->setText(ip);
    ui->passwordLineEdit->setText(password);
    if (!ns.isEmpty()) {
        ui->nsLineEdit->setText(ns);
    }
}

void MainWindow::autoConnect()
{
    // Automatically trigger the connect button click
    QTimer::singleShot(100, this, &MainWindow::on_connectBtn_clicked);
}

void MainWindow::switchToControlTab()
{
    // Switch to control tab (index 1) after successful connection
    QTimer::singleShot(500, this, [this]() {
        if (this->isConnected_ && ui->tabWidget) {
            ui->tabWidget->setCurrentIndex(1); // ros2_control tab
        }
    });
}

MainWindow::~MainWindow()
{
    // Program ပိတ်ရင် Thread ကို စနစ်တကျ ရပ်ဖို့ သေချာပါစေ
    if (communicationThread_ && communicationThread_->isRunning()) {
        communicationThread_->quit();
        communicationThread_->wait(); 
    }
    delete ui;
}


// SLOT METHODS
void MainWindow::onTabChanged(int index)
{
    qDebug() << "is connected status: " << this->isConnected_; 

    previousMode = currentMode;

    switch (index) 
    {
        case 0:
            currentMode = Mode::ssh;
            break;
        case 1:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::ros2_control;
            break;
        case 2:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::ekf;
            break;
        case 3:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::carto;
            break;
        case 4:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::nav2_1;
            break;
        case 5:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::nav2_2;
            break;
        case 6:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::nav2_3;
            break;
        case 7:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::goal;
            break;
        case 8:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::bt;
            break;
        case 9:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::topic;
            break;
        case 10:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::log;
            break;
        default:
            // Handle unexpected index
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            break;
    }
    qDebug() << " currentMode = " << ModeToString(currentMode).c_str();


    switch (currentMode) 
    {
        case Mode::ssh:

            // deactivateRos2ControlTab();
            // deactivateEkfTab();
            // deactivateCartoTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_2Tab();
            // deactivateNav2_3Tab();
            // deactivateBtTab();
            // deactivateTopicTab();
            // deactivateLogTab();
            qDebug() << " Deactivate All. ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }
            break;
        case Mode::ros2_control:
            activateRos2ControlTab();

            // deactivateEkfTab();
            // deactivateCartoTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_2Tab();
            // deactivateNav2_3Tab();
            // deactivateGoalTab();
            // deactivateBtTab();
            // deactivateTopicTab();
            // deactivateLogTab();
            qDebug() << " activateRos2ControlTab called  ";

            switch(previousMode)
            {
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }
            break;
        case Mode::ekf:
            activateEkfTab();

            // deactivateRos2ControlTab();
            // deactivateCartoTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_2Tab();
            // deactivateNav2_3Tab();
            // deactivateGoalTab();
            // deactivateBtTab();
            // deactivateTopicTab();
            // deactivateLogTab();

            qDebug() << " activateEkfTab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }
            break;
        case Mode::carto:
            activateCartoTab();

            // deactivateRos2ControlTab();
            // deactivateEkfTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_2Tab();
            // deactivateNav2_3Tab();
            // deactivateGoalTab();
            // deactivateBtTab();
            // deactivateTopicTab();
            // deactivateLogTab();
            qDebug() << " activateCartoTab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }

            break;
        case Mode::nav2_1:
            activateNav2_1Tab();

            // deactivateRos2ControlTab();
            // deactivateEkfTab();
            // deactivateCartoTab();
            // deactivateNav2_2Tab();
            // deactivateNav2_3Tab();
            // deactivateGoalTab();
            // deactivateBtTab();
            // deactivateTopicTab();
            // deactivateLogTab();
            qDebug() << " activateNav2_1Tab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }

            break;
        case Mode::nav2_2:
            activateNav2_2Tab();

            // deactivateRos2ControlTab();
            // deactivateEkfTab();
            // deactivateCartoTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_3Tab();
            // deactivateGoalTab();
            // deactivateBtTab();
            // deactivateTopicTab();
            // deactivateLogTab();
            qDebug() << " activateNav2_2Tab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }

            break;
        case Mode::nav2_3:
            activateNav2_3Tab();

            // deactivateRos2ControlTab();
            // deactivateEkfTab();
            // deactivateCartoTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_2Tab();
            // deactivateGoalTab();
            // deactivateBtTab();
            // deactivateTopicTab();
            // deactivateLogTab();
            qDebug() << " activateNav2_3Tab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }

            break;
        case Mode::goal:
            activateGoalTab();

            qDebug() << " activateGoalTab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }

            break;
        case Mode::bt:
            activateBtTab();

            // deactivateRos2ControlTab();
            // deactivateEkfTab();
            // deactivateCartoTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_2Tab();
            // deactivateNav2_3Tab();
            // deactivateGoalTab();
            // deactivateTopicTab();
            // deactivateLogTab();
            qDebug() << " activateBtTab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }

            break;
        case Mode::topic:
            activateTopicTab();

            // deactivateRos2ControlTab();
            // deactivateEkfTab();
            // deactivateCartoTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_2Tab();
            // deactivateNav2_3Tab();
            // deactivateGoalTab();
            // deactivateBtTab();
            // deactivateLogTab();
            qDebug() << " activateTopicTab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::log:
                    deactivateLogTab();
                    break;
                default:
                    break;
            }

            break;
        case Mode::log:
            activateLogTab();

            // deactivateRos2ControlTab();
            // deactivateEkfTab();
            // deactivateCartoTab();
            // deactivateNav2_1Tab();
            // deactivateNav2_2Tab();
            // deactivateNav2_3Tab();
            // deactivateGoalTab();
            // deactivateBtTab();
            // deactivateTopicTab();
            qDebug() << " activateBtTab called  ";

            switch(previousMode)
            {
                case Mode::ros2_control:
                    deactivateRos2ControlTab();
                    break;
                case Mode::ekf:
                    deactivateEkfTab();
                    break;
                case Mode::carto:
                    deactivateCartoTab();
                    break;
                case Mode::nav2_1:
                    deactivateNav2_1Tab();
                    break;
                case Mode::nav2_2:
                    deactivateNav2_2Tab();
                    break;
                case Mode::nav2_3:
                    deactivateNav2_3Tab();
                    break;
                case Mode::goal:
                    deactivateGoalTab();
                    break;
                case Mode::bt:
                    deactivateBtTab();
                    break;
                case Mode::topic:
                    deactivateTopicTab();
                    break;
                default:
                    break;
            }

            break;
        default:
            // Handle unexpected index
            break;
    }
    //qDebug() << "Done";
}
void MainWindow::on_rsyncBtn_clicked()
{
    qDebug() << "Rsync button clicked!";

    QString ip = ui->ipLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString robot_ns = ui->nsLineEdit->text().trimmed();

    if (ip.isEmpty()) 
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Missing IP");
        dlg.setFixedSize(400, 100);
        QLabel *label = new QLabel("Please enter the robot IP address.", &dlg);
        label->setAlignment(Qt::AlignCenter);
        QPushButton *okBtn = new QPushButton("OK", &dlg);
        okBtn->setGeometry(150, 150, 100, 30);

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->ipLineEdit) ui->ipLineEdit->setFocus();
        });

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(okBtn);
        dlg.setLayout(layout);
        // Center dialog over main window
        QPoint center = this->geometry().center();
        QPoint globalCenter = this->mapToGlobal(center);
        int dlgX = globalCenter.x() - dlg.width() / 2;
        int dlgY = globalCenter.y() - dlg.height() / 2;
        dlg.move(dlgX, dlgY);
        dlg.exec();
        return;
    }
    if (password.isEmpty()) 
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Missing Password");
        dlg.setFixedSize(400, 100);
        QLabel *label = new QLabel("Please enter the password.", &dlg);
        label->setAlignment(Qt::AlignCenter);
        QPushButton *okBtn = new QPushButton("OK", &dlg);
        okBtn->setGeometry(150, 150, 100, 30);

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->passwordLineEdit) ui->passwordLineEdit->setFocus();
        });

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(okBtn);
        dlg.setLayout(layout);
        // Center dialog over main window
        QPoint center = this->geometry().center();
        QPoint globalCenter = this->mapToGlobal(center);
        int dlgX = globalCenter.x() - dlg.width() / 2;
        int dlgY = globalCenter.y() - dlg.height() / 2;
        dlg.move(dlgX, dlgY);
        dlg.exec();
        return;
    }

    // Store in class variable
    this->robotIp_ = ip;
    this->password_ = password;
    this->robotNamespace_ = robot_ns;

    createCommunicationClient(this->robotNamespace_, this->robotIp_, this->robotPort_.toInt());

    qDebug() << "robotIp_: " << this->robotIp_;
    qDebug() << "password_ : " << this->password_;
    qDebug() << "robotNamespace_ : " << this->robotNamespace_;

    // update algo =======================================
    QString rsync_app = "";

    // If the current Linux user is "mr_robot", prefer the fixed path under /home/mr_robot/data/app
    const QString user = QString::fromLocal8Bit(qgetenv("USER"));
    if (user == "mr_robot" || user == "jackal" || user == "zyme") 
    {
        QString preferred = "";
        if( user == "mr_robot" )    {  preferred = "/home/mr_robot/data/app/rsync_qt/mr_robot/apprsync_qt";   }
        else if( user == "jackal" ) {  preferred = "/home/jackal/data/app/rsync_qt/jackal/apprsync_qt";   }
        else if( user == "zyme" )   {  preferred = "/home/zyme/data/app/rsync_qt/zyme/apprsync_qt";   }

        if (QFileInfo::exists(preferred)) 
        {
            rsync_app = preferred;
        } else 
        {
            QDialog dlg(this);
            dlg.setWindowTitle("rsync app not found");
            dlg.setFixedSize(400, 100);
            QLabel *label = new QLabel("check /home/mr_robot/data/app/rsync_qt/USER/apprsync_qt", &dlg);
            label->setAlignment(Qt::AlignCenter);
            QPushButton *okBtn = new QPushButton("OK", &dlg);
            okBtn->setGeometry(150, 150, 100, 30);

            //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
            QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
                dlgPtr->accept();
                if (ui && ui->ipLineEdit) ui->ipLineEdit->setFocus();
            });

            QVBoxLayout *layout = new QVBoxLayout;
            layout->addWidget(label);
            layout->addWidget(okBtn);
            dlg.setLayout(layout);
            // Center dialog over main window
            QPoint center = this->geometry().center();
            QPoint globalCenter = this->mapToGlobal(center);
            int dlgX = globalCenter.x() - dlg.width() / 2;
            int dlgY = globalCenter.y() - dlg.height() / 2;
            dlg.move(dlgX, dlgY);
            dlg.exec();
            return;

        }
    }
    else
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Not Development Platform");
        dlg.setFixedSize(400, 100);
        QLabel *label = new QLabel("Development need ubuntu 22.04 with user name mr_robot || zyme || jackal, and ~/Desktop/Git/rom_robotics.", &dlg);
        label->setAlignment(Qt::AlignCenter);
        QPushButton *okBtn = new QPushButton("OK", &dlg);
        okBtn->setGeometry(150, 150, 100, 30);

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->ipLineEdit) ui->ipLineEdit->setFocus();
        });

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(okBtn);
        dlg.setLayout(layout);
        // Center dialog over main window
        QPoint center = this->geometry().center();
        QPoint globalCenter = this->mapToGlobal(center);
        int dlgX = globalCenter.x() - dlg.width() / 2;
        int dlgY = globalCenter.y() - dlg.height() / 2;
        dlg.move(dlgX, dlgY);
        dlg.exec();
        return;
    }

    qDebug() << "Using rsync app:" << rsync_app;
    // end update algo ====================================
    QStringList arguments;
    arguments << "--ip" << robotIp_ << "--password" << password;
    
    QProcess *proc = new QProcess(this);
    proc->start(rsync_app, arguments);
    //proc->start(rsync_app);
}
void MainWindow::on_connectBtn_clicked()
{
    qDebug() << "Connection button clicked!";

    QString ip = ui->ipLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString robot_ns = ui->nsLineEdit->text().trimmed();

    if (ip.isEmpty()) 
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Missing IP");
        dlg.setFixedSize(400, 100);
        QLabel *label = new QLabel("Please enter the robot IP address.", &dlg);
        label->setAlignment(Qt::AlignCenter);
        QPushButton *okBtn = new QPushButton("OK", &dlg);
        okBtn->setGeometry(150, 150, 100, 30);

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->ipLineEdit) ui->ipLineEdit->setFocus();
        });

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(okBtn);
        dlg.setLayout(layout);
        // Center dialog over main window
        QPoint center = this->geometry().center();
        QPoint globalCenter = this->mapToGlobal(center);
        int dlgX = globalCenter.x() - dlg.width() / 2;
        int dlgY = globalCenter.y() - dlg.height() / 2;
        dlg.move(dlgX, dlgY);
        dlg.exec();
        return;
    }
    if (password.isEmpty()) 
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Missing Password");
        dlg.setFixedSize(400, 100);
        QLabel *label = new QLabel("Please enter the password.", &dlg);
        label->setAlignment(Qt::AlignCenter);
        QPushButton *okBtn = new QPushButton("OK", &dlg);
        okBtn->setGeometry(150, 150, 100, 30);

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->passwordLineEdit) ui->passwordLineEdit->setFocus();
        });

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(okBtn);
        dlg.setLayout(layout);
        // Center dialog over main window
        QPoint center = this->geometry().center();
        QPoint globalCenter = this->mapToGlobal(center);
        int dlgX = globalCenter.x() - dlg.width() / 2;
        int dlgY = globalCenter.y() - dlg.height() / 2;
        dlg.move(dlgX, dlgY);
        dlg.exec();
        return;
    }

    // Store in class variable
    this->robotIp_ = ip;
    this->password_ = password;
    this->robotNamespace_ = robot_ns;

    createCommunicationClient(this->robotNamespace_, this->robotIp_, this->robotPort_.toInt());

    qDebug() << "robotIp_: " << this->robotIp_;
    qDebug() << "password_ : " << this->password_;
    qDebug() << "robotNamespace_ : " << this->robotNamespace_;
}
void MainWindow::on_closeBtn_clicked()
{
    QCoreApplication::quit();
}
void MainWindow::on_hostTerminalBtn_clicked()
{
    QStringList colorCmds = {
        "echo -e '\\e]11;rgb:00/AA/BB\\a'", // echo_style1
        "echo -e '\\e]11;rgb:00/00/00\\a'", // class_black
        "echo -e '\\e]11;rgb:FF/FF/FF\\a'", // bright_white
        "echo -e '\\e]11;rgb:00/00/80\\a'", // navy_blue
        "echo -e '\\e]11;rgb:22/8B/22\\a'", // forest_green
        "echo -e '\\e]11;rgb:00/FF/FF\\a'", // vibrant_cyan
        "echo -e '\\e]11;rgb:F0/E6/8C\\a'", // warm_sepia
        "echo -e '\\e]11;rgb:4B/00/82\\a'", // deep_purple
        "echo -e '\\e]11;rgb:A9/A9/A9\\a'", // soft_gray
        "echo -e '\\e]11;rgb:FF/A5/00\\a'", // bold_old
        "echo -e '\\e]11;rgb:FF/69/B4\\a'"  // hot_pink
    };

    int idx = QRandomGenerator::global()->bounded(colorCmds.size());
    while( idx == previousCmdColorIndex )
    {
        idx = QRandomGenerator::global()->bounded(colorCmds.size());
    }

    previousCmdColorIndex = idx;
    QString colorCmd = colorCmds[idx]; 

    QStringList args;
    args << "--geometry=80x25-0+0";
    args << "--" << "bash" << "-c" << colorCmd + "; exec bash";
    QProcess::startDetached("gnome-terminal", args);
   
}
void MainWindow::on_robotTerminalBtn_clicked()
{
    qDebug() << "robotIp_ set to: " << robotIp_;
    qDebug() << "input_password set to: " << password_;

    QString sshCmd = QString("sshpass -p '%1' ssh mr_robot@%2").arg(password_, robotIp_);
    QStringList args;
    args << "--" << "bash" << "-c" << sshCmd;
    QProcess::startDetached("gnome-terminal", args);
}
void MainWindow::createCommunicationClient(const QString &robot_ns, const QString &host, quint16 port)
{
    qDebug() << "Hacked" ;
    this->isConnected_ = true;

    // အရင် thread နဲ့ client ကို ရှင်းလင်းခြင်း
    if (communicationThread_ && communicationThread_->isRunning()) 
    {
        // Thread ကို ဖြတ်ဖို့ စောင့်ဆိုင်းခြင်း
        communicationThread_->quit();
        communicationThread_->wait();
        
        // deleteLater ကို ခေါ်ရင် communication_ object ကို thread က delete လုပ်ပေးပါလိမ့်မယ်။
        // communication_->deleteLater(); // (သို့) QThread::finished မှာ ချိတ်ဆက်ထားရင် ပိုကောင်း)
    }
    delete communicationThread_;
    communicationThread_ = nullptr;
    communication_ = nullptr;

    communicationThread_ = new QThread(this);
    communication_ = new RosBridgeClient(robot_ns, host, port, nullptr); 
    communication_->moveToThread(communicationThread_);

    connect(communicationThread_, 
        &QThread::started, 
        communication_, 
        &RosBridgeClient::init);
        
    // Thread စတင်ပြီးမှသာ connectToServer() ကို ခေါ်သင့်ပါတယ်။
    connect(communicationThread_, 
        &QThread::started, 
        communication_, 
        &RosBridgeClient::connectToServer, 
        Qt::QueuedConnection); // worker thread အချင်းချင်းမို့ QueuedConnection မလိုပါဘူး။
            
    // Resource ရှင်းလင်းခြင်း Thread ပြီးသွားရင် Worker object ကို အလိုအလျောက် ဖျက်ပေးဖို့
    connect(communicationThread_, 
        &QThread::finished, 
        communication_, 
        &QObject::deleteLater);

    connect(communication_, 
        &RosBridgeClient::receivedTopicMessage, 
        this, 
        &MainWindow::onReceivedTopicMessage, 
        Qt::QueuedConnection); 

    connect(communication_, 
        &RosBridgeClient::receivedServiceResponse, 
        this, 
        &MainWindow::onReceivedServiceResponse, 
        Qt::QueuedConnection);

    connect(communication_,
        &RosBridgeClient::receivedActionFeedback,
        this,
        &MainWindow::onReceivedActionFeedback,
        Qt::QueuedConnection);
            
    // Worker object က disconnected signal ထုတ်လွှင့်ရင် Thread ကို ရပ်ဖို့
    // လောလောဆယ်မလိုဘူး။ reconnect လုပ်ချင်တာမို့။
    // connect(communication_, 
    //     &RosBridgeClient::disconnected, 
    //     communicationThread_, 
    //     &QThread::quit); 
            
    // ⭐ 5. Thread ကို စတင်တယ်။ (ဒီအချိန်မှသာ Client ရဲ့ Event Loop စပါလိမ့်မယ်)
    communicationThread_->start();

    // -------------------------------------------------------------
}
void MainWindow::on_ekfTuningGuideBtn_clicked()
{
    //QString ekf_tuning_guide_url = "https://docs.ros.org/en/foxy/Tutorials/Localization/Understanding-EKF-Localization-With-Nav2.html";
    //QDesktopServices::openUrl(QUrl(ekf_tuning_guide_url));

    QDir dir = QDir::current();
    dir.cdUp(); dir.cdUp(); dir.cdUp();
    QString upTwoDirs = dir.path();
    QString filePath = upTwoDirs + "/README/ekf_guide.md";
    
    qDebug() << "EKF guide path:" << filePath;

    ReadmeDialog dlg(this, filePath);
    dlg.exec();
}
void MainWindow::on_initialNoiseCovToggleBtn_clicked()
{
    bool currentState = ui->initialNoiseXBtn->isVisible();

    //=================================
    ui->initialNoiseXBtn->setVisible(!currentState);
    ui->initialNoiseYBtn->setVisible(!currentState);
    ui->initialNoiseZBtn->setVisible(!currentState);

    ui->initialNoiseRollBtn->setVisible(!currentState);
    ui->initialNoisePitchBtn->setVisible(!currentState);
    ui->initialNoiseYawBtn->setVisible(!currentState);

    ui->initialNoiseVXBtn->setVisible(!currentState);
    ui->initialNoiseVYBtn->setVisible(!currentState);
    ui->initialNoiseVZBtn->setVisible(!currentState);

    ui->initialNoiseVRollBtn->setVisible(!currentState);
    ui->initialNoiseVPitchBtn->setVisible(!currentState);
    ui->initialNoiseVYawBtn->setVisible(!currentState);

    ui->initialNoiseAXBtn->setVisible(!currentState);
    ui->initialNoiseAYBtn->setVisible(!currentState);
    ui->initialNoiseAZBtn->setVisible(!currentState);
}

void MainWindow::on_processNoiseCovToggleBtn_clicked()
{
    bool currentState = ui->processNoiseXBtn->isVisible();
    //=================================
    ui->processNoiseXBtn->setVisible(!currentState);
    ui->processNoiseYBtn->setVisible(!currentState);
    ui->processNoiseZBtn->setVisible(!currentState);

    ui->processNoiseRollBtn->setVisible(!currentState);
    ui->processNoisePitchBtn->setVisible(!currentState);
    ui->processNoiseYawBtn->setVisible(!currentState);

    ui->processNoiseVXBtn->setVisible(!currentState);
    ui->processNoiseVYBtn->setVisible(!currentState);
    ui->processNoiseVZBtn->setVisible(!currentState);

    ui->processNoiseVRollBtn->setVisible(!currentState);
    ui->processNoiseVPitchBtn->setVisible(!currentState);
    ui->processNoiseVYawBtn->setVisible(!currentState);

    ui->processNoiseAXBtn->setVisible(!currentState);
    ui->processNoiseAYBtn->setVisible(!currentState);
    ui->processNoiseAZBtn->setVisible(!currentState);
}

// OUR UI METHODS
void MainWindow::initRos2ControlTab()
{
    if (ui->ros2_control)
    {
        qDebug() << " Initializing ROS 2 Control Tab UI components ";
        // Remove any existing layout
        QLayout *existing = ui->ros2_control->layout();
        if (existing) 
        {
            delete existing;
        }

        // --- Create a vertical layout (main container) ---
        QVBoxLayout *vLayout = new QVBoxLayout(ui->ros2_control);
        vLayout->setContentsMargins(8, 8, 8, 8);  // 8px padding around edges
        vLayout->setSpacing(8);                   // Space between elements (layouts)
        ui->ros2_control->setLayout(vLayout);

        // ---------------------------------------------------------------------------- create 3 meters
        // --- Create a horizontal layout for the 3 meters ---
        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->setSpacing(16); 
        hLayout->setAlignment(Qt::AlignLeft);
        ros2ControlQmlView_.clear();

        QQuickWidget *speed_meter   = new QQuickWidget(ui->ros2_control);
        QQuickWidget *leftRpm_meter = new QQuickWidget(ui->ros2_control);
        QQuickWidget *rightRpm_meter= new QQuickWidget(ui->ros2_control);

        QVector<QQuickWidget*> meterWidgets = { speed_meter, leftRpm_meter, rightRpm_meter };

        for(int i = 0; i < meterWidgets.size(); ++i)
        {
            QQuickWidget *meter = meterWidgets[i];
            meter->setResizeMode(QQuickWidget::SizeRootObjectToView);

            // Set fixed height (e.g., 60% of container height)
            int meterHeight = static_cast<int>(ui->ros2_control->height() * 0.45);
            meter->setMinimumHeight(meterHeight);
            meter->setMaximumHeight(meterHeight);

            meter->setClearColor(QColor("#2e2e2e"));
            
            meter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            
            if (i == 0) {
                meter->setSource(QUrl(QStringLiteral("qrc:/Speed.qml")));
            } else if (i == 1) {
                meter->setSource(QUrl(QStringLiteral("qrc:/LeftSpeed.qml")));
            } else if (i == 2) {
                meter->setSource(QUrl(QStringLiteral("qrc:/RightSpeed.qml")));
            }

            if (meter->status() != QQuickWidget::Ready) {
                qWarning() << "Failed to load QML for meter" << i << ":" << meter->errors();
            }

            // Add to layout and store reference
            hLayout->addWidget(meter, 1);  // equal width
            ros2ControlQmlView_.append(meter);
        }

        // --- Add the horizontal layout to the vertical layout ---
        vLayout->addLayout(hLayout);
        vLayout->addStretch(1);

        // ---------------------------------------------------------------------------- next 3 meters
        // --- Create a horizontal layout for the Actual 3 meters ---
        QHBoxLayout *lower_hLayout = new QHBoxLayout();
        lower_hLayout->setSpacing(16); 
        
        //ros2ControlQmlView_.clear();

        QQuickWidget *actual_speed_meter   = new QQuickWidget(ui->ros2_control);
        QQuickWidget *actualLeftRpm_meter  = new QQuickWidget(ui->ros2_control);
        QQuickWidget *actualRightRpm_meter = new QQuickWidget(ui->ros2_control);

        QVector<QQuickWidget*> lower_meterWidgets = { actual_speed_meter, actualLeftRpm_meter, actualRightRpm_meter };

        for(int i = 0; i < lower_meterWidgets.size(); ++i)
        {
            QQuickWidget *meter = lower_meterWidgets[i];
            meter->setResizeMode(QQuickWidget::SizeRootObjectToView);

            // Set fixed height (e.g., 60% of container height)
            int meterHeight = static_cast<int>(ui->ros2_control->height() * 0.45);
            meter->setMinimumHeight(meterHeight);
            meter->setMaximumHeight(meterHeight);

            meter->setClearColor(QColor("#2e2e2e"));
            
            meter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            
            if (i == 0) {
                meter->setSource(QUrl(QStringLiteral("qrc:/ActualSpeed.qml")));
            } else if (i == 1) {
                meter->setSource(QUrl(QStringLiteral("qrc:/ActualLeftSpeed.qml")));
            } else if (i == 2) {
                meter->setSource(QUrl(QStringLiteral("qrc:/ActualRightSpeed.qml")));
            }

            if (meter->status() != QQuickWidget::Ready) {
                qWarning() << "Failed to load QML for meter" << i << ":" << meter->errors();
            }

            // Add to layout and store reference
            lower_hLayout->addWidget(meter, 1);  // equal width
            ros2ControlQmlView_.append(meter);
        }

        // --- Add the horizontal layout to the vertical layout ---
        vLayout->addLayout(lower_hLayout);
        vLayout->addStretch(1);
        
        ui->desireVelHzLabel->raise();ui->desireVelHzUnitLabel->raise();
        ui->desireLeftWheelRpmLabel->raise();ui->desireLeftWheelRpmUnitLabel->raise();
        ui->desireRightWheelRpmLabel->raise();ui->desireRightWheelRpmUnitLabel->raise();

        ui->actualVelHzLabel->raise();ui->actualVelHzUnitLabel->raise();
        ui->actualLeftWheelRpmLabel->raise();ui->actualLeftWheelRpmUnitLabel->raise();
        ui->actualRightWheelRpmLabel->raise();ui->actualRightWheelRpmUnitLabel->raise();
        
        
    }
}
void MainWindow::activateRos2ControlTab()
{
    if (!communication_) return;

    QString cmd_vel_topic_name = "/diff_controller/cmd_vel_unstamped";
    QString cmd_vel_msg_type   = "geometry_msgs/msg/Twist";
    //communication_->subscribeTopic(cmd_vel_topic_name, cmd_vel_msg_type);
    // ⭐ Thread-Safe Method: invokeMethod ကို အသုံးပြုခြင်း
    QMetaObject::invokeMethod(
        communication_, 
        "subscribeTopic", 
        Qt::QueuedConnection,
        Q_ARG(const QString&, cmd_vel_topic_name),
        Q_ARG(const QString&, cmd_vel_msg_type)
    );

    QString odom_topic_name = "/diff_controller/odom";
    QString odom_msg_type   = "nav_msgs/msg/Odometry";
    // communication_->subscribeTopic(odom_topic_name, odom_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_, 
        "subscribeTopic", 
        Qt::QueuedConnection,
        Q_ARG(const QString&, odom_topic_name),
        Q_ARG(const QString&, odom_msg_type)
    );

    QString js_topic_name = "/joint_states";
    QString js_msg_type   = "sensor_msgs/msg/JointState";
    // communication_->subscribeTopic(js_topic_name, js_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, js_topic_name),
        Q_ARG(const QString&, js_msg_type)
    );

    qDebug() << "Subscribed to " << cmd_vel_topic_name << "," << odom_topic_name << "," << js_topic_name;
}
void MainWindow::deactivateRos2ControlTab()
{
    if (!communication_) return;
    
    QString cmd_vel_topic_name = "/diff_controller/cmd_vel_unstamped";
    QString odom_topic_name = "/diff_controller/odom";
    QString js_topic_name = "/joint_states";

    // communication_->unsubscribeTopic(cmd_vel_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_, 
        "unsubscribeTopic", 
        Qt::QueuedConnection,
        Q_ARG(const QString&, cmd_vel_topic_name)
    );
    //communication_->unsubscribeTopic(odom_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, odom_topic_name)
    );

    //communication_->unsubscribeTopic(js_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, js_topic_name)
    );

    qDebug() << "Unsubscribed from " << cmd_vel_topic_name << "," << odom_topic_name << "," << js_topic_name;
}


void MainWindow::initEkfTab()
{
    if (ui->ekf)
    {
        qDebug() << " Initializing EKF Tab UI components ";
        QLayout *existing = ui->ekf->layout();
        if (existing) 
        {
            delete existing;
            qDebug() << " Deleted existing layout in EKF Tab ";
        }
        
        QGridLayout* grid = new QGridLayout(ui->ekf);
        grid->setContentsMargins(20, 20, 20, 20);
        grid->setSpacing(0);

        grid->setRowStretch(0, 1);
        grid->setRowStretch(1, 1);
        grid->setColumnStretch(0, 1);
        grid->setColumnStretch(1, 1);

        odomDiffOdomPositionGraphPtr_   = new RomPositionGraph(ui->ekf);
        ekfPositionCovarianceGraphPtr_  = new RomPositionCovarianceGraph(ui->ekf);

        odomDiffOdomImuHeadingGraphPtr_ = new RomPolarHeadingGraph(ui->ekf);
        ekfHeadingCovarianceGraphPtr_   = new RomYawCovarianceGraph(ui->ekf);

        grid->addWidget(odomDiffOdomPositionGraphPtr_, 0, 0);        
        grid->addWidget(ekfPositionCovarianceGraphPtr_, 0, 1);
        grid->addWidget(odomDiffOdomImuHeadingGraphPtr_, 1, 0);
        grid->addWidget(ekfHeadingCovarianceGraphPtr_, 1, 1);

    
        // Optionally, add more widgets or adjust grid layout as needed

        ui->ekf->setLayout(grid);
        
        //=================================
        ui->controllerLegendLabel->raise();
        ui->controllerPoseLegendLabel->raise();
        ui->ekfLegendLabel->raise();
        ui->ekfPoseLegendLabel->raise();
        ui->imuLegendLabel->raise();

        ui->processNoiseXBtn->raise();
        ui->processNoiseYBtn->raise();
        ui->processNoiseZBtn->raise();

        ui->processNoiseRollBtn->raise();
        ui->processNoisePitchBtn->raise();
        ui->processNoiseYawBtn->raise();

        ui->processNoiseVXBtn->raise();
        ui->processNoiseVYBtn->raise();
        ui->processNoiseVZBtn->raise();

        ui->processNoiseVRollBtn->raise();
        ui->processNoiseVPitchBtn->raise();
        ui->processNoiseVYawBtn->raise();

        ui->processNoiseAXBtn->raise();
        ui->processNoiseAYBtn->raise();
        ui->processNoiseAZBtn->raise();



        ui->initialNoiseXBtn->raise();
        ui->initialNoiseYBtn->raise();
        ui->initialNoiseZBtn->raise();

        ui->initialNoiseRollBtn->raise();
        ui->initialNoisePitchBtn->raise();
        ui->initialNoiseYawBtn->raise();

        ui->initialNoiseVXBtn->raise();
        ui->initialNoiseVYBtn->raise();
        ui->initialNoiseVZBtn->raise();

        ui->initialNoiseVRollBtn->raise();
        ui->initialNoiseVPitchBtn->raise();
        ui->initialNoiseVYawBtn->raise();

        ui->initialNoiseAXBtn->raise();
        ui->initialNoiseAYBtn->raise();
        ui->initialNoiseAZBtn->raise();

        ui->initialNoiseXBtn->setVisible(false);
        ui->initialNoiseYBtn->setVisible(false);
        ui->initialNoiseZBtn->setVisible(false);

        ui->initialNoiseRollBtn->setVisible(false);
        ui->initialNoisePitchBtn->setVisible(false);
        ui->initialNoiseYawBtn->setVisible(false);

        ui->initialNoiseVXBtn->setVisible(false);
        ui->initialNoiseVYBtn->setVisible(false);
        ui->initialNoiseVZBtn->setVisible(false);

        ui->initialNoiseVRollBtn->setVisible(false);
        ui->initialNoiseVPitchBtn->setVisible(false);
        ui->initialNoiseVYawBtn->setVisible(false);

        ui->initialNoiseAXBtn->setVisible(false);
        ui->initialNoiseAYBtn->setVisible(false);
        ui->initialNoiseAZBtn->setVisible(false);

        ui->processNoiseXBtn->setVisible(false);
        ui->processNoiseYBtn->setVisible(false);
        ui->processNoiseZBtn->setVisible(false);

        ui->processNoiseRollBtn->setVisible(false);
        ui->processNoisePitchBtn->setVisible(false);
        ui->processNoiseYawBtn->setVisible(false);

        ui->processNoiseVXBtn->setVisible(false);
        ui->processNoiseVYBtn->setVisible(false);
        ui->processNoiseVZBtn->setVisible(false);

        ui->processNoiseVRollBtn->setVisible(false);
        ui->processNoiseVPitchBtn->setVisible(false);
        ui->processNoiseVYawBtn->setVisible(false);

        ui->processNoiseAXBtn->setVisible(false);
        ui->processNoiseAYBtn->setVisible(false);
        ui->processNoiseAZBtn->setVisible(false);
        
    }
}
void MainWindow::activateEkfTab()
{
    if (!communication_) return;

    QString odom_topic_name = "/diff_controller/odom";
    QString odom_msg_type   = "nav_msgs/msg/Odometry";
    // communication_->subscribeTopic(odom_topic_name, odom_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_, 
        "subscribeTopic", 
        Qt::QueuedConnection,
        Q_ARG(const QString&, odom_topic_name),
        Q_ARG(const QString&, odom_msg_type)
    );

    QString ekf_odom_topic_name = "/odom";
    QString ekf_odom_msg_type   = "nav_msgs/msg/Odometry";
    // communication_->subscribeTopic(ekf_odom_topic_name, ekf_odom_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_, 
        "subscribeTopic", 
        Qt::QueuedConnection,
        Q_ARG(const QString&, ekf_odom_topic_name),
        Q_ARG(const QString&, ekf_odom_msg_type)
    );

    QString imu_topic_name = "/imu/out";
    QString imu_msg_type   = "sensor_msgs/msg/Imu";
    // communication_->subscribeTopic(imu_topic_name, imu_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, imu_topic_name),
        Q_ARG(const QString&, imu_msg_type)
    );

    qDebug() << "Subscribed to " << odom_topic_name << "," << ekf_odom_topic_name << "," << imu_topic_name;
}
void MainWindow::deactivateEkfTab()
{
    QString ekf_odom_topic_name = "/odom";
    QString odom_topic_name = "/diff_controller/odom";
    QString imu_topic_name = "/imu/out";
    // communication_->unsubscribeTopic(ekf_odom_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, ekf_odom_topic_name)
    );
    // communication_->unsubscribeTopic(odom_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_, 
        "unsubscribeTopic", 
        Qt::QueuedConnection,
        Q_ARG(const QString&, odom_topic_name)
    );
    // communication_->unsubscribeTopic(imu_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, imu_topic_name)
    );

    qDebug() << "Unsubscribed from " << odom_topic_name << "," << ekf_odom_topic_name << "," << imu_topic_name;
}

void MainWindow::initCartoTab()
{
    if (ui->carto)
    {
        qDebug() << " Initializing Carto Tab UI components ";

        // Clear any existing layout/content on the carto tab (not EKF)
        QLayout *existing = ui->carto->layout();
        if (existing) 
        {
            delete existing;
            qDebug() << " Deleted existing layout in Carto Tab ";
        }

    // ========================================        Create map widget on the carto page
        mapWidgetPtr_ = new CartoRomMapWidget(ui->carto);
        mapWidgetPtr_->setStyleSheet("background:#2a2f36; color:#d3d9e3; border-radius:6px; font-size:20px;");
        mapWidgetPtr_->show();

        // Set initial layout
        QVBoxLayout* mainPanelLayout = new QVBoxLayout(ui->carto);
        mainPanelLayout->setContentsMargins(0,0,0,0);
        mainPanelLayout->addWidget(mapWidgetPtr_);

        ui->carto->setLayout(mainPanelLayout);
    }
}
void MainWindow::activateCartoTab()
{
    if (!communication_) return;

    auto ns_prefix = robotNamespace_;
    if (!ns_prefix.isEmpty() && !ns_prefix.startsWith('/')) ns_prefix.prepend('/');
    QString map_topic_name = ns_prefix + "/map";
    QString map_msg_type   = "nav_msgs/msg/OccupancyGrid";
    // communication_->subscribeTopic(map_topic_name, map_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_, 
        "subscribeTopic", 
        Qt::QueuedConnection,
        Q_ARG(const QString&, map_topic_name),
        Q_ARG(const QString&, map_msg_type)
    );

    QString constraint_list_topic_name = ns_prefix + "/constraint_list";
    QString constraint_list_msg_type   = "visualization_msgs/msg/MarkerArray";
    // communication_->subscribeTopic(constraint_list_topic_name, constraint_list_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, constraint_list_topic_name),
        Q_ARG(const QString&, constraint_list_msg_type)
    );

    QString trajectory_node_list_topic_name = ns_prefix + "/trajectory_node_list";
    QString trajectory_node_list_msg_type   = "visualization_msgs/msg/MarkerArray";
    // communication_->subscribeTopic(trajectory_node_list_topic_name, trajectory_node_list_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, trajectory_node_list_topic_name),
        Q_ARG(const QString&, trajectory_node_list_msg_type)
    );
    QString robot_pose_topic_name = ns_prefix + "/map_bfp_publisher";
    QString robot_pose_msg_type   = "geometry_msgs/msg/Pose2D";
    // communication_->subscribeTopic(constraint_list_topic_name, constraint_list_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, robot_pose_topic_name),
        Q_ARG(const QString&, robot_pose_msg_type)
    );
    /*
    QString scan_matched_points_topic_name = "/scan_matched_points2";
    QString scan_matched_points_msg_type   = "sensor_msgs/msg/PointCloud2";
    // communication_->subscribeTopic(scan_matched_points_topic_name, scan_matched_points_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, scan_matched_points_topic_name),
        Q_ARG(const QString&, scan_matched_points_msg_type)
    );

    QString landmark_poses_list_topic_name = "/landmark_poses_list";
    QString landmark_poses_list_msg_type   = "visualization_msgs/msg/MarkerArray";
    // communication_->subscribeTopic(landmark_poses_list_topic_name, landmark_poses_list_msg_type);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, landmark_poses_list_topic_name),
        Q_ARG(const QString&, landmark_poses_list_msg_type)
    );

    qDebug() << "Subscribed to " << map_topic_name << "," 
    << constraint_list_topic_name << "," 
    << trajectory_node_list_topic_name << "," 
    << scan_matched_points_topic_name;
    */
    qDebug() << "Subscribed to " << map_topic_name << "," 
    << constraint_list_topic_name << "," 
    << trajectory_node_list_topic_name << ", " << robot_pose_topic_name;

}
void MainWindow::deactivateCartoTab()
{
    auto ns_prefix = robotNamespace_;
    if (!ns_prefix.isEmpty() && !ns_prefix.startsWith('/')) ns_prefix.prepend('/');
    QString map_topic_name = ns_prefix + "/map";
    QString constraint_list_topic_name = ns_prefix + "/constraint_list";
    QString trajectory_node_list_topic_name = ns_prefix + "/trajectory_node_list";
    QString scan_matched_points_topic_name = ns_prefix + "/scan_matched_points2";
    //QString landmark_poses_list_topic_name = "/landmark_poses_list";
    QString robot_pose_topic_name = ns_prefix + "/map_bfp_publisher";
    
    // communication_->unsubscribeTopic(map_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_, 
        "unsubscribeTopic", 
        Qt::QueuedConnection,
        Q_ARG(const QString&, map_topic_name)
    );
    // communication_->unsubscribeTopic(constraint_list_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, constraint_list_topic_name)
    );
    // communication_->unsubscribeTopic(trajectory_node_list_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, trajectory_node_list_topic_name)
    );
    // communication_->unsubscribeTopic(scan_matched_points_topic_name);
    // ⭐
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, scan_matched_points_topic_name)
    );
    // communication_->unsubscribeTopic(landmark_poses_list_topic_name);
    // communication_->unsubscribeTopic(robot_pose_topic_name);
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, robot_pose_topic_name)
    );


    qDebug() << "Unsubscribed from " << map_topic_name << "," << constraint_list_topic_name << "," << trajectory_node_list_topic_name << ", " << robot_pose_topic_name;

}

void MainWindow::initNav2_1Tab()
{
    if (ui->navi2_one)
    {
        qDebug() << "Initializing Nav2_1 Tab UI components";

        QLayout *existing = ui->navi2_one->layout();
        if (existing)
        {
            delete existing;
            qDebug() << "Deleted existing layout in Nav2_1 Tab";
        }

        // Create main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(ui->navi2_one);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        // Create title label
        QLabel *titleLabel = new QLabel("NavigateToPose Action Feedback", ui->navi2_one);
        titleLabel->setStyleSheet("font: 14pt 'SF Pro'; color: #03fc84; background: transparent;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Create scroll area
        nav2_1ScrollArea_ = new QScrollArea(ui->navi2_one);
        nav2_1ScrollArea_->setWidgetResizable(true);
        nav2_1ScrollArea_->setStyleSheet("QScrollArea { background: #2e2e2e; border: none; }");

        // Create widget to hold feedback labels
        nav2_1FeedbackWidget_ = new QWidget();
        nav2_1FeedbackWidget_->setStyleSheet("background: #2e2e2e;");
        
        nav2_1FeedbackLayout_ = new QVBoxLayout(nav2_1FeedbackWidget_);
        nav2_1FeedbackLayout_->setContentsMargins(10, 10, 10, 10);
        nav2_1FeedbackLayout_->setSpacing(8);
        nav2_1FeedbackLayout_->setAlignment(Qt::AlignTop);

        // Create null label (initially visible)
        nav2_1NullLabel_ = new QLabel("Null", nav2_1FeedbackWidget_);
        nav2_1NullLabel_->setStyleSheet("font: 20pt 'SF Pro'; color: rgb(246, 97, 81); background: transparent;");
        nav2_1NullLabel_->setAlignment(Qt::AlignCenter);
        nav2_1FeedbackLayout_->addWidget(nav2_1NullLabel_);

        nav2_1FeedbackWidget_->setLayout(nav2_1FeedbackLayout_);
        nav2_1ScrollArea_->setWidget(nav2_1FeedbackWidget_);
        
        mainLayout->addWidget(nav2_1ScrollArea_);
        ui->navi2_one->setLayout(mainLayout);

        // Create timer for feedback timeout detection
        nav2_1FeedbackTimer_ = new QTimer(this);
        nav2_1FeedbackTimer_->setInterval(3000); // 3 seconds
        nav2_1FeedbackTimer_->setSingleShot(false);
        connect(nav2_1FeedbackTimer_, &QTimer::timeout, this, [this]() {
            // Show null state when no feedback received
            if (nav2_1NullLabel_) {
                nav2_1NullLabel_->setVisible(true);
            }
            // Hide all feedback labels
            for (auto label : nav2_1FeedbackLabels_) {
                label->setVisible(false);
            }
        });
    }
}
void MainWindow::activateNav2_1Tab()
{
    if (!communication_) return;

    QString action_name = "/navigate_to_pose";
    QString action_type = "nav2_msgs/action/NavigateToPose";
    
    QMetaObject::invokeMethod(
        communication_,
        "subscribeActionFeedback",
        Qt::QueuedConnection,
        Q_ARG(const QString&, action_name),
        Q_ARG(const QString&, action_type)
    );

    // Start feedback timeout timer
    if (nav2_1FeedbackTimer_) {
        nav2_1FeedbackTimer_->start();
    }

    qDebug() << "Subscribed to NavigateToPose action feedback";
}
void MainWindow::deactivateNav2_1Tab()
{
    if (!communication_) return;

    QString action_name = "/navigate_to_pose";
    
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeActionFeedback",
        Qt::QueuedConnection,
        Q_ARG(const QString&, action_name)
    );

    // Stop feedback timeout timer
    if (nav2_1FeedbackTimer_) {
        nav2_1FeedbackTimer_->stop();
    }

    qDebug() << "Unsubscribed from NavigateToPose action feedback";
}

void MainWindow::initNav2_2Tab()
{
    if (ui->navi2_two)
    {
        qDebug() << "Initializing Nav2_2 Tab UI components";

        QLayout *existing = ui->navi2_two->layout();
        if (existing)
        {
            delete existing;
            qDebug() << "Deleted existing layout in Nav2_2 Tab";
        }

        // Create main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(ui->navi2_two);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        // Create title label
        QLabel *titleLabel = new QLabel("NavigateThroughPoses Action Feedback", ui->navi2_two);
        titleLabel->setStyleSheet("font: 14pt 'SF Pro'; color: #03fc84; background: transparent;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Create scroll area
        nav2_2ScrollArea_ = new QScrollArea(ui->navi2_two);
        nav2_2ScrollArea_->setWidgetResizable(true);
        nav2_2ScrollArea_->setStyleSheet("QScrollArea { background: #2e2e2e; border: none; }");

        // Create widget to hold feedback labels
        nav2_2FeedbackWidget_ = new QWidget();
        nav2_2FeedbackWidget_->setStyleSheet("background: #2e2e2e;");
        
        nav2_2FeedbackLayout_ = new QVBoxLayout(nav2_2FeedbackWidget_);
        nav2_2FeedbackLayout_->setContentsMargins(10, 10, 10, 10);
        nav2_2FeedbackLayout_->setSpacing(8);
        nav2_2FeedbackLayout_->setAlignment(Qt::AlignTop);

        // Create null label (initially visible)
        nav2_2NullLabel_ = new QLabel("Null", nav2_2FeedbackWidget_);
        nav2_2NullLabel_->setStyleSheet("font: 20pt 'SF Pro'; color: rgb(246, 97, 81); background: transparent;");
        nav2_2NullLabel_->setAlignment(Qt::AlignCenter);
        nav2_2FeedbackLayout_->addWidget(nav2_2NullLabel_);

        nav2_2FeedbackWidget_->setLayout(nav2_2FeedbackLayout_);
        nav2_2ScrollArea_->setWidget(nav2_2FeedbackWidget_);
        
        mainLayout->addWidget(nav2_2ScrollArea_);
        ui->navi2_two->setLayout(mainLayout);

        // Create timer for feedback timeout detection
        nav2_2FeedbackTimer_ = new QTimer(this);
        nav2_2FeedbackTimer_->setInterval(3000); // 3 seconds
        nav2_2FeedbackTimer_->setSingleShot(false);
        connect(nav2_2FeedbackTimer_, &QTimer::timeout, this, [this]() {
            // Show null state when no feedback received
            if (nav2_2NullLabel_) {
                nav2_2NullLabel_->setVisible(true);
            }
            // Hide all feedback labels
            for (auto label : nav2_2FeedbackLabels_) {
                label->setVisible(false);
            }
        });
    }
}
void MainWindow::activateNav2_2Tab()
{
    if (!communication_) return;

    QString action_name = "/navigate_through_poses";
    QString action_type = "nav2_msgs/action/NavigateThroughPoses";
    
    QMetaObject::invokeMethod(
        communication_,
        "subscribeActionFeedback",
        Qt::QueuedConnection,
        Q_ARG(const QString&, action_name),
        Q_ARG(const QString&, action_type)
    );

    // Start feedback timeout timer
    if (nav2_2FeedbackTimer_) {
        nav2_2FeedbackTimer_->start();
    }

    qDebug() << "Subscribed to NavigateThroughPoses action feedback";
}
void MainWindow::deactivateNav2_2Tab()
{
    if (!communication_) return;

    QString action_name = "/navigate_through_poses";
    
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeActionFeedback",
        Qt::QueuedConnection,
        Q_ARG(const QString&, action_name)
    );

    // Stop feedback timeout timer
    if (nav2_2FeedbackTimer_) {
        nav2_2FeedbackTimer_->stop();
    }

    // Clear all feedback labels
    for (auto label : nav2_2FeedbackLabels_) {
        label->setVisible(false);
    }

    // Show null state
    if (nav2_2NullLabel_) {
        nav2_2NullLabel_->setVisible(true);
    }

    // Clear behavior tree states
    nav2_2BehaviorTreeStates_.clear();

    qDebug() << "Unsubscribed from NavigateThroughPoses action feedback";
}

void MainWindow::initNav2_3Tab()
{
    if (ui->navi2_three)
    {
        qDebug() << "Initializing Nav2_3 Tab UI components";

        QLayout *existing = ui->navi2_three->layout();
        if (existing)
        {
            delete existing;
            qDebug() << "Deleted existing layout in Nav2_3 Tab";
        }

        // Create main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(ui->navi2_three);
        mainLayout->setContentsMargins(5, 5, 5, 5);
        mainLayout->setSpacing(5);

        // Create TF tree widget
        tfTreeWidget_ = new TFTreeWidget(ui->navi2_three);
        mainLayout->addWidget(tfTreeWidget_);

        ui->navi2_three->setLayout(mainLayout);
    }
}
void MainWindow::activateNav2_3Tab()
{
    if (!communication_) return;

    QString tf_topic_name = "/tf";
    QString tf_msg_type = "tf2_msgs/msg/TFMessage";
    
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, tf_topic_name),
        Q_ARG(const QString&, tf_msg_type)
    );

    qDebug() << "Subscribed to" << tf_topic_name;
}
void MainWindow::deactivateNav2_3Tab()
{
    if (!communication_) return;

    QString tf_topic_name = "/tf";
    
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, tf_topic_name)
    );

    // Clear TF tree visualization
    if (tfTreeWidget_) {
        tfTreeWidget_->clearTransforms();
    }

    qDebug() << "Unsubscribed from" << tf_topic_name;
}

void MainWindow::initBtTab()
{
    if (ui->bt)
    {
        qDebug() << "Initializing BT Tab UI components";

        QLayout *existing = ui->bt->layout();
        if (existing)
        {
            delete existing;
            qDebug() << "Deleted existing layout in BT Tab";
        }

        // Create main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(ui->bt);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        // Create title label
        QLabel *titleLabel = new QLabel("Behavior Tree Status", ui->bt);
        titleLabel->setStyleSheet("font: 14pt 'SF Pro'; color: #03fc84; background: transparent;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Create status label
        btStatusLabel_ = new QLabel("Waiting for behavior tree data...", ui->bt);
        btStatusLabel_->setStyleSheet("font: 11pt 'SF Pro'; color: rgb(246, 97, 81); background: transparent;");
        btStatusLabel_->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(btStatusLabel_);

        // Create scroll area for tree view
        btScrollArea_ = new QScrollArea(ui->bt);
        btScrollArea_->setWidgetResizable(true);
        btScrollArea_->setStyleSheet("QScrollArea { background: #2e2e2e; border: none; }");

        // Create text edit for tree visualization
        btTreeView_ = new QTextEdit();
        btTreeView_->setReadOnly(true);
        btTreeView_->setStyleSheet(
            "QTextEdit {"
            "  background: #2e2e2e;"
            "  color: #03fc84;"
            "  font: 10pt 'Courier New';"
            "  border: none;"
            "}"
        );
        btTreeView_->setPlainText("No behavior tree data received yet.");

        btScrollArea_->setWidget(btTreeView_);
        mainLayout->addWidget(btScrollArea_);

        ui->bt->setLayout(mainLayout);
    }
}
void MainWindow::activateBtTab()
{
    if (!communication_) return;

    QString bt_topic_name = "/behavior_tree";
    QString bt_msg_type = "std_msgs/msg/String";
    
    QMetaObject::invokeMethod(
        communication_,
        "subscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, bt_topic_name),
        Q_ARG(const QString&, bt_msg_type)
    );

    qDebug() << "Subscribed to" << bt_topic_name;

    // Update status
    if (btStatusLabel_) {
        btStatusLabel_->setText("Listening to /behavior_tree topic...");
        btStatusLabel_->setStyleSheet("font: 11pt 'SF Pro'; color: #03fc84; background: transparent;");
    }
}
void MainWindow::deactivateBtTab()
{
    if (!communication_) return;

    QString bt_topic_name = "/behavior_tree";
    
    QMetaObject::invokeMethod(
        communication_,
        "unsubscribeTopic",
        Qt::QueuedConnection,
        Q_ARG(const QString&, bt_topic_name)
    );

    qDebug() << "Unsubscribed from" << bt_topic_name;

    // Reset status
    if (btStatusLabel_) {
        btStatusLabel_->setText("Waiting for behavior tree data...");
        btStatusLabel_->setStyleSheet("font: 11pt 'SF Pro'; color: rgb(246, 97, 81); background: transparent;");
    }
}

void MainWindow::initTopicTab()
{
    // Map all node buttons by their node names
    topicTabNodeButtons_["/controller_manager"] = ui->controller_managerBtn;
    topicTabNodeButtons_["/diff_controller"] = ui->diff_controllerBtn;
    topicTabNodeButtons_["/gpio_controller"] = ui->gpio_controllerBtn;
    topicTabNodeButtons_["/robot_state_publisher"] = ui->robot_state_publisherBtn;
    topicTabNodeButtons_["/joint_broadcaster"] = ui->joint_broadcasterBtn;
    
    topicTabNodeButtons_["/static_tf_pub"] = ui->static_tf_pubBtn;
    topicTabNodeButtons_["/ltme_node"] = ui->ltme_nodeBtn;
    topicTabNodeButtons_["/imu"] = ui->imuBtn;
    topicTabNodeButtons_["/ekf_filter_node"] = ui->ekf_filter_nodeBtn;
    topicTabNodeButtons_["/cartographer_node"] = ui->cartographer_nodeBtn;
    topicTabNodeButtons_["/twist_mux"] = ui->twist_muxBtn;
    
    topicTabNodeButtons_["/behavior_server"] = ui->behavior_serverBtn;
    topicTabNodeButtons_["/bt_navigator"] = ui->bt_navigatorBtn;
    topicTabNodeButtons_["/bt_navigator_navigate_through_poses_rclcpp_node"] = ui->bt_navigator_navigate_through_poses_rclcpp_nodeBtn;
    
    topicTabNodeButtons_["/bt_navigator_navigate_to_pose_rclcpp_node"] = ui->bt_navigator_navigate_to_pose_rclcpp_nodeBtn;
    topicTabNodeButtons_["/bt_stop_client_node"] = ui->bt_stop_client_nodeBtn;
    topicTabNodeButtons_["/controller_server"] = ui->controller_serverBtn;
    
    topicTabNodeButtons_["/global_costmap/global_costmap"] = ui->global_costmapBtn;
    topicTabNodeButtons_["/local_costmap/local_costmap"] = ui->local_costmapBtn;
    topicTabNodeButtons_["/nav2_container"] = ui->nav2_containerBtn;
    topicTabNodeButtons_["/planner_server"] = ui->planner_serverBtn;
    
    topicTabNodeButtons_["/velocity_smoother"] = ui->velocity_smootherBtn;
    topicTabNodeButtons_["/waypoint_follower"] = ui->waypoint_followerBtn;
    topicTabNodeButtons_["/robot_pose_publisher"] = ui->robot_pose_publisherBtn;
    
    topicTabNodeButtons_["/smoother_server"] = ui->smoother_serverBtn;
    topicTabNodeButtons_["/map_server"] = ui->map_serverBtn;
    topicTabNodeButtons_["/lifecycle_manager_navigation"] = ui->lifecycle_manager_navigationBtn;
    topicTabNodeButtons_["/lifecycle_manager_localization"] = ui->lifecycle_manager_localizationBtn;
    
    topicTabNodeButtons_["/which_maps_server"] = ui->which_maps_serverBtn;
    topicTabNodeButtons_["/which_name_server"] = ui->which_name_serverBtn;
    topicTabNodeButtons_["/which_nav_switcher"] = ui->which_nav_switcherBtn;
    topicTabNodeButtons_["/map_base_footprint_pub"] = ui->map_base_footprint_pubBtn;
    
    topicTabNodeButtons_["/which_vel_server"] = ui->which_vel_serverBtn;
    topicTabNodeButtons_["/launch_ros_5050"] = ui->launch_ros_5050Btn;
    topicTabNodeButtons_["/rosapi"] = ui->rosapiBtn;
    topicTabNodeButtons_["/rosbridge_websocket"] = ui->rosbridge_websocketBtn;
    topicTabNodeButtons_["/construct_bt_xml_server"] = ui->construct_bt_xml_serverBtn;
    
    // Connect all button click events
    for (auto it = topicTabNodeButtons_.begin(); it != topicTabNodeButtons_.end(); ++it) {
        QPushButton *btn = it.value();
        if (btn) {
            connect(btn, &QPushButton::clicked, this, &MainWindow::onNodeButtonClicked);
        }
    }
    
    // Create timer for periodic checking
    topicTabCheckTimer_ = new QTimer(this);
    connect(topicTabCheckTimer_, &QTimer::timeout, this, [this]() {
        if (!communication_) return;
        
        auto ns_prefix = robotNamespace_;
        if (!ns_prefix.isEmpty() && !ns_prefix.startsWith('/')) ns_prefix.prepend('/');
        
        QString service_name = ns_prefix + "/rosapi/nodes";
        QString service_type = "rosapi_msgs/srv/Nodes";
        QString service_id = "topic_tab_node_check";
        
        QMetaObject::invokeMethod(
            communication_,
            "callService",
            Qt::QueuedConnection,
            Q_ARG(const QString&, service_name),
            Q_ARG(const QString&, service_id),
            Q_ARG(const QString&, service_type)
        );
    });
}
void MainWindow::activateTopicTab()
{
    if (!communication_) return;

    // Start periodic node checking (every 2 seconds)
    if (topicTabCheckTimer_) {
        topicTabCheckTimer_->start(2000);
        // Trigger immediate check by calling the lambda directly
        QMetaObject::invokeMethod(
            communication_,
            [this]() {
                auto ns_prefix = robotNamespace_;
                if (!ns_prefix.isEmpty() && !ns_prefix.startsWith('/')) ns_prefix.prepend('/');
                
                QString service_name = ns_prefix + "/rosapi/nodes";
                QString service_type = "rosapi_msgs/srv/Nodes";
                QString service_id = "topic_tab_node_check";
                
                QMetaObject::invokeMethod(
                    communication_,
                    "callService",
                    Qt::QueuedConnection,
                    Q_ARG(const QString&, service_name),
                    Q_ARG(const QString&, service_id),
                    Q_ARG(const QString&, service_type)
                );
            },
            Qt::QueuedConnection
        );
    }
}
void MainWindow::deactivateTopicTab()
{
    // Stop periodic checking
    if (topicTabCheckTimer_) {
        topicTabCheckTimer_->stop();
    }
}

void MainWindow::onNodeButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn || !communication_) return;
    
    // Find the node name for this button
    QString node_name;
    for (auto it = topicTabNodeButtons_.begin(); it != topicTabNodeButtons_.end(); ++it) {
        if (it.value() == btn) {
            node_name = it.key();
            break;
        }
    }
    
    if (node_name.isEmpty()) return;
    
    // Check if node is active
    if (!topicTabActiveNodes_.contains(node_name)) {
        QMessageBox::warning(this, "Node Inactive", 
            QString("Node '%1' is not currently running.").arg(node_name));
        return;
    }
    
    // Request node info from rosapi/node_details service
    auto ns_prefix = robotNamespace_;
    if (!ns_prefix.isEmpty() && !ns_prefix.startsWith('/')) ns_prefix.prepend('/');
    
    QString service_name = "/rosapi/node_details";
    QString service_type = "rosapi_msgs/srv/NodeDetails";
    QString service_id = "node_info_" + node_name;
    QString full_node_name = ns_prefix + node_name;
    
    // Build service arguments
    QJsonObject args;
    args["node"] = full_node_name;
    
    QMetaObject::invokeMethod(
        communication_,
        "callService",
        Qt::QueuedConnection,
        Q_ARG(const QString&, service_name),
        Q_ARG(const QString&, service_id),
        Q_ARG(const QString&, service_type),
        Q_ARG(const QJsonObject&, args)
    );
    
    qDebug() << "Requested node info for:" << full_node_name;
}

void MainWindow::onLogServiceButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString service = btn->property("service").toString();
    QString action = btn->property("action").toString();

    if (service.isEmpty() || action.isEmpty()) {
        qDebug() << "Invalid button properties";
        return;
    }

    // Get connection parameters
    QString ip = ui->ipLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text().trimmed();
    
    if (ip.isEmpty()) {
        QMessageBox::warning(this, "Connection Error", "Please enter robot IP address first.");
        return;
    }
    
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Connection Error", "Please enter robot password first.");
        return;
    }

    QString command;
    QString fullServiceName = service + ".service";

    if (action == "status") {
        command = QString("systemctl status %1").arg(fullServiceName);
    } else if (action == "enable") {
        command = QString("echo '%1' | sudo -S systemctl enable %2").arg(password, fullServiceName);
    } else if (action == "disable") {
        command = QString("echo '%1' | sudo -S systemctl disable %2").arg(password, fullServiceName);
    } else if (action == "start") {
        command = QString("echo '%1' | sudo -S systemctl start %2").arg(password, fullServiceName);
    } else if (action == "stop") {
        command = QString("echo '%1' | sudo -S systemctl stop %2").arg(password, fullServiceName);
    } else if (action == "logs") {
        command = QString("journalctl -f -u %1").arg(fullServiceName);
    }

    qDebug() << "Opening SSH terminal for command:" << command;

    // Build terminal command with sshpass for automatic authentication
    QString terminalCommand = QString(
        "gnome-terminal -- bash -c \""
        "echo 'Connecting to %1...'; "
        "sshpass -p '%2' ssh -o StrictHostKeyChecking=no mr_robot@%1 '%3'; "
        "echo ''; echo 'Press Enter to close...'; read\""
    ).arg(ip, password, command);

    // Execute in background
    QProcess::startDetached("bash", QStringList() << "-c" << terminalCommand);
}

void MainWindow::initLogTab()
{
    if (ui->log)
    {
        qDebug() << "Initializing Log Tab UI components";

        QLayout *existing = ui->log->layout();
        if (existing)
        {
            delete existing;
            qDebug() << "Deleted existing layout in Log Tab";
        }

        // Create main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(ui->log);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        // Create title label
        QLabel *titleLabel = new QLabel("System Service Management", ui->log);
        titleLabel->setStyleSheet("font: 14pt 'SF Pro'; color: #03fc84; background: transparent;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Create scroll area for service controls
        QScrollArea *scrollArea = new QScrollArea(ui->log);
        scrollArea->setWidgetResizable(true);
        scrollArea->setStyleSheet("QScrollArea { background: #2e2e2e; border: none; }");

        QWidget *scrollWidget = new QWidget();
        QGridLayout *gridLayout = new QGridLayout(scrollWidget);
        gridLayout->setSpacing(5);
        gridLayout->setContentsMargins(5, 5, 5, 5);

        // Service list
        QStringList services = {
            "rom_ros2_control",
            "rom_efk",
            "rom_which_maps_carto_server",
            "rom_which_nav_server",
            "rom_which_vel_server",
            "rom_which_name_server",
            "rom_waypoints_provider",
            "rom_obstacles_provider",
            "rom_rosbridge_websocket",
            "rom_map_bfp",
            "rom_which_tasks_server"
        };

        // Header row
        QLabel *serviceHeader = new QLabel("Service Name");
        serviceHeader->setStyleSheet("font: bold 10pt 'SF Pro'; color: #03fc84; background: transparent;");
        gridLayout->addWidget(serviceHeader, 0, 0);

        QStringList headers = {"Status", "Enable", "Disable", "Start", "Stop", "Logs"};
        for (int col = 0; col < headers.size(); ++col)
        {
            QLabel *header = new QLabel(headers[col]);
            header->setStyleSheet("font: bold 10pt 'SF Pro'; color: #03fc84; background: transparent;");
            header->setAlignment(Qt::AlignCenter);
            gridLayout->addWidget(header, 0, col + 1);
        }

        // Button style
        QString buttonStyle = 
            "QPushButton {"
            "  background: #3e3e3e;"
            "  color: #03fc84;"
            "  border: 1px solid #555;"
            "  border-radius: 4px;"
            "  padding: 5px 10px;"
            "  font: 9pt 'SF Pro';"
            "  min-width: 60px;"
            "}"
            "QPushButton:hover {"
            "  background: #4e4e4e;"
            "  border: 1px solid #03fc84;"
            "}"
            "QPushButton:pressed {"
            "  background: #2e2e2e;"
            "}";

        // Create buttons for each service
        int row = 1;
        for (const QString &service : services)
        {
            // Service name label
            QLabel *serviceLabel = new QLabel(service);
            serviceLabel->setStyleSheet("font: 10pt 'SF Pro'; color: white; background: transparent;");
            gridLayout->addWidget(serviceLabel, row, 0);

            // Status button
            QPushButton *statusBtn = new QPushButton("Status");
            statusBtn->setStyleSheet(buttonStyle);
            statusBtn->setProperty("service", service);
            statusBtn->setProperty("action", "status");
            connect(statusBtn, &QPushButton::clicked, this, &MainWindow::onLogServiceButtonClicked);
            gridLayout->addWidget(statusBtn, row, 1);
            logStatusButtons_[service] = statusBtn;

            // Enable button
            QPushButton *enableBtn = new QPushButton("Enable");
            enableBtn->setStyleSheet(buttonStyle);
            enableBtn->setProperty("service", service);
            enableBtn->setProperty("action", "enable");
            connect(enableBtn, &QPushButton::clicked, this, &MainWindow::onLogServiceButtonClicked);
            gridLayout->addWidget(enableBtn, row, 2);
            logEnableButtons_[service] = enableBtn;

            // Disable button
            QPushButton *disableBtn = new QPushButton("Disable");
            disableBtn->setStyleSheet(buttonStyle);
            disableBtn->setProperty("service", service);
            disableBtn->setProperty("action", "disable");
            connect(disableBtn, &QPushButton::clicked, this, &MainWindow::onLogServiceButtonClicked);
            gridLayout->addWidget(disableBtn, row, 3);
            logDisableButtons_[service] = disableBtn;

            // Start button
            QPushButton *startBtn = new QPushButton("Start");
            startBtn->setStyleSheet(buttonStyle);
            startBtn->setProperty("service", service);
            startBtn->setProperty("action", "start");
            connect(startBtn, &QPushButton::clicked, this, &MainWindow::onLogServiceButtonClicked);
            gridLayout->addWidget(startBtn, row, 4);
            logStartButtons_[service] = startBtn;

            // Stop button
            QPushButton *stopBtn = new QPushButton("Stop");
            stopBtn->setStyleSheet(buttonStyle);
            stopBtn->setProperty("service", service);
            stopBtn->setProperty("action", "stop");
            connect(stopBtn, &QPushButton::clicked, this, &MainWindow::onLogServiceButtonClicked);
            gridLayout->addWidget(stopBtn, row, 5);
            logStopButtons_[service] = stopBtn;

            // Logs button
            QPushButton *logsBtn = new QPushButton("Logs");
            logsBtn->setStyleSheet(buttonStyle);
            logsBtn->setProperty("service", service);
            logsBtn->setProperty("action", "logs");
            connect(logsBtn, &QPushButton::clicked, this, &MainWindow::onLogServiceButtonClicked);
            gridLayout->addWidget(logsBtn, row, 6);
            logLogsButtons_[service] = logsBtn;

            row++;
        }

        scrollWidget->setLayout(gridLayout);
        scrollArea->setWidget(scrollWidget);
        mainLayout->addWidget(scrollArea);

        // Create info label
        QLabel *infoLabel = new QLabel("Click any button to open SSH terminal and execute command on robot.");
        infoLabel->setStyleSheet("font: 10pt 'SF Pro'; color: rgb(246, 97, 81); background: transparent; padding: 10px;");
        infoLabel->setAlignment(Qt::AlignCenter);
        infoLabel->setWordWrap(true);
        mainLayout->addWidget(infoLabel);

        ui->log->setLayout(mainLayout);
    }
}
void MainWindow::activateLogTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;

}
void MainWindow::deactivateLogTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;

}


void MainWindow::initGoalTab()
{
    if (ui->goal)
    {
        qDebug() << "Initializing Goal Tab UI components";

        QLayout *existing = ui->goal->layout();
        if (existing)
        {
            delete existing;
            qDebug() << "Deleted existing layout in Goal Tab";
        }

        // Create main layout
        QHBoxLayout *mainLayout = new QHBoxLayout(ui->goal);
        mainLayout->setContentsMargins(10, 10, 10, 10);
        mainLayout->setSpacing(10);

        // ============ LEFT SIDE: NavigateToPose ============
        QWidget *leftWidget = new QWidget();
        QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
        leftLayout->setContentsMargins(10, 10, 10, 10);
        leftLayout->setSpacing(15);

        // Left title
        QLabel *leftTitle = new QLabel("NavigateToPose");
        leftTitle->setStyleSheet("font: bold 14pt 'SF Pro'; color: #03fc84; background: transparent;");
        leftTitle->setAlignment(Qt::AlignCenter);
        leftLayout->addWidget(leftTitle);

        // X input
        QLabel *xLabel = new QLabel("X Position:");
        xLabel->setStyleSheet("font: 11pt 'SF Pro'; color: white; background: transparent;");
        leftLayout->addWidget(xLabel);

        QComboBox *xCombo = new QComboBox();
        xCombo->setStyleSheet(
            "QComboBox {"
            "  background: #3e3e3e;"
            "  color: white;"
            "  border: 1px solid #555;"
            "  border-radius: 4px;"
            "  padding: 5px;"
            "  font: 10pt 'SF Pro';"
            "}"
            "QComboBox:hover {"
            "  border: 1px solid #03fc84;"
            "}"
            "QComboBox::drop-down {"
            "  border: none;"
            "}"
            "QComboBox QAbstractItemView {"
            "  background: #3e3e3e;"
            "  color: white;"
            "  selection-background-color: #03fc84;"
            "  selection-color: black;"
            "}"
        );
        for (int i = -5; i <= 5; ++i) {
            xCombo->addItem(QString::number(i));
        }
        xCombo->setCurrentText("0");
        leftLayout->addWidget(xCombo);
        goalToPoseX_ = xCombo;  // Store pointer

        // Y input
        QLabel *yLabel = new QLabel("Y Position:");
        yLabel->setStyleSheet("font: 11pt 'SF Pro'; color: white; background: transparent;");
        leftLayout->addWidget(yLabel);

        QComboBox *yCombo = new QComboBox();
        yCombo->setStyleSheet(
            "QComboBox {"
            "  background: #3e3e3e;"
            "  color: white;"
            "  border: 1px solid #555;"
            "  border-radius: 4px;"
            "  padding: 5px;"
            "  font: 10pt 'SF Pro';"
            "}"
            "QComboBox:hover {"
            "  border: 1px solid #03fc84;"
            "}"
            "QComboBox::drop-down {"
            "  border: none;"
            "}"
            "QComboBox QAbstractItemView {"
            "  background: #3e3e3e;"
            "  color: white;"
            "  selection-background-color: #03fc84;"
            "  selection-color: black;"
            "}"
        );
        for (int i = -5; i <= 5; ++i) {
            yCombo->addItem(QString::number(i));
        }
        yCombo->setCurrentText("0");
        leftLayout->addWidget(yCombo);
        goalToPoseY_ = yCombo;  // Store pointer

        // Theta input
        QLabel *thetaLabel = new QLabel("Theta (degrees):");
        thetaLabel->setStyleSheet("font: 11pt 'SF Pro'; color: white; background: transparent;");
        leftLayout->addWidget(thetaLabel);

        QComboBox *thetaCombo = new QComboBox();
        thetaCombo->setStyleSheet(
            "QComboBox {"
            "  background: #3e3e3e;"
            "  color: white;"
            "  border: 1px solid #555;"
            "  border-radius: 4px;"
            "  padding: 5px;"
            "  font: 10pt 'SF Pro';"
            "}"
            "QComboBox:hover {"
            "  border: 1px solid #03fc84;"
            "}"
            "QComboBox::drop-down {"
            "  border: none;"
            "}"
            "QComboBox QAbstractItemView {"
            "  background: #3e3e3e;"
            "  color: white;"
            "  selection-background-color: #03fc84;"
            "  selection-color: black;"
            "}"
        );
        for (int i = -180; i <= 180; i += 15) {
            thetaCombo->addItem(QString::number(i));
        }
        thetaCombo->setCurrentText("0");
        leftLayout->addWidget(thetaCombo);
        goalToPoseTheta_ = thetaCombo;  // Store pointer

        leftLayout->addStretch();

        // NavigateToPose button
        QPushButton *navigateToPoseBtn = new QPushButton("Navigate To Pose");
        navigateToPoseBtn->setStyleSheet(
            "QPushButton {"
            "  background: #03fc84;"
            "  color: black;"
            "  border: none;"
            "  border-radius: 6px;"
            "  padding: 12px;"
            "  font: bold 12pt 'SF Pro';"
            "  min-height: 40px;"
            "}"
            "QPushButton:hover {"
            "  background: #02dd75;"
            "}"
            "QPushButton:pressed {"
            "  background: #01cc66;"
            "}"
        );
        leftLayout->addWidget(navigateToPoseBtn);
        navigateToPoseBtn_ = navigateToPoseBtn;  // Store pointer
        connect(navigateToPoseBtn, &QPushButton::clicked, this, &MainWindow::onNavigateToPoseClicked);

        leftWidget->setLayout(leftLayout);
        leftWidget->setStyleSheet("QWidget { background: #2e2e2e; border-radius: 8px; }");

        // ============ RIGHT SIDE: NavigateThroughPoses ============
        QWidget *rightWidget = new QWidget();
        QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
        rightLayout->setContentsMargins(10, 10, 10, 10);
        rightLayout->setSpacing(15);

        // Right title
        QLabel *rightTitle = new QLabel("NavigateThroughPoses");
        rightTitle->setStyleSheet("font: bold 14pt 'SF Pro'; color: #03fc84; background: transparent;");
        rightTitle->setAlignment(Qt::AlignCenter);
        rightLayout->addWidget(rightTitle);

        // Create scroll area for poses
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

        QWidget *scrollWidget = new QWidget();
        QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
        scrollLayout->setSpacing(10);

        // Create 4 pose inputs
        for (int poseIdx = 0; poseIdx < 4; ++poseIdx)
        {
            QGroupBox *poseGroup = new QGroupBox(QString("Pose %1").arg(poseIdx + 1));
            poseGroup->setStyleSheet(
                "QGroupBox {"
                "  color: #03fc84;"
                "  border: 1px solid #555;"
                "  border-radius: 6px;"
                "  margin-top: 10px;"
                "  font: bold 11pt 'SF Pro';"
                "  background: transparent;"
                "}"
                "QGroupBox::title {"
                "  subcontrol-origin: margin;"
                "  left: 10px;"
                "  padding: 0 5px;"
                "}"
            );

            QVBoxLayout *poseLayout = new QVBoxLayout();
            poseLayout->setSpacing(8);

            // X
            QLabel *xLbl = new QLabel("X:");
            xLbl->setStyleSheet("font: 10pt 'SF Pro'; color: white; background: transparent;");
            poseLayout->addWidget(xLbl);

            QComboBox *xCmb = new QComboBox();
            xCmb->setStyleSheet(
                "QComboBox {"
                "  background: #3e3e3e;"
                "  color: white;"
                "  border: 1px solid #555;"
                "  border-radius: 4px;"
                "  padding: 5px;"
                "  font: 10pt 'SF Pro';"
                "}"
                "QComboBox QAbstractItemView {"
                "  background: #3e3e3e;"
                "  color: white;"
                "  selection-background-color: #03fc84;"
                "  selection-color: black;"
                "}"
            );
            for (int i = -5; i <= 5; ++i) {
                xCmb->addItem(QString::number(i));
            }
            xCmb->setCurrentText("0");
            poseLayout->addWidget(xCmb);
            goalThroughPosesX_[poseIdx] = xCmb;  // Store pointer

            // Y
            QLabel *yLbl = new QLabel("Y:");
            yLbl->setStyleSheet("font: 10pt 'SF Pro'; color: white; background: transparent;");
            poseLayout->addWidget(yLbl);

            QComboBox *yCmb = new QComboBox();
            yCmb->setStyleSheet(
                "QComboBox {"
                "  background: #3e3e3e;"
                "  color: white;"
                "  border: 1px solid #555;"
                "  border-radius: 4px;"
                "  padding: 5px;"
                "  font: 10pt 'SF Pro';"
                "}"
                "QComboBox QAbstractItemView {"
                "  background: #3e3e3e;"
                "  color: white;"
                "  selection-background-color: #03fc84;"
                "  selection-color: black;"
                "}"
            );
            for (int i = -5; i <= 5; ++i) {
                yCmb->addItem(QString::number(i));
            }
            yCmb->setCurrentText("0");
            poseLayout->addWidget(yCmb);
            goalThroughPosesY_[poseIdx] = yCmb;  // Store pointer

            // Theta
            QLabel *thetaLbl = new QLabel("Theta:");
            thetaLbl->setStyleSheet("font: 10pt 'SF Pro'; color: white; background: transparent;");
            poseLayout->addWidget(thetaLbl);

            QComboBox *thetaCmb = new QComboBox();
            thetaCmb->setStyleSheet(
                "QComboBox {"
                "  background: #3e3e3e;"
                "  color: white;"
                "  border: 1px solid #555;"
                "  border-radius: 4px;"
                "  padding: 5px;"
                "  font: 10pt 'SF Pro';"
                "}"
                "QComboBox QAbstractItemView {"
                "  background: #3e3e3e;"
                "  color: white;"
                "  selection-background-color: #03fc84;"
                "  selection-color: black;"
                "}"
            );
            for (int i = -180; i <= 180; i += 15) {
                thetaCmb->addItem(QString::number(i));
            }
            thetaCmb->setCurrentText("0");
            poseLayout->addWidget(thetaCmb);
            goalThroughPosesTheta_[poseIdx] = thetaCmb;  // Store pointer

            poseGroup->setLayout(poseLayout);
            scrollLayout->addWidget(poseGroup);
        }

        scrollWidget->setLayout(scrollLayout);
        scrollArea->setWidget(scrollWidget);
        rightLayout->addWidget(scrollArea);

        // NavigateThroughPoses button
        QPushButton *navigateThroughPosesBtn = new QPushButton("Navigate Through Poses");
        navigateThroughPosesBtn->setStyleSheet(
            "QPushButton {"
            "  background: #03fc84;"
            "  color: black;"
            "  border: none;"
            "  border-radius: 6px;"
            "  padding: 12px;"
            "  font: bold 12pt 'SF Pro';"
            "  min-height: 40px;"
            "}"
            "QPushButton:hover {"
            "  background: #02dd75;"
            "}"
            "QPushButton:pressed {"
            "  background: #01cc66;"
            "}"
        );
        rightLayout->addWidget(navigateThroughPosesBtn);
        navigateThroughPosesBtn_ = navigateThroughPosesBtn;  // Store pointer
        connect(navigateThroughPosesBtn, &QPushButton::clicked, this, &MainWindow::onNavigateThroughPosesClicked);

        rightWidget->setLayout(rightLayout);
        rightWidget->setStyleSheet("QWidget { background: #2e2e2e; border-radius: 8px; }");

        // Add both sides to main layout
        mainLayout->addWidget(leftWidget);
        mainLayout->addWidget(rightWidget);

        ui->goal->setLayout(mainLayout);

        qDebug() << "Goal tab UI created successfully";
    }
}

void MainWindow::activateGoalTab()
{
    // TODO: Subscribe to goal-related topics or services
    qDebug() << "Goal tab activated";
}

void MainWindow::deactivateGoalTab()
{
    // TODO: Unsubscribe from goal-related topics or services
    qDebug() << "Goal tab deactivated";
}

void MainWindow::onNavigateToPoseClicked()
{
    if (!communication_) {
        QMessageBox::warning(this, "Not Connected", "Please connect to robot first.");
        return;
    }

    // Get values from combo boxes
    double x = goalToPoseX_->currentText().toDouble();
    double y = goalToPoseY_->currentText().toDouble();
    double theta_deg = goalToPoseTheta_->currentText().toDouble();
    double theta_rad = theta_deg * M_PI / 180.0;

    qDebug() << "Sending NavigateToPose goal: x=" << x << ", y=" << y << ", theta=" << theta_deg << "deg";

    // Build goal message
    QJsonObject pose;
    
    // Position
    QJsonObject position;
    position["x"] = x;
    position["y"] = y;
    position["z"] = 0.0;
    pose["position"] = position;

    // Orientation (convert theta to quaternion)
    QJsonObject orientation;
    double half_theta = theta_rad / 2.0;
    orientation["x"] = 0.0;
    orientation["y"] = 0.0;
    orientation["z"] = sin(half_theta);
    orientation["w"] = cos(half_theta);
    pose["orientation"] = orientation;

    // Frame ID
    QJsonObject header;
    header["frame_id"] = "map";
    
    QJsonObject poseStamped;
    poseStamped["header"] = header;
    poseStamped["pose"] = pose;

    // Build goal
    QJsonObject goal;
    goal["pose"] = poseStamped;

    // Send action goal
    QString action_name = "/navigate_to_pose";
    QString action_type = "nav2_msgs/action/NavigateToPose";
    QString goal_id = "nav_to_pose_" + QString::number(QDateTime::currentMSecsSinceEpoch());

    QMetaObject::invokeMethod(
        communication_,
        "sendActionGoal",
        Qt::QueuedConnection,
        Q_ARG(const QString&, action_name),
        Q_ARG(const QString&, goal_id),
        Q_ARG(const QString&, action_type),
        Q_ARG(const QJsonObject&, goal)
    );

    qDebug() << "NavigateToPose goal sent with ID:" << goal_id;
    
    // Show confirmation
    if (btStatusLabel_) {
        btStatusLabel_->setText(QString("Goal sent: (%1, %2, %3°)").arg(x).arg(y).arg(theta_deg));
    }
}

void MainWindow::onNavigateThroughPosesClicked()
{
    if (!communication_) {
        QMessageBox::warning(this, "Not Connected", "Please connect to robot first.");
        return;
    }

    qDebug() << "Sending NavigateThroughPoses goal with 4 poses";

    // Build poses array
    QJsonArray poses;
    
    for (int i = 0; i < 4; ++i)
    {
        double x = goalThroughPosesX_[i]->currentText().toDouble();
        double y = goalThroughPosesY_[i]->currentText().toDouble();
        double theta_deg = goalThroughPosesTheta_[i]->currentText().toDouble();
        double theta_rad = theta_deg * M_PI / 180.0;

        qDebug() << "  Pose" << (i+1) << ": x=" << x << ", y=" << y << ", theta=" << theta_deg << "deg";

        // Position
        QJsonObject position;
        position["x"] = x;
        position["y"] = y;
        position["z"] = 0.0;

        // Orientation (convert theta to quaternion)
        QJsonObject orientation;
        double half_theta = theta_rad / 2.0;
        orientation["x"] = 0.0;
        orientation["y"] = 0.0;
        orientation["z"] = sin(half_theta);
        orientation["w"] = cos(half_theta);

        // Pose
        QJsonObject pose;
        pose["position"] = position;
        pose["orientation"] = orientation;

        // PoseStamped
        QJsonObject header;
        header["frame_id"] = "map";
        
        QJsonObject poseStamped;
        poseStamped["header"] = header;
        poseStamped["pose"] = pose;

        poses.append(poseStamped);
    }

    // Build goal
    QJsonObject goal;
    goal["poses"] = poses;

    // Send action goal
    QString action_name = "/navigate_through_poses";
    QString action_type = "nav2_msgs/action/NavigateThroughPoses";
    QString goal_id = "nav_through_poses_" + QString::number(QDateTime::currentMSecsSinceEpoch());

    QMetaObject::invokeMethod(
        communication_,
        "sendActionGoal",
        Qt::QueuedConnection,
        Q_ARG(const QString&, action_name),
        Q_ARG(const QString&, goal_id),
        Q_ARG(const QString&, action_type),
        Q_ARG(const QJsonObject&, goal)
    );

    qDebug() << "NavigateThroughPoses goal sent with ID:" << goal_id;
    
    // Show confirmation
    if (btStatusLabel_) {
        btStatusLabel_->setText("NavigateThroughPoses goal sent with 4 waypoints");
    }
}


void MainWindow::onReceivedTopicMessage(const QString &topic, const QJsonObject &msg)
{
    /* ROS2 CONTROL TAB */
    if( currentMode == Mode::ros2_control )
    {
       // topic name က /diff_controller/cmd_vel_unstamped 
        if( topic == "/diff_controller/cmd_vel_unstamped" )
        {
            if (ros2ControlQmlView_.size() != 6) return;

            QObject* root = ros2ControlQmlView_[0] ? ros2ControlQmlView_[0]->rootObject() : nullptr;
            
            if ( msg.isEmpty() || !msg.contains("linear") ) 
            {
                if (root) 
                {
                    root->setProperty("speed", 0.0);
                }
                return;
            }

            QJsonObject linear = msg.value("linear").toObject();
            double vx = linear.value("x").toDouble();
            //double vy = linear.value("y").toDouble();

            if ( vx < 0 ) { vx *= -1; }

            double speed = vx;
            if (speed > 1.0) { speed = 1.0; } // cap at 1.0 m/s
            
            speed = speed * 100.0; // convert to m/s for display


            if (root) 
            {
                QVariant qmlSpeed = QVariant::fromValue(speed);
                root->setProperty("speed", qmlSpeed);
            }

            int right_rpm = 0;
            int left_rpm  = 0;

            QJsonObject angular = msg.value("angular").toObject();
            double vz = angular.value("z").toDouble();

            robotVelocityToWheelRpms(vx, vz, wheel_radius_, wheel_seperation_, left_rpm, right_rpm);

            if( right_rpm < 0 ) { right_rpm *= -1; }
            if( left_rpm  < 0 ) { left_rpm  *= -1; }
        
            // left rpm
            QObject* left_root = ros2ControlQmlView_[1] ? ros2ControlQmlView_[1]->rootObject() : nullptr;
            if (left_root)
            {
                QVariant qmlLeftRpm = QVariant::fromValue(left_rpm);
                left_root->setProperty("speed", qmlLeftRpm);
            }
            // right rpm
            QObject* right_root = ros2ControlQmlView_[2] ? ros2ControlQmlView_[2]->rootObject() : nullptr;
            if (right_root)
            {
                QVariant qmlRightRpm = QVariant::fromValue(right_rpm);
                right_root->setProperty("speed", qmlRightRpm);
            }

        }
        // topic name က /diff_controller/odom for Actual Robot Velocity
        else if( topic == "/diff_controller/odom" )
        {
            if (ros2ControlQmlView_.size() != 6) return;

            QObject* root = ros2ControlQmlView_[3] ? ros2ControlQmlView_[3]->rootObject() : nullptr;
            
            if ( msg.isEmpty() || !msg.contains("twist") ) 
            {
                if (root) 
                {
                    root->setProperty("speed", 0.0);
                }
                return;
            }

            QJsonObject twist = msg.value("twist").toObject();
            QJsonObject child_twist = twist.value("twist").toObject();
            QJsonObject linear = child_twist.value("linear").toObject();
            double vx = linear.value("x").toDouble();
            //double vy = linear.value("y").toDouble();

            
            if ( vx < 0 ) { vx *= -1; }

            double speed = vx;
            if (speed > 1.0) { speed = 1.0; } // cap at 1.0 m/s
            
            speed = speed * 100.0; // convert to m/s for display

            if (root) 
            {
                QVariant qmlSpeed = QVariant::fromValue(speed);
                root->setProperty("speed", qmlSpeed);
            }
        }
        // topic name က /joint_states for Actual Robot RPMs
        else if( topic == "/joint_states" )
        {
            if (ros2ControlQmlView_.size() != 6) return;
            QObject* leftActualRpmRoot  = ros2ControlQmlView_[4] ? ros2ControlQmlView_[4]->rootObject() : nullptr;
            QObject* rightActualRpmRoot = ros2ControlQmlView_[5] ? ros2ControlQmlView_[5]->rootObject() : nullptr;

            if ( msg.isEmpty() || !msg.contains("name") || !msg.contains("velocity") ) 
            {
                if (leftActualRpmRoot) 
                {
                    leftActualRpmRoot->setProperty("speed", 0.0);
                }
                if (rightActualRpmRoot) 
                {
                    rightActualRpmRoot->setProperty("speed", 0.0);
                }
                return;
            }

            QJsonArray nameArray = msg.value("name").toArray();
            QJsonArray velocityArray = msg.value("velocity").toArray();
            double left_wheel_velocity  = 0.0;
            double right_wheel_velocity = 0.0;

            for (int i = 0; i < nameArray.size(); ++i) 
            {
                QString joint_name = nameArray[i].toString();
                double joint_velocity = velocityArray[i].toDouble();

                if (joint_name == "left_wheel_joint") 
                {
                    left_wheel_velocity = joint_velocity;
                } 
                else if (joint_name == "right_wheel_joint") 
                {
                    right_wheel_velocity = joint_velocity;
                }
            }
            left_wheel_velocity = left_wheel_velocity * (60.0 / (2.0 * M_PI)); // convert rad/s to RPM
            right_wheel_velocity = right_wheel_velocity * (60.0 / (2.0 * M_PI)); // convert rad/s to RPM

            if( left_wheel_velocity < 0 ) { left_wheel_velocity *= -1; }
            if( right_wheel_velocity < 0 ) { right_wheel_velocity *= -1; }

            if (leftActualRpmRoot)
            {
                QVariant qmlLeftActualRpm = QVariant::fromValue(static_cast<int>(left_wheel_velocity));
                leftActualRpmRoot->setProperty("speed", qmlLeftActualRpm);
            }
            if (rightActualRpmRoot)
            {
                QVariant qmlRightActualRpm = QVariant::fromValue(static_cast<int>(right_wheel_velocity));
                rightActualRpmRoot->setProperty("speed", qmlRightActualRpm);
            }
        }
    }
    

    /* EKF TAB */
    else if( currentMode == Mode::ekf ) 
    {
    QString ekf_odom_topic_name = "/odom";
    QString odom_topic_name = "/diff_controller/odom";
    QString imu_topic_name = "/imu/out";
        
    static QPointF ekf_position;
    static QPointF odom_position;
    static double imu_heading = 0.0;
    static double odom_heading = 0.0;
    static double ekf_heading = 0.0;

    // Covariances
    static double ekf_x  = 0.0; static double ekf_y = 0.0; static double ekf_yaw = 0.0;

    static double xx_cov = 0.0; static double xy_cov = 0.0;
    static double yx_cov = 0.0; static double yy_cov = 0.0;
    static double yaw_cov = 0.0;

        if( topic == ekf_odom_topic_name )
        {
            if ( msg.isEmpty() || !msg.contains("pose") ) 
            {
                return;
            }

            QJsonObject pose = msg.value("pose").toObject();
            QJsonObject pose_child = pose.value("pose").toObject();
            QJsonObject position = pose_child.value("position").toObject();
            double x = position.value("x").toDouble();
            double y = position.value("y").toDouble();
            ekf_position = QPointF(x, y);

            QJsonObject orientation = pose_child.value("orientation").toObject();
            double qx = orientation.value("x").toDouble();
            double qy = orientation.value("y").toDouble();
            double qz = orientation.value("z").toDouble();
            double qw = orientation.value("w").toDouble();
            ekf_heading = quaternionToYawDegrees(qx, qy, qz, qw);

           QJsonArray covariance_array = pose.value("covariance").toArray();

            if (covariance_array.isEmpty() || covariance_array.size() != 36) 
            {
                qDebug() << "Covariance array is invalid or incomplete.";
                return; 
            }
            
            xx_cov = covariance_array.at(0).toDouble();
            xy_cov = covariance_array.at(1).toDouble();
            yx_cov = covariance_array.at(6).toDouble();
            yy_cov = covariance_array.at(7).toDouble();
            yaw_cov = covariance_array.at(35).toDouble();

            Eigen::Matrix2d covariance_xy_matrix;
            covariance_xy_matrix << xx_cov, xy_cov, yx_cov, yy_cov;

            qDebug() << " EKF Odom Position: " << ekf_position << ", Heading: " << ekf_heading;
            qDebug() << " Diff Odom Position: " << odom_position << ", Heading: " << odom_heading;
            qDebug() << " EKF Odom Position Covariance: " << xx_cov << "," << xy_cov << "," << yx_cov << "," << yy_cov << "," << yaw_cov;

           
            ekf_x = x; ekf_y = y; //ekf_yaw = ekf_heading; ======================== for check

            // xx_cov = position_covariance_obj.value("0").toDouble();
            // xy_cov = position_covariance_obj.value("1").toDouble();
            // yx_cov = position_covariance_obj.value("6").toDouble();
            // yy_cov = position_covariance_obj.value("7").toDouble();
            // yaw_cov= position_covariance_obj.value("35").toDouble();

            if( ekfPositionCovarianceGraphPtr_ )
            {
                ekfPositionCovarianceGraphPtr_->updateGraph(ekf_x, ekf_y, covariance_xy_matrix);
            }
            if( ekfHeadingCovarianceGraphPtr_ )
            {
                ekfHeadingCovarianceGraphPtr_->updateGraph(ekf_heading, yaw_cov);
            }
            if( odomDiffOdomImuHeadingGraphPtr_ )
            {
                odomDiffOdomImuHeadingGraphPtr_->updateGraph(odom_heading, imu_heading, ekf_heading);
            }
            if( odomDiffOdomPositionGraphPtr_ )
            {
                odomDiffOdomPositionGraphPtr_->updateGraph(odom_position, ekf_position);
            }
        }
        else if( topic == odom_topic_name )
        {
            if ( msg.isEmpty() || !msg.contains("pose") ) 
            {
                return;
            }

            QJsonObject pose = msg.value("pose").toObject();
            QJsonObject pose_child = pose.value("pose").toObject();
            QJsonObject position = pose_child.value("position").toObject();
            double x = position.value("x").toDouble();
            double y = position.value("y").toDouble();
            odom_position = QPointF(x, y);

            QJsonObject orientation = pose_child.value("orientation").toObject();
            double qx = orientation.value("x").toDouble();
            double qy = orientation.value("y").toDouble();
            double qz = orientation.value("z").toDouble();
            double qw = orientation.value("w").toDouble();
            odom_heading = quaternionToYawDegrees(qx, qy, qz, qw);
        }
        else if( topic == imu_topic_name )
        {
            if ( msg.isEmpty() || !msg.contains("orientation") ) 
            {
                return;
            }

            QJsonObject orientation = msg.value("orientation").toObject();
            double qx = orientation.value("x").toDouble();
            double qy = orientation.value("y").toDouble();
            double qz = orientation.value("z").toDouble();
            double qw = orientation.value("w").toDouble();

            // Convert quaternion to yaw angle in degrees
            double siny_cosp = 2.0 * (qw * qz + qx * qy);
            double cosy_cosp = 1.0 - 2.0 * (qy * qy + qz * qz);
            double yaw = std::atan2(siny_cosp, cosy_cosp);
            double yaw_degrees = yaw * (180.0 / M_PI);
            imu_heading = yaw_degrees;
        }
    }

    /* CARTO TAB */
    else if( currentMode == Mode::carto )
    {
    auto ns_prefix = robotNamespace_;

    if (!ns_prefix.isEmpty() && !ns_prefix.startsWith('/')) ns_prefix.prepend('/');
    QString map_topic_name = ns_prefix + "/map";
    QString constraint_list_topic_name = ns_prefix + "/constraint_list";
    QString trajectory_node_list_topic_name = ns_prefix + "/trajectory_node_list";
    QString robot_pose_topic_name = ns_prefix + "/map_bfp_publisher";

        //QString scan_matched_points_topic_name = "/scan_matched_points2";

        //qDebug() << " Current mode is CARTO. Received topic: " << topic;
        if( topic == map_topic_name )
        {
            qDebug() << " Received map topic message ";
            if( mapWidgetPtr_ )
            {
                qDebug() << " Calling updateMap(msg)";
                mapWidgetPtr_->updateMap(msg);
            }
        }
        else if( topic == constraint_list_topic_name )
        {
            if( mapWidgetPtr_ )
            {
                qDebug() << " Calling updateConstraintList(msg)";
                mapWidgetPtr_->updateConstraintList(msg);
            }
        }
        else if( topic == trajectory_node_list_topic_name )
        {
            if( mapWidgetPtr_ )
            {
                qDebug() << " Calling updateTrajectoryNodeList(msg)";
                mapWidgetPtr_->updateTrajectoryNodeList(msg);
            }
        }
        else if( topic == robot_pose_topic_name )
        {
            if( mapWidgetPtr_ )
            {
                qDebug() << " Calling updateRobotPose(msg)";
                mapWidgetPtr_->updateRobotPose(msg);
            }
        }
        // else if( topic == scan_matched_points_topic_name ){}
    }

    /* NAV2 1 TAB */
    else if( currentMode == Mode::nav2_1 ) {}

    /* NAV2 2 TAB */
    else if( currentMode == Mode::nav2_2 ) {}

    /* NAV2 3 TAB */
    else if( currentMode == Mode::nav2_3 )
    {
        QString tf_topic_name = "/tf";
        
        if (topic == tf_topic_name)
        {
            if (msg.isEmpty() || !msg.contains("transforms")) return;
            
            QJsonArray transforms = msg.value("transforms").toArray();
            
            for (const QJsonValue &transformValue : transforms)
            {
                QJsonObject transform = transformValue.toObject();
                
                // Get header
                QJsonObject header = transform.value("header").toObject();
                QString child_frame = transform.value("child_frame_id").toString();
                QString parent_frame = header.value("frame_id").toString();
                
                // Get transform data
                QJsonObject trans = transform.value("transform").toObject();
                QJsonObject translation = trans.value("translation").toObject();
                QJsonObject rotation = trans.value("rotation").toObject();
                
                double tx = translation.value("x").toDouble();
                double ty = translation.value("y").toDouble();
                double tz = translation.value("z").toDouble();
                
                double qx = rotation.value("x").toDouble();
                double qy = rotation.value("y").toDouble();
                double qz = rotation.value("z").toDouble();
                double qw = rotation.value("w").toDouble();
                
                // Update TF tree widget
                if (tfTreeWidget_)
                {
                    tfTreeWidget_->updateTransform(parent_frame, child_frame,
                                                  tx, ty, tz,
                                                  qx, qy, qz, qw);
                }
            }
        }
    }

    /* BT TAB */
    else if( currentMode == Mode::bt )
    {
        QString bt_topic_name = "/behavior_tree";
        
        if (topic == bt_topic_name)
        {
            // Extract behavior tree string from std_msgs/String
            QString bt_data = msg.value("data").toString();
            
            if (!bt_data.isEmpty() && bt_data != lastBehaviorTree_)
            {
                lastBehaviorTree_ = bt_data;
                
                // Update status label
                if (btStatusLabel_) {
                    btStatusLabel_->setText("Behavior Tree Active");
                    btStatusLabel_->setStyleSheet("font: 11pt 'SF Pro'; color: #03fc84; background: transparent;");
                }
                
                // Parse and display the behavior tree
                if (btTreeView_) {
                    // Format the tree data for better readability
                    QString formattedTree = formatBehaviorTree(bt_data);
                    btTreeView_->setPlainText(formattedTree);
                }
                
                qDebug() << "Behavior tree updated";
            }
        }
    }

    // /* TOPIC TAB */
    // else if( currentMode == Mode::topic ) 
    // {
    //     // 
    // }

    /* LOG TAB */
    else if( currentMode == Mode::log ) {}
}

void MainWindow::onReceivedActionFeedback(const QString &action_name, const QJsonObject &feedback)
{
    // Determine which tab is handling this feedback
    bool isNav2_1 = (currentMode == Mode::nav2_1 && action_name == "/navigate_to_pose");
    bool isNav2_2 = (currentMode == Mode::nav2_2 && action_name == "/navigate_through_poses");
    
    if (!isNav2_1 && !isNav2_2) return;
    
    qDebug() << "Action feedback received for:" << action_name;
    qDebug() << "Feedback data:" << feedback;

    // Select the appropriate UI components based on current mode
    QTimer* feedbackTimer = isNav2_1 ? nav2_1FeedbackTimer_ : nav2_2FeedbackTimer_;
    QLabel* nullLabel = isNav2_1 ? nav2_1NullLabel_ : nav2_2NullLabel_;
    QWidget* feedbackWidget = isNav2_1 ? nav2_1FeedbackWidget_ : nav2_2FeedbackWidget_;
    QVBoxLayout* feedbackLayout = isNav2_1 ? nav2_1FeedbackLayout_ : nav2_2FeedbackLayout_;
    QMap<QString, QLabel*>& feedbackLabels = isNav2_1 ? nav2_1FeedbackLabels_ : nav2_2FeedbackLabels_;
    QStringList& behaviorTreeStates = isNav2_1 ? nav2_1BehaviorTreeStates_ : nav2_2BehaviorTreeStates_;
    int maxStatesSize = isNav2_1 ? nav2_1BehaviorTreeStatesMaxSize_ : nav2_2BehaviorTreeStatesMaxSize_;

    // Reset feedback timeout timer
    if (feedbackTimer) {
        feedbackTimer->start();
    }

    // Hide null label
    if (nullLabel) {
        nullLabel->setVisible(false);
    }

    // Helper lambda to recursively parse JSON and create/update labels
    std::function<void(const QString&, const QJsonValue&, int)> displayJsonValue;
    displayJsonValue = [&](const QString& key, const QJsonValue& value, int indent) {
        QString prefix = QString(indent * 2, ' ');
        
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                displayJsonValue(key.isEmpty() ? it.key() : key + "." + it.key(), it.value(), indent);
            }
        }
        else if (value.isArray()) {
            QJsonArray arr = value.toArray();
            
            // Special handling for behavior_tree_states array
            if (key == "behavior_tree_states") {
                // Add new states to history (keep only last N)
                for (int i = 0; i < arr.size(); ++i) {
                    QString state = arr[i].toString();
                    if (!state.isEmpty()) {
                        behaviorTreeStates.append(state);
                        // Keep only last N entries
                        if (behaviorTreeStates.size() > maxStatesSize) {
                            behaviorTreeStates.removeFirst();
                        }
                    }
                }
                
                // Display all stored states
                QString labelKey = "behavior_tree_states";
                QLabel* label = nullptr;
                
                if (feedbackLabels.contains(labelKey)) {
                    label = feedbackLabels[labelKey];
                } else {
                    label = new QLabel(feedbackWidget);
                    label->setStyleSheet(
                        "QLabel {"
                        "  font: 10pt 'SF Pro';"
                        "  color: #03fc84;"
                        "  background: transparent;"
                        "  padding: 3px;"
                        "}"
                    );
                    label->setWordWrap(true);
                    feedbackLabels[labelKey] = label;
                    feedbackLayout->addWidget(label);
                }
                
                // Build display text with all states (oldest to newest)
                QString displayText = QString("behavior_tree_states (last %1):\n").arg(maxStatesSize);
                for (int i = 0; i < behaviorTreeStates.size(); ++i) {
                    displayText += QString("  [%1] %2\n").arg(i + 1).arg(behaviorTreeStates[i]);
                }
                
                label->setText(displayText);
                label->setVisible(true);
                
                return; // Skip normal array processing
            }
            
            // Normal array processing for other arrays
            for (int i = 0; i < arr.size(); ++i) {
                displayJsonValue(key + "[" + QString::number(i) + "]", arr[i], indent);
            }
        }
        else {
            // Create or update label for this field
            QString labelKey = key;
            QString displayValue;
            
            if (value.isDouble()) {
                displayValue = QString::number(value.toDouble(), 'f', 4);
            } else if (value.isBool()) {
                displayValue = value.toBool() ? "true" : "false";
            } else if (value.isString()) {
                displayValue = value.toString();
            } else {
                displayValue = "null";
            }
            
            QLabel* label = nullptr;
            if (feedbackLabels.contains(labelKey)) {
                label = feedbackLabels[labelKey];
            } else {
                // Create new label
                label = new QLabel(feedbackWidget);
                label->setStyleSheet(
                    "QLabel {"
                    "  font: 11pt 'SF Pro';"
                    "  color: #03fc84;"
                    "  background: transparent;"
                    "  padding: 3px;"
                    "}"
                );
                feedbackLabels[labelKey] = label;
                feedbackLayout->addWidget(label);
            }
            
            label->setText(QString("%1: %2").arg(labelKey, displayValue));
            label->setVisible(true);
        }
    };

    // Parse all feedback fields
    displayJsonValue("", feedback, 0);
}

void MainWindow::onReceivedServiceResponse(const QString &service_name, const QString &id, const QJsonObject &msg)
{
    qDebug() << "Service response received. Service:" << service_name << ", ID:" << id;
    qDebug() << "Response values:" << msg;
    
    // Handle topic tab node checking
    if (id == "topic_tab_node_check" && currentMode == Mode::topic) {
        // Extract nodes array from response
        QJsonArray nodes_array = msg.value("nodes").toArray();
        QStringList active_nodes;
        
        for (const QJsonValue &node_val : nodes_array) {
            QString node_name = node_val.toString();
            // Remove namespace prefix if present
            if (!robotNamespace_.isEmpty()) {
                if (node_name.startsWith(robotNamespace_)) {
                    node_name.remove(0, robotNamespace_.length());
                }
            }
            active_nodes.append(node_name);
        }
        
        qDebug() << "Active nodes:" << active_nodes;
        
        // Store active nodes list
        topicTabActiveNodes_ = active_nodes;
        
        // Update button colors based on node status
        for (auto it = topicTabNodeButtons_.begin(); it != topicTabNodeButtons_.end(); ++it) {
            QString node_name = it.key();
            QPushButton *btn = it.value();
            
            if (!btn) continue;
            
            if (active_nodes.contains(node_name)) {
                // Node exists - green background
                btn->setStyleSheet(
                    "QPushButton {"
                    "  background: rgb(0, 255, 0);"
                    "  font: 14px solid;"
                    "  color: black;"
                    "}"
                );
            } else {
                // Node not found - red background
                btn->setStyleSheet(
                    "QPushButton {"
                    "  background: rgb(246, 97, 81);"
                    "  font: 14px solid;"
                    "  color: white;"
                    "}"
                );
            }
        }
    }
    // Handle node info request response
    else if (id.startsWith("node_info_")) {
        QString node_name = id.mid(10); // Remove "node_info_" prefix
        
        // Extract node details from response
        QJsonArray subscribing = msg.value("subscribing").toArray();
        QJsonArray publishing = msg.value("publishing").toArray();
        QJsonArray services = msg.value("services").toArray();
        
        QString info_text = QString("Node: %1\n\n").arg(node_name);
        
        // Subscribing topics
        info_text += "Subscribing Topics:\n";
        if (subscribing.isEmpty()) {
            info_text += "  (none)\n";
        } else {
            for (const QJsonValue &topic : subscribing) {
                info_text += QString("  - %1\n").arg(topic.toString());
            }
        }
        info_text += "\n";
        
        // Publishing topics
        info_text += "Publishing Topics:\n";
        if (publishing.isEmpty()) {
            info_text += "  (none)\n";
        } else {
            for (const QJsonValue &topic : publishing) {
                info_text += QString("  - %1\n").arg(topic.toString());
            }
        }
        info_text += "\n";
        
        // Services
        info_text += "Services:\n";
        if (services.isEmpty()) {
            info_text += "  (none)\n";
        } else {
            for (const QJsonValue &service : services) {
                info_text += QString("  - %1\n").arg(service.toString());
            }
        }
        
        // Show dialog
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Node Information");
        msgBox.setText(info_text);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #2e2e2e; color: #03fc84; }"
            "QLabel { color: #03fc84; font: 10pt 'SF Pro'; }"
            "QPushButton { background-color: #444; color: white; padding: 5px 15px; border-radius: 3px; }"
            "QPushButton:hover { background-color: #555; }"
        );
        msgBox.exec();
    }
}

void MainWindow::robotVelocityToWheelRpms(double linear_velocity, double angular_velocity, double wheel_radius, double wheel_seperation, int &left_rpm, int &right_rpm)
{
    // Calculate wheel linear velocities
    double v_left = linear_velocity - (angular_velocity * wheel_seperation / 2.0);
    double v_right = linear_velocity + (angular_velocity * wheel_seperation / 2.0);

    // Convert linear velocities to angular velocities (rad/s)
    double omega_left = v_left / wheel_radius;
    double omega_right = v_right / wheel_radius;

    // Convert angular velocities to RPM
    left_rpm = static_cast<int>((omega_left * 60.0) / (2.0 * M_PI));
    right_rpm = static_cast<int>((omega_right * 60.0) / (2.0 * M_PI));
}

double MainWindow::quaternionToYawDegrees(double &qx, double &qy, double &qz, double &qw)
{
    double siny_cosp = 2.0 * (qw * qz + qx * qy);
    double cosy_cosp = 1.0 - 2.0 * (qy * qy + qz * qz);
    double yaw = std::atan2(siny_cosp, cosy_cosp);
    double yaw_degrees = yaw * (180.0 / M_PI);
    return yaw_degrees;
}

double MainWindow::yawDegreesToQuaternion(double &yaw_degrees, double &qx, double &qy, double &qz, double &qw)
{
    double half_yaw = yaw_degrees * 0.5 * (M_PI / 180.0);
    qw = std::cos(half_yaw);
    qx = 0.0;
    qy = 0.0;
    qz = std::sin(half_yaw);
}

QString MainWindow::formatBehaviorTree(const QString &bt_data)
{
    // Format the behavior tree XML/string for better readability
    QString formatted = bt_data;
    
    // Add newlines and indentation for XML-like structure
    formatted.replace("><", ">\n<");
    
    // Add indentation
    QStringList lines = formatted.split('\n');
    QString result;
    int indent = 0;
    
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        
        if (trimmed.isEmpty()) continue;
        
        // Decrease indent for closing tags
        if (trimmed.startsWith("</")) {
            indent = qMax(0, indent - 2);
        }
        
        // Add indented line
        result += QString(indent, ' ') + trimmed + "\n";
        
        // Increase indent for opening tags (but not self-closing)
        if (trimmed.startsWith("<") && !trimmed.startsWith("</") && 
            !trimmed.endsWith("/>") && !trimmed.contains("</")) {
            indent += 2;
        }
    }
    
    return result.isEmpty() ? bt_data : result;
}
