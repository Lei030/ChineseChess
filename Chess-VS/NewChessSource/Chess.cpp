#include "Chess.h"

Chess::Chess() {
    reset();
}

// 辅助函数：统一设置棋子属性
void Chess::setPiece(ChessPiece& p, int r, int c, const char* name, char type, PieceType id) {
    p.r = r; p.c = c;
    p.type = type;
    p.id = id;
    strcpy_s(p.name, sizeof(p.name), name);
}

void Chess::init() {
    // 清空棋盘
    for (int i = 0; i < ROW; i++) {
        for (int k = 0; k < COL; k++) {
            setPiece(map[i][k], i, k, "", ' ', EMPTY);
        }
    }

    // 定义首行棋子的类型顺序
    PieceType rowTypes[] = { ROOK, KNIGHT, BISHOP, ADVISOR, KING, ADVISOR, BISHOP, KNIGHT, ROOK };

    // 初始化黑棋 (Row 0-4)
    const char* blackNames[] = { "車","馬","象","士","将","士","象","馬","車" };
    for (int k = 0; k < COL; k++)
        setPiece(map[0][k], 0, k, blackNames[k], 'B', rowTypes[k]);

    setPiece(map[2][1], 2, 1, "軳", 'B', CANNON);
    setPiece(map[2][7], 2, 7, "軳", 'B', CANNON);
    for (int k = 0; k < COL; k += 2)
        setPiece(map[3][k], 3, k, "卒", 'B', PAWN);

    // 初始化红棋 (Row 5-9)
    const char* redNames[] = { "车","马","相","仕","帅","仕","相","马","车" };
    for (int k = 0; k < COL; k++)
        setPiece(map[9][k], 9, k, redNames[k], 'R', rowTypes[k]);

    setPiece(map[7][1], 7, 1, "炮", 'R', CANNON);
    setPiece(map[7][7], 7, 7, "炮", 'R', CANNON);
    for (int k = 0; k < COL; k += 2)
        setPiece(map[6][k], 6, k, "兵", 'R', PAWN);
}

void Chess::reset() {
    isGameOver = false; winner = '\0';
    selectedRow = -1; selectedCol = -1;
    currentTurn = 'R'; // 永远红方先手
    m_isCheck = false;
    history.clear();
    init();
}

int getPieceValue(PieceType id) {
    switch (id) {
    case ROOK: return 90;
    case CANNON: return 45;
    case KNIGHT: return 40;
    case BISHOP: return 20;
    case ADVISOR: return 20;
    case PAWN: return 10;
    case KING: return 10000;
    default: return 0;
    }
}

int Chess::evaluateBoard(char playerType) const {
    int score = 0;
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            if (map[r][c].type == playerType) score += getPieceValue(map[r][c].id);
            else if (map[r][c].type != ' ' && map[r][c].type != playerType) score -= getPieceValue(map[r][c].id);
        }
    }
    return score;
}

