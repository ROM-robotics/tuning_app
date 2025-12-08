#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QString>
#include <QQuickWidget>

#include "design/rom_design.hpp"
#include "design/covarianceDisplay.hpp"
#include "design/rom_structures.h"
#include "design/tf_tree_widget.hpp"
#include "communication/ros_bridge_client.hpp"

// Forward declare RomMapWidget (defined in sdk/rom_map_widget.hpp)
namespace rom_dynamics { namespace ui { namespace qt { class RomMapWidget; class CartoRomMapWidget; } } }

using rom_dynamics::communication::RosBridgeClient;
using rom_dynamics::data_types::Mode;

using rom_dynamics::ui::qt::RomPolarHeadingGraph;
using rom_dynamics::ui::qt::RomPositionGraph;
using rom_dynamics::ui::qt::RomPositionCovarianceGraph;
using rom_dynamics::ui::qt::RomYawCovarianceGraph;
using rom_dynamics::ui::qt::RomMapWidget;
using rom_dynamics::ui::qt::CartoRomMapWidget;


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setConnectionParams(const QString &ip, const QString &password, const QString &ns = "");
    void autoConnect();
    void switchToControlTab();
    void createCommunicationClient(const QString &robot_ns, const QString &host, quint16 port);

    void initRos2ControlTab();
    void activateRos2ControlTab();
    void deactivateRos2ControlTab();

    void initEkfTab();
    void activateEkfTab();
    void deactivateEkfTab();

    void initCartoTab();
    void activateCartoTab();
    void deactivateCartoTab();

    void initNav2_1Tab();
    void activateNav2_1Tab();
    void deactivateNav2_1Tab();

    void initNav2_2Tab();
    void activateNav2_2Tab();
    void deactivateNav2_2Tab();

    void initNav2_3Tab();
    void activateNav2_3Tab();
    void deactivateNav2_3Tab();

    void initGoalTab();
    void activateGoalTab();
    void deactivateGoalTab();

    void initBtTab();
    void activateBtTab();
    void deactivateBtTab();

    void initTopicTab();
    void activateTopicTab();
    void deactivateTopicTab();

    void initLogTab();
    void activateLogTab();
    void deactivateLogTab();

    void robotVelocityToWheelRpms(double linear_velocity, double angular_velocity, double wheel_radius, double wheel_seperation, int &left_rpm, int &right_rpm);

    double quaternionToYawDegrees(double &qx, double &qy, double &qz, double &qw);
    double yawDegreesToQuaternion(double &yaw_degrees, double &qx, double &qy, double &qz, double &qw);
    
    QString formatBehaviorTree(const QString &bt_data);

signals:
    //void createCommunicationClient(const QString &robot_ns, const QString &host, quint16 port);

private slots:
    void onTabChanged(int index);
    void on_closeBtn_clicked();
    void on_rsyncBtn_clicked();
    void on_connectBtn_clicked();
    void on_hostTerminalBtn_clicked();
    void on_robotTerminalBtn_clicked();
    void on_ekfTuningGuideBtn_clicked();
    void on_initialNoiseCovToggleBtn_clicked();
    void on_processNoiseCovToggleBtn_clicked();

    // from web socket
    void onReceivedTopicMessage(const QString &topic, const QJsonObject &msg);
    void onReceivedServiceResponse(const QString &service_name, const QString &id, const QJsonObject &msg);
    void onReceivedActionFeedback(const QString &action_name, const QJsonObject &feedback);


protected:
    // test();

