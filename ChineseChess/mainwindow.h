// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include "chessboard.h"
#include "chessnetwork.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 菜单操作
    void onSinglePlayer();    // 单人自走
    void onEasyAI();          // 初级人机
    void onHardAI();          // 高级人机
    void onLanServer();       // 局域网服务端
    void onLanClient();       // 局域网客户端
    void onRestart();         // 重新开始
    // 网络走棋
    void onMoveReceived(QPoint src, QPoint dst);

private:
    void initUI();            // 初始化UI

    ChessBoard *m_chessBoard; // 棋盘控件
    ChessNetwork *m_network;  // 网络模块
    bool m_isServer = false;  // 是否为服务端
};

#endif // MAINWINDOW_H
