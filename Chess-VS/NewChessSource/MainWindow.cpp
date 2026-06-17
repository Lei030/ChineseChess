
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

// 构造函数
MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent), m_ai(m_chess) {

    initUI();    // 初始化界面
    initAudio(); // 初始化音频 (只保留音效)

    // 配置AI定时器为单次触发
    m_aiTimer = new QTimer(this);
    m_aiTimer->setSingleShot(true);
    setupConnections();

    // 初始状态设置
    m_stackedWidget->setCurrentWidget(m_menuPage);
    m_currentMode = MODE_MENU;
}

MainWindow::~MainWindow() {}

void MainWindow::setupConnections() {
    // 核心连接：棋盘操作 -> 主窗口逻辑处理
    connect(m_boardWidget, &BoardWidget::playerTriedMove, this, &MainWindow::onPlayerTriedMove);

    // AI定时器
    connect(m_aiTimer, &QTimer::timeout, this, &MainWindow::onAIMoveTimer);

    // 网络信号
    connect(&m_net, &NetworkManager::moveReceived, this, &MainWindow::onNetMoveReceived);
    connect(&m_net, &NetworkManager::clientConnected, this, &MainWindow::onNetConnected);
    connect(&m_net, &NetworkManager::errorOccurred, this, &MainWindow::onNetError);
    // 网络断开时直接返回菜单
    connect(&m_net, &NetworkManager::disconnected, this, &MainWindow::onBtnMenuClicked);
}

// 初始化音频系统
void MainWindow::initAudio() {
    // 【已删除】背景音乐 BGM 相关代码...

    // --- 配置将军音效 (保留) ---
    m_effectPlayer = new QMediaPlayer(this);
    m_effectOutput = new QAudioOutput(this);
    m_effectPlayer->setAudioOutput(m_effectOutput);

    m_effectPlayer->setSource(QUrl("qrc:/check.aac"));
    m_effectOutput->setVolume(1.0); // 音效保持响亮
}

