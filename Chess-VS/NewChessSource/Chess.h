#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <cstring>
#include <algorithm>

// 棋盘参数常量
#define CHESS_GRID_SIZE 65
#define ROW 10
#define COL 9
#define INTERVAL 60
#define BOARD_PIXEL_WIDTH (COL * CHESS_GRID_SIZE + 2 * INTERVAL)
#define BOARD_PIXEL_HEIGHT (ROW * CHESS_GRID_SIZE + 2 * INTERVAL)

// 【核心修改】使用枚举代替字符串进行逻辑判断
enum PieceType {
    EMPTY = 0,
    KING,   // 将/帅
    ADVISOR,// 士/仕
    BISHOP, // 象/相
    KNIGHT, // 马
    ROOK,   // 车
    CANNON, // 炮
    PAWN    // 卒/兵
};

struct ChessPiece {
    char name[8]; // 仅用于显示名字
    char type;    // 'R' (红), 'B' (黑), ' ' (空)
    PieceType id; // 【新增】逻辑ID，用于规则判断
    int r, c;
};

struct MoveStep {
    int fromRow, fromCol, toRow, toCol;
    ChessPiece capturedPiece;
};

class Chess {
private:
    ChessPiece map[ROW][COL];
    char currentTurn;
    int selectedRow, selectedCol;
    bool isGameOver;
    char winner;
    bool m_isCheck;
    std::vector<MoveStep> history;

    void init();
    // 【新增】辅助设置函数声明
    void setPiece(ChessPiece& p, int r, int c, const char* name, char type, PieceType id);

public:
    Chess();
    void reset();

    // Getters
    char getCurrentTurn() const { return currentTurn; }
    bool isGameOverFlag() const { return isGameOver; }
    char getWinner() const { return winner; }
    int getHistorySize() const { return history.size(); }
    int getSelectedRow() const { return selectedRow; }
    int getSelectedCol() const { return selectedCol; }
    bool isCheck() const { return m_isCheck; }
    const ChessPiece& getPiece(int row, int col) const { return map[row][col]; }
    // 【新增】canUndo 函数声明
    bool canUndo() const { return !history.empty(); }

    // 核心功能
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) const;
    bool isKingInCheck(char attackerType) const;
    void checkGameOver();
    bool hasValidMoves(char playerType);

    void selectChess(int row, int col);
    bool moveChess(int fromRow, int fromCol, int toRow, int toCol, bool recordHistory = true);
    void cancelSelect();
    void undoLastMove();
    int evaluateBoard(char playerType) const;
};