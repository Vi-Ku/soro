#ifndef VIDEOCLIENT_H
#define VIDEOCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QList>

#include "soro_global.h"
#include "channel.h"
#include "videoencoding.h"

namespace Soro {
namespace MissionControl {

class VideoClient : public QObject {
    Q_OBJECT
public:
    enum State {
        ConnectingState,
        ConnectedState,
        StreamingState
    };

    explicit VideoClient(QString name, SocketAddress server, QHostAddress host, Logger *log = 0, QObject *parent = 0);

    void addForwardingAddress(SocketAddress address);
    void removeForwardingAddress(SocketAddress address);

    /* Gets the format of the stream currently being received
     */
    const StreamFormat* getStreamFormat();

    VideoClient::State getState();

signals:
    void stateChanged(VideoClient::State state);
    void serverEos();
    void serverError(QString description);

private:
    char _buffer[65536];
    QString _name;
    SocketAddress _server;
    Logger *_log;
    State _state = ConnectingState;
    StreamFormat *_streamFormat = NULL;
    QUdpSocket *_videoSocket;
    Channel *_controlChannel;
    int _punchTimerId = TIMER_INACTIVE;
    QList<SocketAddress> _forwardAddresses;

    void setState(State state);

private slots:
    void controlMessageReceived(const char *message, Channel::MessageSize size);
    void videoSocketReadyRead();
    void controlChannelStateChanged(Channel::State state);

protected:
    void timerEvent(QTimerEvent *e);
};

} // namespace MissionControl
} // namespace Soro

#endif // VIDEOCLIENT_H