//【核心修复】重构规则判断逻辑，确保吃子判定正确
bool Chess::isValidMove(int fromRow, int fromCol, int toRow, int toCol) const {
    // 1. 边界检查
    if (fromRow < 0 || fromRow >= ROW || fromCol < 0 || fromCol >= COL ||
        toRow < 0 || toRow >= ROW || toCol < 0 || toCol >= COL) return false;

    // 获取源棋子和目标棋子引用
    const ChessPiece& source = map[fromRow][fromCol];
    const ChessPiece& target = map[toRow][toCol];

    // 【防御性检查】源位置必须有棋子
    if (source.id == EMPTY) return false;

    // 2. 【核心检查】目标位置规则
    // 目标不能是己方棋子。即：目标要么是空地(' ')，要么是敌方棋子。
    if (target.type != ' ' && target.type == source.type) {
        return false; // 不能吃自己人
    }

    PieceType id = source.id;
    char chessType = source.type;
    int rowDiff = std::abs(fromRow - toRow);
    int colDiff = std::abs(fromCol - toCol);

    switch (id) {
    case KING:
        if (toCol < 3 || toCol > 5) return false;
        if ((chessType == 'B' && toRow > 2) || (chessType == 'R' && toRow < 7)) return false;
        // 老将照面判断
        if (map[toRow][toCol].id == KING) {
            if (fromCol != toCol) return false;
            for (int r = std::min(fromRow, toRow) + 1; r < std::max(fromRow, toRow); r++)
                if (map[r][fromCol].id != EMPTY) return false;
            return true;
        }
        return (rowDiff + colDiff == 1);

    case ADVISOR:
        if (toCol < 3 || toCol > 5) return false;
        if ((chessType == 'B' && toRow > 2) || (chessType == 'R' && toRow < 7)) return false;
        return (rowDiff == 1 && colDiff == 1);

    case BISHOP:
        if ((chessType == 'B' && toRow > 4) || (chessType == 'R' && toRow < 5)) return false;
        if (rowDiff != 2 || colDiff != 2) return false;
        if (map[(fromRow + toRow) / 2][(fromCol + toCol) / 2].id != EMPTY) return false;
        return true;

    case KNIGHT:
        if (!((rowDiff == 1 && colDiff == 2) || (rowDiff == 2 && colDiff == 1))) return false;
        {
            int legR = fromRow;
            int legC = fromCol;
            if (rowDiff == 2) legR = (fromRow + toRow) / 2;
            else legC = (fromCol + toCol) / 2;
            if (map[legR][legC].id != EMPTY) return false;
        }
        return true;

    case ROOK:
        if (rowDiff != 0 && colDiff != 0) return false;
        if (rowDiff != 0) {
            for (int r = std::min(fromRow, toRow) + 1; r < std::max(fromRow, toRow); r++)
                if (map[r][fromCol].id != EMPTY) return false;
        }
        else {
            for (int c = std::min(fromCol, toCol) + 1; c < std::max(fromCol, toCol); c++)
                if (map[fromRow][c].id != EMPTY) return false;
        }
        // 只要路径无阻挡，且目标不是自己人（开头已检查），就可以走/吃
        return true;

    case CANNON:
        if (rowDiff != 0 && colDiff != 0) return false;
        {
            int count = 0;
            if (rowDiff != 0) {
                for (int r = std::min(fromRow, toRow) + 1; r < std::max(fromRow, toRow); r++)
                    if (map[r][fromCol].id != EMPTY) count++;
            }
            else {
                for (int c = std::min(fromCol, toCol) + 1; c < std::max(fromCol, toCol); c++)
                    if (map[fromRow][c].id != EMPTY) count++;
            }

            // 炮的逻辑：
            // 移动到空地：中间无子，且目标是空地
            if (count == 0 && target.type == ' ') return true;
            // 吃子：中间有一子，且目标是敌方棋子（非空且非己方）
            if (count == 1 && target.type != ' ') return true;
        }
        return false;

    case PAWN:
    {
        bool crossed = (chessType == 'B' && fromRow > 4) || (chessType == 'R' && fromRow < 5);
        int forward = (chessType == 'B') ? 1 : -1;
        if (toRow == fromRow + forward && colDiff == 0) return true;
        if (crossed && rowDiff == 0 && colDiff == 1) return true;
    }
    return false;
    }
    return false;
}

bool Chess::isKingInCheck(char attackerType) const {
    char defenderType = (attackerType == 'B') ? 'R' : 'B';
    int kingR = -1, kingC = -1;
    for (int i = 0; i < ROW; i++) {
        for (int k = 0; k < COL; k++) {
            if (map[i][k].id == KING && map[i][k].type == defenderType) {
                kingR = i; kingC = k; goto Found;
            }
        }
    }
    return false; // Should not happen
Found:
    for (int r = 0; r < ROW; r++) {
        for (int c = 0; c < COL; c++) {
            if (map[r][c].type == attackerType) {
                if (isValidMove(r, c, kingR, kingC)) return true;
            }
        }
    }
    return false;
}

