#ifndef CHANNEL_H
#define CHANNEL_H

#include "soro_global.h"
#include "logger.h"
#include <QUdpSocket>
#include <QTcpSocket>
#include <cstring>

#define UDP_HEADER_BYTES 5
#define TCP_HEADER_BYTES 7

#define IDLE_CONNECTION_TIMEOUT 5000
#define QOS_UPDATE_INTERVAL 1000
#define TCP_VERIFY_WINDOW 5000
#define MONITOR_INTERVAL 100

#define SENT_TIME_TABLE_CAP 500

#define LOG_I(X) if (_log != NULL) _log->i(LOG_TAG, X)
#define LOG_W(X) if (_log != NULL) _log->w(LOG_TAG, X)
#define LOG_E(X) if (_log != NULL) _log->e(LOG_TAG, X)

typedef quint32 MESSAGE_ID;     //4 bytes
typedef quint8 MESSAGE_TYPE;    //1 byte
typedef quint16 MESSAGE_LENGTH; //2 bytes

/* Channels abstract over internet communication in a super-easy way, supporting either
 * TCP or UDP as the transport protocol.
 *
 *
 * The following information describes the inner workings of this class and is not
 * necessary to understand the API.
 *
 * -------- TCP message structure --------
 *  (2 bytes)   Packet Length                   (quint16)
 *  (1 byte)    Type                            (quint8)
 *  (4 bytes)   ID                              (quint32)
 *  ~ Message Data ~
 *
 * -------- UDP message structure --------
 *  (1 byte)    Type                            (quint8)
 *  (4 bytes)   ID                              (quint32)
 *  ~ Message Data ~
 *
 * The 'Type' field contains information about the data contained in the message.
 *  - For a normal message, 'Type' is TYPE_NORMAL.
 *
 *  - For a handshaking message, 'Type' is TYPE_HANDSHAKE, and the
 * message data contains the channel name the other side was given.
 *
 *  - For a heartbeat message, 'Type' is TYPE_HEARTBEAT, and the
 * message data is empty.
 *
 * The ID field uniquely identifies all messages sent by this endpoint. The ID value
 * increases (newer messages have higher IDs), so they also function an a sequence number
 * in UDP mode.
 */
class SOROSHARED_EXPORT Channel: public QObject {
    Q_OBJECT

public:
    
    /* Wrapper for a QHostAddress (IP) and quint16 (port) used to identify
     * the address of a network socket
     */
    struct SocketAddress {
        QHostAddress address;
        quint16 port;

        SocketAddress(QHostAddress address, quint16 port) {
            this->address = address;
            this->port = port;
        }

        SocketAddress() {
            this->address = QHostAddress::Any;
            this->port = 0;
        }

        inline QString toString() {
            return address.toString() + ":" + QString::number(port);
        }

        inline bool operator==(const SocketAddress& other) {
            return (address == other.address) & (port = other.port);
        }

        inline bool operator!=(const SocketAddress& other) {
            return !(*this == other);
        }
    };

    /* Protocol modes supported by a Channel
     */
    enum Protocol {
        PROTOCOL_UDP, PROTOCOL_TCP
    };

    /* Lists the current state a Channel can be in
     */
    enum Status {
        CHANNEL_STATUS_CONNECTING,
        CHANNEL_STATUS_CONNECTED,
        CHANNEL_STATUS_DISCONNECTED,
        CHANNEL_STATUS_FATAL_ERROR
    };

    /* Contains initialization information needed to create
     * a channel
     */
    struct Configuration {
        SocketAddress serverAddress;
        bool isServer;
        bool dropOldUdpPackets;
        Logger *logger;
        QString name;
        Protocol protocol;
    };

    //The maximum size of a sent message (the header may make the actual message
    //slighty larger)
    static const MESSAGE_LENGTH MAX_MESSAGE_LENGTH = 512;

    ~Channel();
    
    Channel(QObject *parent, Configuration configuration) ;
    Channel (QObject *parent, QString configFile, Logger *log);
    QString getName();
    Protocol getProtocol();
    SocketAddress getPeerAddress();
    void route(Channel *receiver);
    void unroute(Channel *receiver);
    void open();
    void close();
    bool sendMessage(QByteArray message);
    bool isServer();
    Status getStatus();

private:
    //Message type identifiers
    static const MESSAGE_TYPE MSGTYPE_NORMAL = 0;
    static const MESSAGE_TYPE MSGTYPE_CLIENT_HANDSHAKE = 1;
    static const MESSAGE_TYPE MSGTYPE_SERVER_HANDSHAKE = 2;
    static const MESSAGE_TYPE MSGTYPE_HEARTBEAT = 3;
    static const MESSAGE_TYPE MSGTYPE_QOS_ACK = 4;

    static const QString IPV4_REGEX;

    char *_buffer;
    MESSAGE_LENGTH _bufferLength;
    QByteArray _nameUtf8;
    Status _status;
    QHash<MESSAGE_ID, QTime>* _sentTimeTable;
    bool _qosAckNextMessage;
    QString _name;
    QString LOG_TAG;
    SocketAddress _serverAddress;
    SocketAddress _hostAddress;
    SocketAddress _peerAddress;
    Protocol _protocol;
    Logger* _log;
    int _monitorTimeout;
    bool _socketError;
    bool _isServer;
    bool _dropOldPackets;
    int _tcpVerifyTimeouts;
    bool _tcpVerified;
    QTcpSocket *_tcpSocket;
    QTcpServer *_tcpServer;
    QUdpSocket *_udpSocket;
    QAbstractSocket *_socket;
    MESSAGE_ID _nextSendID;
    MESSAGE_ID _lastReceiveID;
    quint64 _messagesUp;
    quint64 _messagesDown;
    QTimer *_monitorTimer;
    QTime _lastReceiveTime;
    QTime _lastSendTime;
    QTime _lastQosSendTime;

    inline void setStatus(Status status);
    inline void setPeerAddress(Channel::SocketAddress address);
    inline bool sendMessage(QByteArray message, MESSAGE_TYPE type);
    bool sendMessage(QByteArray message, MESSAGE_TYPE type, MESSAGE_ID ID);
    void close(Status status);
    inline bool compareHandshake(QByteArray message);
    inline void sendHandshake();
    inline void sendHeartbeat();
    void resetConnection();
    void processBufferedMessage(MESSAGE_TYPE type, MESSAGE_ID ID,
                                QByteArray message, SocketAddress address);
    void configureNewTcpSocket();
    void resetConnectionVars();
    void init();
    bool parseBoolString(QString string, bool* value);

private slots:
    void routeMessage(Channel *sender, QByteArray message);
    void udpReadyRead();
    void tcpReadyRead();
    void monitorTick();
    void tcpConnected();
    void newTcpClient();
    void socketError(QAbstractSocket::SocketError err);
    void serverError(QAbstractSocket::SocketError err);

signals:
    void messageReceived(Channel *channel, QByteArray message);
    void statusChanged(Channel *channel, Channel::Status status);
    void peerAddressChanged(Channel *channel, Channel::SocketAddress peerAddress);
    void qosUpdate(Channel *channel, int rtt, quint64 msg_up, quint64 msg_down);
};

#endif // CHANNEL_H
