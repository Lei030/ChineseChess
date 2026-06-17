// chessboard.h
#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include "chessman.h"

class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(QWidget *parent = nullptr);

    // 游戏模式
    enum GameMode {
        SinglePlayer,       // 单人自走
        EasyAI,             // 初级人机
        HardAI,             // 高级人机
        LanOnline           // 局域网联机
    };

    // 初始化棋盘（重置游戏）
    void initBoard();

    // 设置游戏模式
    void setGameMode(GameMode mode) { m_gameMode = mode; }

    // 走棋（src->dst，返回是否走棋成功）
    bool moveChess(QPoint src, QPoint dst);

    // AI 走棋（返回走棋步骤：src->dst）
    QPair<QPoint, QPoint> aiMove(bool isHard = false);

protected:
    // 绘制棋盘和棋子
    void paintEvent(QPaintEvent *event) override;
    // 鼠标交互
    void mousePressEvent(QMouseEvent *event) override;

private:
    // 检查走棋规则（核心：判断某步棋是否合法）
    bool checkMoveRule(ChessMan *chess, QPoint dst);
    // 坐标转换：窗口像素坐标 -> 棋盘逻辑坐标
    QPoint pixelToLogic(QPoint pixelPos);
    // 坐标转换：棋盘逻辑坐标 -> 窗口像素坐标
    QPoint logicToPixel(QPoint logicPos);
    // 检查是否将军/胜负
    bool checkWin(ChessColor color);

    // 棋盘数据
    QVector<QVector<ChessMan*>> m_board;  // 10x9 棋盘（x:0-9, y:0-8）
    ChessMan *m_selectedChess = nullptr;   // 选中的棋子
    QPoint m_selectedPos;                 // 选中的位置
    ChessColor m_currentTurn = Red;       // 当前回合（红先黑后）
    GameMode m_gameMode = SinglePlayer;   // 当前游戏模式
    int m_cellSize = 60;                  // 棋盘格子大小（像素）
};

#endif // CHESSBOARD_H
