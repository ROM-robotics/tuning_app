#include "ros_bridge_client.hpp"
//#include "rom_structures.h" // for ROM_COLOR_* macros

#include <QDateTime>
#include <QDebug>
#include <cmath>
#include <QMetaType>

rom_dynamics::communication::RosBridgeClient::RosBridgeClient(const QString &robot_ns, const QString &host, quint16 port, QObject *parent)
    : QObject(parent), m_robotNamespace(robot_ns), m_host(host), m_port(port) 
{
    // ⭐ ဤနေရာတွင် မည်သည့် connect မှ မလုပ်ပါနှင့်။
    // ⭐ Pointers များကို nullptr ပေးထားပါ (သို့မဟုတ် Header တွင် ပေးထားပါ)

    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::RosBridgeClient      ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    // connect(&m_socket, &QWebSocket::connected, this, &RosBridgeClient::onSocketConnected);
    // connect(&m_socket, &QWebSocket::disconnected, this, &RosBridgeClient::onSocketDisconnected);
    // connect(&m_socket, &QWebSocket::textMessageReceived, this, &RosBridgeClient::onTextMessageReceived);
    // connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &RosBridgeClient::onSocketError);

    // //this->connectToServer();

    // m_reconnectTimer.setInterval(3000);
    // m_reconnectTimer.setSingleShot(true);
    // connect(&m_reconnectTimer, &QTimer::timeout, this, &RosBridgeClient::ensureReconnect);
}

// MAIN API
// ⭐ QObject-based members များကို ဖန်တီးမည့် slot
void rom_dynamics::communication::RosBridgeClient::init()
{
    // ဤ init() သည် ယခု Worker Thread တွင် run နေပါသည်။
    
    // Pointers များကို Worker Thread ထဲတွင် ဖန်တီးခြင်း
    m_socket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this); // Parent ကို 'this' ပေးခြင်းဖြင့် affinity ရပါသည်
    m_reconnectTimer = new QTimer(this); // Parent ကို 'this' ပေးခြင်းဖြင့် affinity ရပါသည်

    // Signals/Slots များကို ချိတ်ဆက်ခြင်း
    connect(m_socket, &QWebSocket::connected, this, &RosBridgeClient::onSocketConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &RosBridgeClient::onSocketDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &RosBridgeClient::onTextMessageReceived);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &RosBridgeClient::onSocketError);

    m_reconnectTimer->setInterval(3000);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &RosBridgeClient::ensureReconnect);
}

void rom_dynamics::communication::RosBridgeClient::connectToServer() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::connectToServer      ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    if (isConnected()) return;

    const QUrl url(QStringLiteral("ws://%1:%2").arg(m_host).arg(m_port));
    m_socket->open(url);
}

void rom_dynamics::communication::RosBridgeClient::disconnectFromServer() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[    RosBridgeClient::disconnectFromServer   ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    m_reconnectTimer->stop();
    
    if (isConnected()) 
    {
        m_socket->close();
    }
}

void rom_dynamics::communication::RosBridgeClient::sendJson(const QJsonObject &obj) 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[         RosBridgeClient::sendJson          ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    QJsonDocument doc(obj);
    m_socket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void rom_dynamics::communication::RosBridgeClient::ensureReconnect() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::ensureReconnect      ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    if ( !isConnected() ) 
    {
        connectToServer();
    }
}


void rom_dynamics::communication::RosBridgeClient::onSocketConnected() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::onSocketConnected     ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    emit connected(); // slot to mainwindow
    
}

void rom_dynamics::communication::RosBridgeClient::onSocketDisconnected() 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[    RosBridgeClient::onSocketDisconnected   ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    emit disconnected();

    if (!isConnected()) 
    {
        m_reconnectTimer->start();
    }
}

void rom_dynamics::communication::RosBridgeClient::onSocketError(QAbstractSocket::SocketError) 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[       RosBridgeClient::onSocketError       ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    emit errorOccurred(m_socket->errorString());
    if (!isConnected()) 
    {
        m_reconnectTimer->start();
    }
}

