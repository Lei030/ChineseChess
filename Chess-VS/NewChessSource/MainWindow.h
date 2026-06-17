#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include "Chess.h"
#include "AI.h"
#include "NetworkManager.h"
#include "BoardWidget.h"
#include <QListWidget>
#include <QMediaPlayer>
#include <QAudioOutput>

enum GameMode { MODE_MENU, MODE_PVE, MODE_PVP_NET };

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // 菜单槽函数
    void onBtnPVEEasyClicked();
    void onBtnPVEHardClicked();
    void onBtnHostClicked();
    void onBtnJoinClicked();

    // 游戏控制槽函数
    void onBtnUndoClicked();
    void onBtnRestartClicked();
    void onBtnMenuClicked();
    // 注意：这里删除了 onBtnMusicClicked

    // 游戏逻辑槽函数
    void onPlayerTriedMove(int fr, int fc, int tr, int tc);
    void onAIMoveTimer();

    // 网络槽函数
    void onNetMoveReceived(int fr, int fc, int tr, int tc);
    void onNetConnected();
    void onNetError(QString msg);

private:
    // UI初始化
    void initUI();
    void setupConnections();
    void updateSidePanel();

    // 【关键修复】确保这个函数没有被注释掉！
    void initAudio();

    // 游戏控制
    void initGame(GameMode mode, char localType);
    void checkGameOverUI();
    void endGameProcessing();

    // 核心成员
    Chess m_chess;
    AI m_ai;
    NetworkManager m_net;

    // UI组件
    QStackedWidget* m_stackedWidget;
    QWidget* m_menuPage;
    QWidget* m_gamePage;
    BoardWidget* m_boardWidget;
    QLabel* m_infoLabel;
    QLabel* m_timerLabel;

    // 【关键修复】确保这个按钮没有被注释掉！
    QPushButton* m_btnUndo;

    // 游戏状态
    QTimer* m_aiTimer;
    GameMode m_currentMode;
    char m_localPlayerType;
    bool m_isHumanTurn;
    AIDifficulty m_currentAIDifficulty;
    int m_totalMoves;

    // 音频相关 (只保留音效，BGM已删除)
    QMediaPlayer* m_effectPlayer;
    QAudioOutput* m_effectOutput;
};