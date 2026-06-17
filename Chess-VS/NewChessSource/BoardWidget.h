#pragma once
#include <QWidget>
#include "Chess.h"

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget(QWidget* parent = nullptr);
    void setChessRef(Chess* chessCtx) { m_chess = chessCtx; }

    // 【新增】设置当前是否为人机模式
    void setPVEMode(bool isPVE) { m_isPVE = isPVE; }

    QPoint gridToPixel(int r, int c) const;
    bool pixelToGrid(const QPoint& p, int& r, int& c) const;

signals:
    // 当玩家在棋盘上有效点击并尝试走棋时发出信号
    void playerTriedMove(int fr, int fc, int tr, int tc);

protected:
    void paintEvent(QPaintEvent* event) override;
    // 确保这里只有一个 mousePressEvent
    void mousePressEvent(QMouseEvent* event) override;

private:
    Chess* m_chess = nullptr;
    // 【新增】标记当前是否为人机模式
    bool m_isPVE = false;

    void drawGrid(QPainter& painter);
    void drawPieces(QPainter& painter);
    void drawHighlights(QPainter& painter);
};