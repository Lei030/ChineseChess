// chessai.cpp
#include "chessai.h"
#include <QRandomGenerator>
#include <QVector>

ChessAI::ChessAI(QVector<QVector<ChessMan*>> board, ChessColor color)
    : m_board(board), m_color(color)
{}

QPair<QPoint, QPoint> ChessAI::getBestMove(bool isHard)
{
    if (isHard) {
        return scoreBasedMove();
    } else {
        return randomMove();
    }
}

QPair<QPoint, QPoint> ChessAI::randomMove()
{
    // 收集所有合法走棋
    QVector<QPair<QPoint, QPoint>> validMoves;
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 9; y++) {
            ChessMan *chess = m_board[x][y];
            if (chess->type() == None || chess->color() != m_color) {
                continue;
            }

            // 遍历所有可能的目标位置
            for (int dx = 0; dx < 10; dx++) {
                for (int dy = 0; dy < 9; dy++) {
                    QPoint dst(dx, dy);
                    if (isMoveValid(QPoint(x, y), dst)) {
                        validMoves.append(qMakePair(QPoint(x, y), dst));
                    }
                }
            }
        }
    }

    // 随机选一个合法走棋
    if (!validMoves.isEmpty()) {
        int idx = QRandomGenerator::global()->bounded(validMoves.size());
        return validMoves[idx];
    }
    return qMakePair(QPoint(-1, -1), QPoint(-1, -1));
}

QPair<QPoint, QPoint> ChessAI::scoreBasedMove()
{
    QVector<QPair<QPoint, QPoint>> validMoves;
    QVector<int> moveScores;

    // 收集所有合法走棋并评分
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 9; y++) {
            ChessMan *chess = m_board[x][y];
            if (chess->type() == None || chess->color() != m_color) {
                continue;
            }

            for (int dx = 0; dx < 10; dx++) {
                for (int dy = 0; dy < 9; dy++) {
                    QPoint dst(dx, dy);
                    if (isMoveValid(QPoint(x, y), dst)) {
                        validMoves.append(qMakePair(QPoint(x, y), dst));

                        // 计算评分：吃子得分 + 棋子自身价值 + 位置价值
                        int score = 0;
                        ChessMan *dstChess = m_board[dx][dy];
                        if (dstChess->type() != None) {
                            score += getChessScore(dstChess->type()) * 2; // 吃子双倍分
                        }
                        score += getChessScore(chess->type());

                        // 关键位置加分（九宫格、中路）
                        if (dst.y() == 4 || (dx >= 7 && dx <= 9 && dy >=3 && dy <=5)) {
                            score += 10;
                        }

                        moveScores.append(score);
                    }
                }
            }
        }
    }

    // 选评分最高的走棋
    if (!validMoves.isEmpty()) {
        int maxScore = -1;
        int maxIdx = 0;
        for (int i = 0; i < moveScores.size(); i++) {
            if (moveScores[i] > maxScore) {
                maxScore = moveScores[i];
                maxIdx = i;
            }
        }
        return validMoves[maxIdx];
    }
    return qMakePair(QPoint(-1, -1), QPoint(-1, -1));
}

int ChessAI::getChessScore(ChessType type)
{
    // 棋子价值评分（参考中国象棋子力价值）
    switch (type) {
    case RedJiang: case BlackJiang: return 1000; // 帅/将最重要
    case RedChe: case BlackChe: return 90;       // 车
    case RedPao: case BlackPao: return 45;       // 炮
    case RedMa: case BlackMa: return 40;         // 马
    case RedXiang: case BlackXiang: return 20;   // 相/象
    case RedShi: case BlackShi: return 20;       // 士
    case RedBing: case BlackZu: return 10;       // 兵/卒
    default: return 0;
    }
}

bool ChessAI::isMoveValid(QPoint src, QPoint dst)
{
    // 简化版走棋规则检查（复用棋盘类的核心逻辑，此处省略，可直接调用棋盘的checkMoveRule）
    ChessMan *srcChess = m_board[src.x()][src.y()];
    ChessMan *dstChess = m_board[dst.x()][dst.y()];

    if (srcChess->color() == dstChess->color()) {
        return false;
    }

    // 此处可直接复用ChessBoard的checkMoveRule方法，或重新实现核心逻辑
    return true;
}