// 初始化界面 UI
void MainWindow::initUI() {
    setWindowTitle("中国象棋");
    setFixedSize(1000, 760);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    // 阴影颜色：冷灰色
    auto addShadow = [](QWidget* widget, QColor color = QColor(60, 70, 90, 40), int blur = 15, int offset = 4) {
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(blur);
        shadow->setOffset(0, offset);
        shadow->setColor(color);
        widget->setGraphicsEffect(shadow);
        };

    // 全局背景：蓝白渐变
    QString globalBgStyle = "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #F0F9FF, stop:1 #B3E5FC);";

    // --- 1. 菜单页面  ---
    m_menuPage = new QWidget();
    m_menuPage->setStyleSheet(globalBgStyle);
    QVBoxLayout* menuLayout = new QVBoxLayout(m_menuPage);

    QLabel* title = new QLabel("中国象棋");
    title->setFont(QFont("Microsoft YaHei", 60, QFont::Bold));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #0277BD; background: transparent; margin-bottom: 20px;");

    auto createPrimaryBtn = [&](const QString& text) {
        QPushButton* btn = new QPushButton(text);
        btn->setFixedSize(300, 60);
        btn->setFont(QFont("Microsoft YaHei", 18, QFont::Bold));
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { background-color: #29B6F6; color: white; border-radius: 30px; border: none; }"
            "QPushButton:hover { background-color: #03A9F4; }"
            "QPushButton:pressed { background-color: #039BE5; transform: scale(0.98); }"
        );
        addShadow(btn, QColor(3, 169, 244, 80));
        return btn;
        };

    auto createSecondaryBtn = [&](const QString& text) {
        QPushButton* btn = new QPushButton(text);
        btn->setFixedSize(300, 60);
        btn->setFont(QFont("Microsoft YaHei", 18));
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { background-color: #FFFFFF; color: #0277BD; border-radius: 15px; border: 1px solid #B3E5FC; }"
            "QPushButton:hover { background-color: #E1F5FE; }"
            "QPushButton:pressed { background-color: #B3E5FC; }"
        );
        addShadow(btn, QColor(2, 119, 189, 30));
        return btn;
        };

    QPushButton* btnPVEEasy = createPrimaryBtn("简单人机 ");
    QPushButton* btnPVEHard = createPrimaryBtn("困难人机 ");
    QPushButton* btnHost = createSecondaryBtn("创建主机 (执红)");
    QPushButton* btnJoin = createSecondaryBtn("连接主机 (执黑)");

    connect(btnPVEEasy, &QPushButton::clicked, this, &MainWindow::onBtnPVEEasyClicked);
    connect(btnPVEHard, &QPushButton::clicked, this, &MainWindow::onBtnPVEHardClicked);
    connect(btnHost, &QPushButton::clicked, this, &MainWindow::onBtnHostClicked);
    connect(btnJoin, &QPushButton::clicked, this, &MainWindow::onBtnJoinClicked);

    menuLayout->addStretch(1);
    menuLayout->addWidget(title);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(btnPVEEasy, 0, Qt::AlignCenter);
    menuLayout->addSpacing(15);
    menuLayout->addWidget(btnPVEHard, 0, Qt::AlignCenter);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(btnHost, 0, Qt::AlignCenter);
    menuLayout->addSpacing(15);
    menuLayout->addWidget(btnJoin, 0, Qt::AlignCenter);
    menuLayout->addStretch(1);

    // --- 2. 游戏页面 ---
    m_gamePage = new QWidget();
    m_gamePage->setStyleSheet(globalBgStyle);
    QHBoxLayout* gameLayout = new QHBoxLayout(m_gamePage);
    gameLayout->setContentsMargins(30, 30, 30, 30);

    m_boardWidget = new BoardWidget();
    m_boardWidget->setChessRef(&m_chess);
    addShadow(m_boardWidget, QColor(40, 60, 80, 50), 30, 10);

    // 侧边控制栏
    QWidget* sidePanel = new QWidget();
    sidePanel->setStyleSheet("background-color: #FFFFFF; border-radius: 24px; border: 1px solid #E1F5FE;");
    addShadow(sidePanel, QColor(40, 60, 80, 20), 40, 10);

    QVBoxLayout* sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setContentsMargins(20, 30, 20, 30);

    m_infoLabel = new QLabel("红方回合");
    m_infoLabel->setFont(QFont("Microsoft YaHei", 26, QFont::Bold));
    m_infoLabel->setAlignment(Qt::AlignCenter);
    m_infoLabel->setStyleSheet("color: #37474F; border: none; background: transparent;");

    m_timerLabel = new QLabel("步数: 0");
    m_timerLabel->setFont(QFont("Microsoft YaHei", 16));
    m_timerLabel->setAlignment(Qt::AlignCenter);
    m_timerLabel->setStyleSheet("color: #78909C; border: none; background: transparent;");

    // 游戏内按钮样式生成器
    auto createGameBtn = [&](const QString& text, QString color) {
        QPushButton* btn = new QPushButton(text);
        btn->setFixedSize(160, 55);
        btn->setFont(QFont("Microsoft YaHei", 16, QFont::Bold));
        btn->setCursor(Qt::PointingHandCursor);
        QString style = QString(
            "QPushButton { background-color: #F0F8FF; color: %1; border-radius: 16px; border: 1px solid #B3E5FC; }"
            "QPushButton:hover { background-color: #E1F5FE; }"
            "QPushButton:pressed { background-color: #B3E5FC; }"
            "QPushButton:disabled { color: #CFD8DC; border-color: #ECEFF1; }"
        ).arg(color);
        btn->setStyleSheet(style);
        return btn;
        };

    m_btnUndo = createGameBtn("悔  棋", "#039BE5");
    QPushButton* btnRestart = createGameBtn("重新开始", "#039BE5");

    // 【已删除】音乐开关按钮

    QPushButton* btnMenu = createGameBtn("退  出", "#E57373");

    connect(m_btnUndo, &QPushButton::clicked, this, &MainWindow::onBtnUndoClicked);
    connect(btnRestart, &QPushButton::clicked, this, &MainWindow::onBtnRestartClicked);
    connect(btnMenu, &QPushButton::clicked, this, &MainWindow::onBtnMenuClicked);

    sideLayout->addWidget(m_infoLabel);
    sideLayout->addSpacing(10);
    sideLayout->addWidget(m_timerLabel);
    sideLayout->addStretch(1);

    sideLayout->addWidget(m_btnUndo, 0, Qt::AlignCenter);
    sideLayout->addSpacing(15);
    sideLayout->addWidget(btnRestart, 0, Qt::AlignCenter);
    sideLayout->addSpacing(40); // 调整间距

    // 【已删除】音乐按钮布局

    sideLayout->addWidget(btnMenu, 0, Qt::AlignCenter);
    sideLayout->addStretch(1);

    gameLayout->addWidget(m_boardWidget);
    gameLayout->addSpacing(40);
    gameLayout->addWidget(sidePanel);

    m_stackedWidget->addWidget(m_menuPage);
    m_stackedWidget->addWidget(m_gamePage);
}
// 初始化游戏状态
void MainWindow::initGame(GameMode mode, char localType) {
    m_currentMode = mode;
    m_localPlayerType = localType;
    m_totalMoves = 0;
    m_chess.reset();

    m_boardWidget->setPVEMode(mode == MODE_PVE);
    m_isHumanTurn = (m_chess.getCurrentTurn() == m_localPlayerType);
    updateSidePanel();

    m_stackedWidget->setCurrentWidget(m_gamePage);
    m_boardWidget->update();

    // 【已删除】背景音乐播放逻辑

    if (mode == MODE_PVE && !m_isHumanTurn) {
        m_aiTimer->start(500);
    }
}

