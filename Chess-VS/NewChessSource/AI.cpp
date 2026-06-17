#include "AI.h"
#include <cstdlib>
#include <vector>
#include <tuple>
#include <ctime>
#include <algorithm> 

// 定义极值，用于初始化 alpha 和 beta
const int INF = 1000000;

bool AI::aiMove(AIDifficulty difficulty) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    if (difficulty == AI_EASY) {
        return easyMove(); 
    }
    else {
        return hardMove(); 
    }
}

// 简单AI保持不变
bool AI::easyMove() {
    std::vector<std::tuple<int, int, int, int>> validMoves;
    for (int fr = 0; fr < ROW; fr++) {
        for (int fc = 0; fc < COL; fc++) {
            if (chess.getPiece(fr, fc).type != aiType) continue;

            for (int tr = 0; tr < ROW; tr++) {
                for (int tc = 0; tc < COL; tc++) {
                    if (chess.moveChess(fr, fc, tr, tc, true)) {
                        validMoves.emplace_back(fr, fc, tr, tc);
                        chess.undoLastMove();
                    }
                }
            }
        }
    }
    if (!validMoves.empty()) {
        int idx = std::rand() % validMoves.size();
        chess.moveChess(std::get<0>(validMoves[idx]), std::get<1>(validMoves[idx]),
            std::get<2>(validMoves[idx]), std::get<3>(validMoves[idx]), true);
        return true; // 成功走棋
    }
    return false; // 无路可走
}



bool AI::hardMove() {
    // 1. 初始化 (使用极小值，确保能被更新)
    int bestScore = -2000000;

    // 注意：这里定义的是【复数】bestMoves，是一个列表
    std::vector<std::tuple<int, int, int, int>> bestMoves;

    // 2. 遍历所有走法
    for (int fr = 0; fr < ROW; fr++) {
        for (int fc = 0; fc < COL; fc++) {
            if (chess.getPiece(fr, fc).type != aiType) continue;

            for (int tr = 0; tr < ROW; tr++) {
                for (int tc = 0; tc < COL; tc++) {
                    if (chess.moveChess(fr, fc, tr, tc, true)) {

                        // 3. 递归计算分数 (搜索深度 = 3)
                        int score = minimax(3, false, -2000000, 2000000);

                        chess.undoLastMove();

                        // 4. 更新最佳列表
                        if (score > bestScore) {
                            bestScore = score;
                            bestMoves.clear();
                            bestMoves.emplace_back(fr, fc, tr, tc);
                        }
                        else if (score == bestScore) {
                            bestMoves.emplace_back(fr, fc, tr, tc);
                        }
                    }
                }
            }
        }
    }

    // 5. 执行移动
    if (!bestMoves.empty()) {
        // 随机选择一种最佳走法
        int idx = std::rand() % bestMoves.size();

        // 【修复点】：先定义 move 变量接住 bestMoves[idx]
        std::tuple<int, int, int, int> move = bestMoves[idx];

        // 使用 move 变量来获取坐标
        chess.moveChess(
            std::get<0>(move),
            std::get<1>(move),
            std::get<2>(move),
            std::get<3>(move),
            true
        );
        return true;
    }
    else {
        // 如果 hard 模式算不出棋（被绝杀），尝试降级用随机步
        return easyMove();
    }
}


// 【核心新增】Minimax 递归函数实现
int AI::minimax(int depth, bool isMax, int alpha, int beta) {
    char humanType = (aiType == 'B' ? 'R' : 'B');

    // 1. 递归终止条件：达到搜索深度 或 游戏结束（此处简化为只看深度）
    if (depth == 0) {
        // 返回局面评分：(AI总分 - 玩家总分)
        // 这个差值越大，对AI越有利
        return chess.evaluateBoard(aiType) - chess.evaluateBoard(humanType);
    }

    // 2. 极大化层 (AI的回合)：寻找最大分
    if (isMax) {
        int maxEval = -INF;

        for (int fr = 0; fr < ROW; fr++) {
            for (int fc = 0; fc < COL; fc++) {
                // 遍历 AI 的棋子
                if (chess.getPiece(fr, fc).type != aiType) continue;

                for (int tr = 0; tr < ROW; tr++) {
                    for (int tc = 0; tc < COL; tc++) {
                        if (chess.moveChess(fr, fc, tr, tc, true)) {
                            // 递归调用，下一层是Min层
                            int eval = minimax(depth - 1, false, alpha, beta);
                            chess.undoLastMove();

                            maxEval = std::max(maxEval, eval);
                            alpha = std::max(alpha, eval);

                            // Alpha-Beta 剪枝
                            if (beta <= alpha) {
                                return maxEval;
                            }
                        }
                    }
                }
            }
        }
        return maxEval == -INF ? -INF : maxEval; // 如果无路可走
    }
    // 3. 极小化层 (玩家的回合)：假设玩家会选择对AI最不利（分最低）的走法
    else {
        int minEval = INF;

        for (int fr = 0; fr < ROW; fr++) {
            for (int fc = 0; fc < COL; fc++) {
                // 遍历 玩家(对手) 的棋子
                if (chess.getPiece(fr, fc).type != humanType) continue;

                for (int tr = 0; tr < ROW; tr++) {
                    for (int tc = 0; tc < COL; tc++) {
                        if (chess.moveChess(fr, fc, tr, tc, true)) {
                            // 递归调用，下一层是Max层
                            int eval = minimax(depth - 1, true, alpha, beta);
                            chess.undoLastMove();

                            minEval = std::min(minEval, eval);
                            beta = std::min(beta, eval);

                            // Alpha-Beta 剪枝
                            if (beta <= alpha) {
                                return minEval;
                            }
                        }
                    }
                }
            }
        }
        return minEval == INF ? INF : minEval;
    }
}