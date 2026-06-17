// mainwindow.cpp
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_chessBoard = new ChessBoard(this);
    m_network = new ChessNetwork(this);

    // 绑定网络信号
    connect(m_network, &ChessNetwork::moveReceived, this, &MainWindow::onMoveReceived);
    connect(m_network, &ChessNetwork::connected, [](){
        QMessageBox::information(nullptr, "提示", "连接成功！");
    });
    connect(m_network, &ChessNetwork::disconnected, [](){
        QMessageBox::warning(nullptr, "提示", "连接断开！");
    });

    initUI();
}

MainWindow::~MainWindow()
{
    delete m_chessBoard;
    delete m_network;
}

void MainWindow::initUI()
{
    // 设置窗口
    setWindowTitle("中国象棋 - Qt C++");
    setMinimumSize(m_chessBoard->minimumSize());//根据棋盘大小自动适配
    // 创建菜单栏
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *gameMenu = menuBar->addMenu("游戏");

    // 游戏模式菜单项
    QAction *singleAction = new QAction("单人自走", this);
    QAction *easyAIAction = new QAction("初级人机对战", this);
    QAction *hardAIAction = new QAction("高级人机对战", this);
    QAction *lanServerAction = new QAction("局域网-作为主机", this);
    QAction *lanClientAction = new QAction("局域网-加入游戏", this);
    QAction *restartAction = new QAction("重新开始", this);

    // 绑定菜单事件
    connect(singleAction, &QAction::triggered, this, &MainWindow::onSinglePlayer);
    connect(easyAIAction, &QAction::triggered, this, &MainWindow::onEasyAI);
    connect(hardAIAction, &QAction::triggered, this, &MainWindow::onHardAI);
    connect(lanServerAction, &QAction::triggered, this, &MainWindow::onLanServer);
    connect(lanClientAction, &QAction::triggered, this, &MainWindow::onLanClient);
    connect(restartAction, &QAction::triggered, this, &MainWindow::onRestart);

    gameMenu->addAction(singleAction);
    gameMenu->addAction(easyAIAction);
    gameMenu->addAction(hardAIAction);
    gameMenu->addSeparator();
    gameMenu->addAction(lanServerAction);
    gameMenu->addAction(lanClientAction);
    gameMenu->addSeparator();
    gameMenu->addAction(restartAction);

    setMenuBar(menuBar);

    // 设置中心控件
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(m_chessBoard);
    setCentralWidget(centralWidget);
}

void MainWindow::onSinglePlayer()
{
    m_chessBoard->setGameMode(ChessBoard::SinglePlayer);
    m_chessBoard->initBoard();
}

void MainWindow::onEasyAI()
{
    m_chessBoard->setGameMode(ChessBoard::EasyAI);
    m_chessBoard->initBoard();
}

void MainWindow::onHardAI()
{
    m_chessBoard->setGameMode(ChessBoard::HardAI);
    m_chessBoard->initBoard();
}

void MainWindow::onLanServer()
{
    m_isServer = true;
    if (m_network->startServer(8888)) {
        QMessageBox::information(this, "提示", "服务端已启动，等待客户端连接...");
        m_chessBoard->setGameMode(ChessBoard::LanOnline);
        m_chessBoard->initBoard();
    } else {
        QMessageBox::critical(this, "错误", "服务端启动失败！");
    }
}

void MainWindow::onLanClient()
{
    m_isServer = false;
    QString ip = QInputDialog::getText(this, "连接服务器", "请输入服务器IP地址：");
    if (ip.isEmpty()) {
        return;
    }

    if (m_network->connectToServer(ip, 8888)) {
        m_chessBoard->setGameMode(ChessBoard::LanOnline);
        m_chessBoard->initBoard();
    } else {
        QMessageBox::critical(this, "错误", "连接服务器失败！");
    }
}

void MainWindow::onRestart()
{
    m_chessBoard->initBoard();
}

void MainWindow::onMoveReceived(QPoint src, QPoint dst)
{
    // 服务端控制红方，客户端控制黑方，仅处理对方的走棋
    if ((m_isServer && m_chessBoard->moveChess(src, dst)) ||
        (!m_isServer && m_chessBoard->moveChess(src, dst))) {
        m_chessBoard->update();
    }
}
