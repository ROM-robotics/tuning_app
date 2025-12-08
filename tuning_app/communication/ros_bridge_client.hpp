#ifndef ROSE_BRIDGE_CLIENT_HPP
#define ROSE_BRIDGE_CLIENT_HPP

#pragma once
#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace rom_dynamics::communication {
    
class RosBridgeClient : public QObject 
{
    Q_OBJECT
public:
    explicit RosBridgeClient(const QString &robot_ns="", 
        const QString &host = "127.0.0.1", 
        quint16 port = 9090, 
        QObject *parent = nullptr);

    // --------------------------------- MAIN API
    bool isConnected() const { return m_socket->state() == QAbstractSocket::ConnectedState; }

signals:
    // --------------------------------- MAIN API
    void connected();
    void disconnected();
    void errorOccurred(const QString &msg);
    
    // --------------------------------- TOPIC SUBSCRIPTIONS
    void receivedTopicMessage(const QString &topic_name, const QJsonObject &msg);

    // --------------------------------- ACTION FEEDBACK
    void receivedActionFeedback(const QString &action_name, const QJsonObject &feedback);

    // ⭐ rosapi related, and all services that we request from mainwindow
    void receivedServiceResponse(const QString &service_name, const QString &id, const QJsonObject &response_values);

public slots:
    // --------------------------------- MAIN API
    void init();
    void connectToServer();
    void disconnectFromServer();
    
    // --------------------------------- TOPIC SUBSCRIPTIONS
    void subscribeTopic(const QString &topic_name, const QString &msg_type);
    void unsubscribeTopic(const QString &topic_name);

    // --------------------------------- ACTION SUBSCRIPTIONS
    void subscribeActionFeedback(const QString &action_name, const QString &action_type);
    void unsubscribeActionFeedback(const QString &action_name);
    
    // --------------------------------- ACTION GOAL
    void sendActionGoal(const QString &action_name, const QString &goal_id, const QString &action_type, const QJsonObject &goal);

    // ⭐ rosapi related
    void getTopicsList(const QString &id);
    // ⭐ Helper function for rosapi service call
    void callService(const QString &service_name, const QString &id, const QString &msg_type);
    // ⭐ Helper function for rosapi service call with arguments
    void callService(const QString &service_name, const QString &id, const QString &msg_type, const QJsonObject &args);
    
private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError err);
    
    void onTextMessageReceived(const QString &msg);

    
protected:
        

private:
    void sendJson(const QJsonObject &obj);
    void ensureReconnect();

    // main api variables
    // QWebSocket m_socket;
    // ⭐ QWebSocket နှင့် QTimer ကို Pointers များအဖြစ် သုံးတယ်။
    QWebSocket *m_socket = nullptr;

    QString m_robotNamespace;
    QString m_host;
    quint16 m_port{9090};

    // QTimer m_reconnectTimer;
    // ⭐ QTimer ကို Pointer အဖြစ် သုံးတယ်။
    QTimer *m_reconnectTimer = nullptr; 
    
};
}

#endif