private:
    Ui::MainWindow *ui;

    QString robotIp_         = "192.168.1.xx";
    QString robotPort_       = "9090";
    QString password_        = "ghostman";
    QString robotNamespace_  = "";
    bool isConnected_        = false;

    int previousCmdColorIndex = -1;

    Mode currentMode;
    Mode previousMode;
    RosBridgeClient *communication_ = nullptr;
    QThread *communicationThread_ = nullptr;

    // QRC speed meter
    QVector<QQuickWidget*> ros2ControlQmlView_;
    QQuickItem *qmlRoot_ = nullptr;
    QTimer *qmlUpdateTimer_ = nullptr;
    QVector<QTimer*> speedTimeoutTimers_;

    // robotspecs
    double wheel_radius_ = 0.100; // meters
    double wheel_seperation_ = 0.400; // meters

    // ekf
    RomPolarHeadingGraph *odomDiffOdomImuHeadingGraphPtr_ = nullptr;
    RomPositionGraph *odomDiffOdomPositionGraphPtr_ = nullptr;
    RomPositionCovarianceGraph *ekfPositionCovarianceGraphPtr_ = nullptr;
    RomYawCovarianceGraph *ekfHeadingCovarianceGraphPtr_ = nullptr;

    CartoRomMapWidget *mapWidgetPtr_ = nullptr;

    // nav2_1 - action feedback display
    QScrollArea *nav2_1ScrollArea_ = nullptr;
    QWidget *nav2_1FeedbackWidget_ = nullptr;
    QVBoxLayout *nav2_1FeedbackLayout_ = nullptr;
    QLabel *nav2_1NullLabel_ = nullptr;
    QTimer *nav2_1FeedbackTimer_ = nullptr;
    QMap<QString, QLabel*> nav2_1FeedbackLabels_; // key: field name, value: label widget
    QStringList nav2_1BehaviorTreeStates_; // Keep last N behavior tree states
    const int nav2_1BehaviorTreeStatesMaxSize_ = 20; // Maximum number of states to keep

    // nav2_2 - action feedback display (NavigateThroughPoses)
    QScrollArea *nav2_2ScrollArea_ = nullptr;
    QWidget *nav2_2FeedbackWidget_ = nullptr;
    QVBoxLayout *nav2_2FeedbackLayout_ = nullptr;
    QLabel *nav2_2NullLabel_ = nullptr;
    QTimer *nav2_2FeedbackTimer_ = nullptr;
    QMap<QString, QLabel*> nav2_2FeedbackLabels_; // key: field name, value: label widget
    QStringList nav2_2BehaviorTreeStates_; // Keep last N behavior tree states
    const int nav2_2BehaviorTreeStatesMaxSize_ = 20; // Maximum number of states to keep

    // nav2_3 - TF tree visualization
    TFTreeWidget *tfTreeWidget_ = nullptr;

    // bt tab - behavior tree visualization
    QScrollArea *btScrollArea_ = nullptr;
    QWidget *btWidget_ = nullptr;
    QVBoxLayout *btLayout_ = nullptr;
    QLabel *btStatusLabel_ = nullptr;
    QTextEdit *btTreeView_ = nullptr;
    QString lastBehaviorTree_;

    // topic tab - node status monitoring
    QMap<QString, QPushButton*> topicTabNodeButtons_; // key: node name, value: button widget
    QTimer *topicTabCheckTimer_ = nullptr;
    QStringList topicTabActiveNodes_; // List of currently active nodes

    // log tab - systemctl service management
    QMap<QString, QPushButton*> logStatusButtons_;   // service name -> status button
    QMap<QString, QPushButton*> logEnableButtons_;   // service name -> enable button
    QMap<QString, QPushButton*> logDisableButtons_;  // service name -> disable button
    QMap<QString, QPushButton*> logStartButtons_;    // service name -> start button
    QMap<QString, QPushButton*> logStopButtons_;     // service name -> stop button
    QMap<QString, QPushButton*> logLogsButtons_;     // service name -> logs button

    // goal tab - navigation goal sending
    QComboBox *goalToPoseX_ = nullptr;
    QComboBox *goalToPoseY_ = nullptr;
    QComboBox *goalToPoseTheta_ = nullptr;
    QPushButton *navigateToPoseBtn_ = nullptr;
    
    QComboBox *goalThroughPosesX_[4];
    QComboBox *goalThroughPosesY_[4];
    QComboBox *goalThroughPosesTheta_[4];
    QPushButton *navigateThroughPosesBtn_ = nullptr;
    
private slots:
    void onNodeButtonClicked();
    void onLogServiceButtonClicked();
    void onNavigateToPoseClicked();
    void onNavigateThroughPosesClicked();

};
#endif // MAINWINDOW_H
