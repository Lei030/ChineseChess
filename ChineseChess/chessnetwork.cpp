// chessnetwork.cpp
#include "chessnetwork.h"
#include <QDataStream>

ChessNetwork::ChessNetwork(QObject *parent) : QObject(parent)
{
    m_server = new QTcpServer(this);
    m_socket = nullptr;

    // 绑定服务端新连接信号
    connect(m_server, &QTcpServer::newConnection, this, &ChessNetwork::onNewConnection);
}

bool ChessNetwork::startServer(int port)
{
    // 监听本地所有IP的指定端口
    if (m_server->listen(QHostAddress::Any, port)) {
        m_localIP = m_server->serverAddress();
        return true;
    }
    return false;
}

bool ChessNetwork::connectToServer(QString ip, int port)
{
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
    }

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &ChessNetwork::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ChessNetwork::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::connected, this, &ChessNetwork::connected);

    // 连接服务器
    m_socket->connectToHost(ip, port);
    return m_socket->waitForConnected(3000);
}

void ChessNetwork::sendMove(QPoint src, QPoint dst)
{
    if (!m_socket || m_socket->state() != QTcpSocket::ConnectedState) {
        return;
    }

    // 序列化走棋数据（src.x, src.y, dst.x, dst.y）
    QDataStream stream(m_socket);
    stream << src.x() << src.y() << dst.x() << dst.y();
}

void ChessNetwork::onNewConnection()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
    }

    // 接受客户端连接
    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &ChessNetwork::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ChessNetwork::onSocketDisconnected);
    emit connected();
}

void ChessNetwork::onReadyRead()
{
    if (!m_socket) {
        return;
    }

    // 反序列化走棋数据
    QDataStream stream(m_socket);
    int srcX, srcY, dstX, dstY;
    stream >> srcX >> srcY >> dstX >> dstY;

    emit moveReceived(QPoint(srcX, srcY), QPoint(dstX, dstY));
}

void ChessNetwork::onSocketDisconnected()
{
    emit disconnected();
}
