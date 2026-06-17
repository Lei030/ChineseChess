#pragma once
#include "Chess.h"

enum AIDifficulty { AI_EASY, AI_HARD };

class AI {
private:
    Chess& chess;
    char aiType; // AI执棋方（通常为 'B' 黑方）

    // 搜索深度，层数越高越聪明但计算越慢。3-4是PC端比较合适的值
    const int SEARCH_DEPTH = 3;

    // 简单AI：随机移动
    bool easyMove();
    // 困难AI：Minimax + Alpha-Beta 剪枝入口
    bool hardMove();

    // Minimax 搜索算法核心函数
    // depth: 剩余搜索深度
    // isMax: true代表AI层（找最大分），false代表玩家层（找最小分）
    // alpha: 目前找到的最好下限
    // beta: 目前找到的最好上限
    int minimax(int depth, bool isMax, int alpha, int beta);

public:
    AI(Chess& chessInstance) : chess(chessInstance), aiType('B') {}
    void setAIType(char type) { aiType = type; }
    bool aiMove(AIDifficulty difficulty);
};