void rom_dynamics::communication::RosBridgeClient::onTextMessageReceived(const QString &msg) 
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[   RosBridgeClient::onTextMessageReceived   ] : %2 %3")
                              .arg(ROM_COLOR_GREEN).arg(msg).arg(ROM_COLOR_RESET);
    #endif

    // qDebug() << " RosBridgeClient::onTextMessageReceived Msg: " << msg;

    QJsonParseError err{};
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError) 
    {
        #ifdef ROM_DEBUG
            qWarning() << "RosBridgeClient JSON parse error:" << err.errorString();
        #endif

        return;
    }

    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();

    const QString op = obj.value("op").toString();

    if (op == "publish") 
    {
        const QString topic = obj.value("topic").toString();
        
            emit receivedTopicMessage(topic, obj.value("msg").toObject());
            // qDebug() << "receivedTopicMessage emitted" << obj;
    }
    // ⭐ Service Response ကို Handle လုပ်ခြင်း
    else if (op == "service_response") 
    {

        //const QString service_name = obj.value("service").toString();
        // ⭐ Service ID ကို ထုတ်ယူပါ။
        const QString response_id = obj.value("id").toString(); 
        
        QJsonObject response_values = obj.value("values").toObject(); 

        qDebug() << response_id << ", " << response_values;   
        //emit receivedServiceResponse(service_name, response_id, response_values); 
        
        #ifdef ROM_DEBUG
            qDebug() << "Received service response. ID:" << response_id << ", Service:" << service_name;
        #endif
    }
}


// ROSBRIDGE API
void rom_dynamics::communication::RosBridgeClient::subscribeTopic(const QString &topic_name, const QString &msg_type)
{
    if (!isConnected())
    {
        connectToServer();
    }

    QString topic_to_subscribe = m_robotNamespace + topic_name;

    QJsonObject msg;

    msg["op"] = "subscribe";
    msg["topic"] = topic_to_subscribe;
    msg["type"] = msg_type;
    sendJson(msg);
   

    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::subscribeTopic      ] : %2 %3 ")
                              .arg(ROM_COLOR_GREEN).arg(topic_to_subscribe).arg(ROM_COLOR_RESET);
    #endif
}


void rom_dynamics::communication::RosBridgeClient::unsubscribeTopic(const QString &topic_name)
{
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[     RosBridgeClient::unsubscribeTopic     ]%2")
                              .arg(ROM_COLOR_GREEN).arg(ROM_COLOR_RESET);
    #endif

    if (!isConnected() ) return;

    QString topic_to_unsubscribe = m_robotNamespace + topic_name;

    QJsonObject msg;
    msg["op"] = "unsubscribe";
    msg["topic"] = topic_to_unsubscribe;
    sendJson(msg);

}


void rom_dynamics::communication::RosBridgeClient::getTopicsList(const QString &id)
{
    const QString &msg_type = "rosapi_msgs/srv/Topics";
    callService("rosapi/topics", id, msg_type);
}


void rom_dynamics::communication::RosBridgeClient::callService(const QString &service_name, const QString &id, const QString &msg_type)
{
    if (!isConnected())
    {
        qWarning() << "RosBridgeClient::callService - Not connected to server.";
        return;
    }
    if ( id.isEmpty() || service_name.isEmpty() || msg_type.isEmpty() ) 
    {
        qWarning() << "service name or id or msg_type is empty";
        return;
    }
    
    // rosbridge protocol: { "op": "call", "service": "...", "args": {...}, "id": "..." }

    QString service_name_with_ns = m_robotNamespace + service_name;

    QJsonObject msg;
    msg["op"] = "call";
    msg["service"] = service_name_with_ns;
    msg["type"] = msg_type;
    msg["id"] = id;

    QJsonObject args;
    msg["args"] = args;

    sendJson(msg);
    
    #ifdef ROM_DEBUG
        qDebug().noquote() << QString("%1[      RosBridgeClient::callService         ] : %2 %3")
                              .arg(ROM_COLOR_GREEN).arg(service_name).arg(ROM_COLOR_RESET);
    #endif
}