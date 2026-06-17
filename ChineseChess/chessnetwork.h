// chessnetwork.h
#ifndef CHESSNETWORK_H
#define CHESSNETWORK_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QPoint>

class ChessNetwork : public QObject
{
    Q_OBJECT
public:
    explicit ChessNetwork(QObject *parent = nullptr);

    // 作为服务端：启动监听
    bool startServer(int port = 8888);
    // 作为客户端：连接服务器
    bool connectToServer(QString ip, int port = 8888);
    // 发送走棋数据
    void sendMove(QPoint src, QPoint dst);

signals:
    // 接收到走棋数据
    void moveReceived(QPoint src, QPoint dst);
    // 连接状态变化
    void connected();
    void disconnected();

private slots:
    void onNewConnection();        // 服务端新连接
    void onReadyRead();            // 接收数据
    void onSocketDisconnected();   // 断开连接

private:
    QTcpServer *m_server;          // 服务端
    QTcpSocket *m_socket;          // 通信套接字
    QHostAddress m_localIP;        // 本地IP
};

#endif // CHESSNETWORK_H