bool Chess::hasValidMoves(char playerType) {
    for (int fr = 0; fr < ROW; fr++) {
        for (int fc = 0; fc < COL; fc++) {
            if (map[fr][fc].type != playerType) continue;
            for (int tr = 0; tr < ROW; tr++) {
                for (int tc = 0; tc < COL; tc++) {

                
                    if (moveChess(fr, fc, tr, tc, true)) {

                        undoLastMove(); 
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Chess::checkGameOver() {
    if (isGameOver) return;
    if (!hasValidMoves(currentTurn)) {
        isGameOver = true;
        winner = (currentTurn == 'B' ? 'R' : 'B');
    }
}

void Chess::selectChess(int row, int col) {
    // 增加对 EMPTY 的判断，防止选中空地
    if (row >= 0 && row < ROW && col >= 0 && col < COL &&
        map[row][col].id != EMPTY && map[row][col].type == currentTurn) {
        selectedRow = row; selectedCol = col;
    }
}

// 【核心修改】使用新逻辑的 moveChess 函数
bool Chess::moveChess(int fromRow, int fromCol, int toRow, int toCol, bool recordHistory) {
    // 【新增】防御性检查：确保源位置有棋子
    if (map[fromRow][fromCol].id == EMPTY) return false;

    // 基础校验：移动的是当前回合的棋子
    if (map[fromRow][fromCol].type != currentTurn) return false;

    // 规则校验：走法是否合法（包括吃子规则）
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) return false;

    ChessPiece captured = map[toRow][toCol];
    ChessPiece moving = map[fromRow][fromCol];

    // 执行移动：覆盖目标位置
    map[toRow][toCol] = moving;
    map[toRow][toCol].r = toRow;
    map[toRow][toCol].c = toCol;
    // 清空源位置
    setPiece(map[fromRow][fromCol], fromRow, fromCol, "", ' ', EMPTY);

    // 校验：移动后自己的将/帅是否被将军（若被将军则撤销）
    char selfType = currentTurn;
    // 正确的做法是检查对手是否在攻击自己，即自己是否被将军。
    char opponentType = (selfType == 'R' ? 'B' : 'R');
    if (isKingInCheck(opponentType)) {
        // 撤销移动
        map[fromRow][fromCol] = moving;
        map[toRow][toCol] = captured;
        return false;
    }

    // 强制记录历史（除非明确要求不记录，比如AI模拟）
    if (recordHistory) {
        history.push_back({ fromRow, fromCol, toRow, toCol, captured });
    }

    // 切换回合
    currentTurn = (currentTurn == 'R') ? 'B' : 'R';
    cancelSelect();
    char attacker = (currentTurn == 'R') ? 'B' : 'R';
    m_isCheck = isKingInCheck(attacker);
    return true;
}

void Chess::cancelSelect() {
    selectedRow = -1; selectedCol = -1;
}

void Chess::undoLastMove() {
    if (history.empty() || isGameOver) return;

    MoveStep last = history.back();
    history.pop_back();

    // 恢复移动的棋子
    map[last.fromRow][last.fromCol] = map[last.toRow][last.toCol];
    map[last.fromRow][last.fromCol].r = last.fromRow;
    map[last.fromRow][last.fromCol].c = last.fromCol;

    // 恢复被吃掉的棋子
    map[last.toRow][last.toCol] = last.capturedPiece;

    // 切换回合回退
    currentTurn = (currentTurn == 'R') ? 'B' : 'R';
    cancelSelect();
    isGameOver = false;
    winner = '\0';
    char attacker = (currentTurn == 'R') ? 'B' : 'R';
    m_isCheck = isKingInCheck(attacker);
}