// 玩家尝试走棋
void MainWindow::onPlayerTriedMove(int fr, int fc, int tr, int tc) {
    if (!m_isHumanTurn) return;
    if (m_chess.getPiece(fr, fc).type != m_localPlayerType) return;

    if (m_chess.moveChess(fr, fc, tr, tc)) {
        m_boardWidget->update();
        m_isHumanTurn = false;
        m_totalMoves++;
        updateSidePanel();

        m_chess.checkGameOver(); // 检查状态

        if (m_currentMode == MODE_PVP_NET) {
            m_net.sendMove(fr, fc, tr, tc);
        }
        else if (m_currentMode == MODE_PVE) {
            if (!m_chess.isGameOverFlag()) {
                m_aiTimer->start(500);
            }
        }
        checkGameOverUI();
    }
    else {
        m_chess.cancelSelect();
        m_boardWidget->update();
    }
}

void MainWindow::updateSidePanel() {
    QString turnStr = (m_chess.getCurrentTurn() == 'R' ? "红方" : "黑方");
    m_infoLabel->setText("当前回合: " + turnStr);

    // 将军音效逻辑 (保留)
    if (m_chess.isCheck() && !m_chess.isGameOverFlag()) {
        m_effectPlayer->stop();
        m_effectPlayer->play();

        m_infoLabel->setStyleSheet("color: #E53935; font-weight: bold;"); // 红色警告
        m_infoLabel->setText(turnStr + " 被将军!");
    }
    else {
        m_infoLabel->setStyleSheet(m_chess.getCurrentTurn() == 'R' ? "color: #37474F;" : "color: #263238;");
    }

    int round = m_totalMoves / 2 + 1;
    if (m_totalMoves == 0) round = 1;
    m_timerLabel->setText(QString("第 %1 回合").arg(round));

    if (m_currentMode == MODE_PVP_NET || m_chess.isGameOverFlag()) {
        m_btnUndo->setEnabled(false);
    }
    else {
        m_btnUndo->setEnabled(m_chess.canUndo());
    }
}

// AI 走棋逻辑
void MainWindow::onAIMoveTimer() {
    if (m_chess.getCurrentTurn() != 'B') {
        qDebug() << "AI抢跑拦截";
        m_aiTimer->stop();
        m_isHumanTurn = (m_chess.getCurrentTurn() == m_localPlayerType);
        return;
    }

    bool aiMoved = m_ai.aiMove(m_currentAIDifficulty);

    if (aiMoved) {
        m_boardWidget->update();
        m_isHumanTurn = true;
        updateSidePanel();
        m_chess.checkGameOver();
        checkGameOverUI();
    }
    else {
        m_aiTimer->stop();
        QMessageBox::information(this, "游戏结束", "电脑无棋可走，红方获胜！");
        endGameProcessing();
    }
}

// 槽函数实现
void MainWindow::onBtnPVEEasyClicked() {
    m_ai.setAIType('B');
    m_currentAIDifficulty = AI_EASY;
    initGame(MODE_PVE, 'R');
}

