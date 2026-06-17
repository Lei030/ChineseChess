// chessai.h
#ifndef CHESSAI_H
#define CHESSAI_H

#include <QVector>
#include <QPoint>
#include <QPair>
#include "chessman.h"

class ChessAI
{
public:
    ChessAI(QVector<QVector<ChessMan*>> board, ChessColor color);

    // 获取最佳走棋（isHard：true=高级AI，false=初级AI）
    QPair<QPoint, QPoint> getBestMove(bool isHard);

private:
    // 初级AI：随机生成合法走棋
    QPair<QPoint, QPoint> randomMove();
    // 高级AI：简单评分算法（优先吃子、保帅、占关键位置）
    QPair<QPoint, QPoint> scoreBasedMove();
    // 计算棋子评分
    int getChessScore(ChessType type);
    // 检查走棋是否合法（复用棋盘的规则，简化版）
    bool isMoveValid(QPoint src, QPoint dst);

    QVector<QVector<ChessMan*>> m_board;  // 棋盘数据
    ChessColor m_color;                   // AI 控制的颜色（黑方）
};

#endif // CHESSAI_H
