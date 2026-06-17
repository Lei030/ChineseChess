#include "NetworkManager.h"
#include <QHostAddress>
#include <QNetworkInterface> // 用于获取本机IP

NetworkManager::NetworkManager(QObject* parent) : QObject(parent) {
    m_udpSocket = new QUdpSocket(this);
    m_broadcastTimer = new QTimer(this);
    connect(m_broadcastTimer, &QTimer::timeout, this, &NetworkManager::sendBroadcast);
}

NetworkManager::~NetworkManager() {
    closeNetwork();
}

// 获取本机 IP 列表（过滤掉 localhost）
QStringList NetworkManager::getLocalIPs() {
    QStringList ipList;
    const QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for (const QHostAddress& address : addresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::LocalHost) {
            ipList << address.toString();
        }
    }
    return ipList;
}

// 启动主机
void NetworkManager::startHost(quint16 port) {
    closeNetwork();
    m_isServer = true;
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);

    if (!m_server->listen(QHostAddress::Any, port)) {
        emit errorOccurred("无法启动监听: " + m_server->errorString());
        closeNetwork();
        return;
    }

    // 【新增】启动 UDP 广播，每 1 秒发一次
    m_broadcastTimer->start(1000);
}

// 【新增】主机发送广播
void NetworkManager::sendBroadcast() {
    QByteArray datagram = "ChineseChessRoom"; // 广播内容，可以加上房间名
    // 向局域网广播地址 255.255.255.255 发送
    m_udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, m_broadcastPort);
}

// 连接到主机
void NetworkManager::connectToHost(QString ip, quint16 port) {
    closeNetwork(); // 先清理旧连接
    m_isServer = false;
    m_socket = new QTcpSocket(this);
    // 连接Socket信号
    connect(m_socket, &QTcpSocket::connected, this, &NetworkManager::clientConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    // 使用 QOverload 处理重载的 errorOccurred 信号
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
        this, &NetworkManager::onSocketError);

    // 开始连接
    m_socket->connectToHost(ip, port);
}

// 【新增】客机开始搜索
void NetworkManager::startDiscovery() {
    // 绑定 UDP 端口，允许地址重用（ShareAddress），这样同一台电脑开多个客户端也能收到
    m_udpSocket->bind(m_broadcastPort, QUdpSocket::ShareAddress);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::onBroadcastReceived);
}

// 【新增】客机停止搜索
void NetworkManager::stopDiscovery() {
    m_udpSocket->close();
    m_udpSocket->disconnect(this);
}

// 【新增】客机处理接收到的广播
void NetworkManager::onBroadcastReceived() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(m_udpSocket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;

        m_udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        // 验证广播内容是否正确
        if (datagram == "ChineseChessRoom") {
            // 过滤掉自己发出的广播（如果在同一台电脑测试）
            QStringList localIPs = getLocalIPs();
            if (!localIPs.contains(sender.toString())) {
                // 发出信号：发现了一个房间 (IP地址, 默认名字)
                emit roomFound(sender.toString(), "局域网对战");
            }
        }
    }
}

// 关闭网络时记得停止广播
void NetworkManager::closeNetwork() {
    if (m_broadcastTimer->isActive()) m_broadcastTimer->stop();
    m_udpSocket->close();
    // ... 原有的关闭 TCP 逻辑 ...
    if (m_server) { m_server->close(); m_server->deleteLater(); m_server = nullptr; }
    if (m_socket) {
        m_socket->disconnect(this);
        m_socket->abort();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

// 处理新连接（主机端）
void NetworkManager::onNewConnection() {
    if (!m_server) return;
    // 获取连接的Socket
    m_socket = m_server->nextPendingConnection();
    if (!m_socket) return;

    // 连接Socket信号
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    // 连接错误信号
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
        this, &NetworkManager::onSocketError);

    // 停止监听，只接受这一个对战连接
    m_server->close();
    emit clientConnected();
}

// 发送走棋数据
void NetworkManager::sendMove(int fr, int fc, int tr, int tc) {
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        NetMoveMsg msg = { fr, fc, tr, tc };
        // 直接写入结构体数据
        m_socket->write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        m_socket->flush(); // 确保数据立即发送
    }
}

// 接收数据
void NetworkManager::onReadyRead() {
    if (!m_socket) return;
    // 循环读取所有完整的数据包
    while (m_socket->bytesAvailable() >= (qint64)sizeof(NetMoveMsg)) {
        NetMoveMsg msg;
        m_socket->read(reinterpret_cast<char*>(&msg), sizeof(msg));
        // 发出收到走棋信号
        emit moveReceived(msg.fromRow, msg.fromCol, msg.toRow, msg.toCol);
    }
}

// Socket错误处理
void NetworkManager::onSocketError(QAbstractSocket::SocketError) {
    if (m_socket) {
        emit errorOccurred(m_socket->errorString());
        // 发生错误后断开连接
        m_socket->disconnectFromHost();
    }
}