void MainWindow::onBtnPVEHardClicked() {
    m_ai.setAIType('B');
    m_currentAIDifficulty = AI_HARD;
    initGame(MODE_PVE, 'R');
}

void MainWindow::onBtnHostClicked() {
    m_net.startHost();
    QMessageBox::information(this, "等待连接", "正在监听端口 27015...\n同时已发送局域网广播。");
}

void MainWindow::onBtnJoinClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle("连接主机");
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QListWidget* list = new QListWidget;
    layout->addWidget(new QLabel("正在搜索局域网主机..."));
    layout->addWidget(list);

    QPushButton* btnManual = new QPushButton("手动输入IP");
    layout->addWidget(btnManual);

    connect(&m_net, &NetworkManager::roomFound, list, [&](QString ip, QString name) {
        list->addItem(name + " (" + ip + ")");
        });

    connect(list, &QListWidget::itemDoubleClicked, [&](QListWidgetItem* item) {
        QString text = item->text();
        int start = text.indexOf('(');
        int end = text.indexOf(')');
        if (start != -1 && end != -1) {
            m_net.connectToHost(text.mid(start + 1, end - start - 1));
            dialog.accept();
        }
        });

    connect(btnManual, &QPushButton::clicked, [&]() {
        bool ok;
        QString ip = QInputDialog::getText(&dialog, "输入IP", "IP:", QLineEdit::Normal, "", &ok);
        if (ok && !ip.isEmpty()) {
            m_net.connectToHost(ip);
            dialog.accept();
        }
        });

    m_net.startDiscovery();
    dialog.exec();
    m_net.stopDiscovery();
}

void MainWindow::onNetConnected() {
    if (m_net.isServer()) initGame(MODE_PVP_NET, 'R');
    else initGame(MODE_PVP_NET, 'B');
    QMessageBox::information(this, "成功", "连接成功，对战开始！");
}

void MainWindow::onNetError(QString msg) {
    QMessageBox::critical(this, "网络错误", msg);
    onBtnMenuClicked();
}

void MainWindow::onNetMoveReceived(int fr, int fc, int tr, int tc) {
    m_chess.moveChess(fr, fc, tr, tc);
    m_boardWidget->update();
    m_isHumanTurn = true;
    m_totalMoves++;
    updateSidePanel();
    m_chess.checkGameOver();
    checkGameOverUI();
}

void MainWindow::checkGameOverUI() {
    if (m_chess.isGameOverFlag()) {
        endGameProcessing();
        QString winner = (m_chess.getWinner() == 'R' ? "红方" : "黑方");
        QMessageBox::information(this, "游戏结束", winner + "获胜！\n祝贺！");
    }
}

void MainWindow::endGameProcessing() {
    m_aiTimer->stop();
    m_isHumanTurn = false;
    updateSidePanel();
    // 【已删除】停止背景音乐逻辑
}

void MainWindow::onBtnRestartClicked() {
    if (m_currentMode == MODE_PVP_NET) {
        QMessageBox::information(this, "提示", "联机模式请返回菜单重新连接。");
        return;
    }
    m_aiTimer->stop();
    initGame(m_currentMode, m_localPlayerType);
}

void MainWindow::onBtnUndoClicked() {
    m_aiTimer->stop();
    if (m_currentMode == MODE_PVP_NET) return;

    if (m_currentMode == MODE_PVE) {
        if (m_isHumanTurn) { // 玩家回合悔棋
            if (m_chess.getHistorySize() >= 2) {
                m_chess.undoLastMove();
                m_chess.undoLastMove();
                m_totalMoves -= 2;
            }
        }
        else { // AI思考时悔棋
            if (m_chess.canUndo()) {
                m_chess.undoLastMove();
                m_totalMoves -= 1;
                m_isHumanTurn = true;
            }
        }
    }
    m_chess.cancelSelect();
    m_boardWidget->update();
    updateSidePanel();
}

void MainWindow::onBtnMenuClicked() {
    m_aiTimer->stop();
    // 【已删除】停止背景音乐逻辑

    const bool wasBlocked = m_net.blockSignals(true);
    m_net.closeNetwork();
    m_net.blockSignals(wasBlocked);

    m_stackedWidget->setCurrentWidget(m_menuPage);
    m_currentMode = MODE_MENU;
}

// 【已删除】onBtnMusicClicked 函数