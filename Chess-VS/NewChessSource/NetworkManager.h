#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket> 
#include <QTimer>    

// 网络传输的走棋数据包结构定义
// 使用 #pragma pack(1) 确保结构体按1字节对齐，避免不同平台/编译器间的内存对齐问题
#pragma pack(push, 1)
struct NetMoveMsg {
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
};
#pragma pack(pop)

class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();

    // 判断当前是否为主机模式
    bool isServer() const { return m_isServer; }
    // 判断是否处于连接状态
    bool isConnected() const { return m_socket && m_socket->state() == QAbstractSocket::ConnectedState; }
    static QStringList getLocalIPs();

signals:
    // 信号定义
    void clientConnected(); // 客户端连接成功（仅主机端发）
    void disconnected();    // 连接断开
    void moveReceived(int fr, int fc, int tr, int tc); // 收到对手走棋
    void errorOccurred(QString msg); // 发生错误
    void roomFound(QString ip, QString name);

public slots:
    // 启动主机监听
    void startHost(quint16 port = 27015);
    // 连接到主机
    void connectToHost(QString ip, quint16 port = 27015);
    // 发送走棋数据
    void sendMove(int fr, int fc, int tr, int tc);
    // 关闭网络连接
    void closeNetwork();
    void startDiscovery();
    // 【新增】停止搜索
    void stopDiscovery();

private slots:
    // 内部槽函数
    void onNewConnection(); // 处理新连接
    void onReadyRead();     // 处理数据接收
    void onSocketError(QAbstractSocket::SocketError socketError); // 处理Socket错误
    // 【新增】发送广播（主机用）
    void sendBroadcast();
    // 【新增】接收广播（客机用）
    void onBroadcastReceived();

private:
    QTcpServer* m_server = nullptr;
    QTcpSocket* m_socket = nullptr;
    bool m_isServer = false;
    // 【新增】UDP 广播相关
    QUdpSocket* m_udpSocket = nullptr;
    QTimer* m_broadcastTimer = nullptr;
    quint16 m_broadcastPort = 45454; // 定义一个广播端口
};