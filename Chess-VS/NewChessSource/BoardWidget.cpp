#include "BoardWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QRadialGradient> 

BoardWidget::BoardWidget(QWidget* parent) : QWidget(parent) {
    setFixedSize(BOARD_PIXEL_WIDTH, BOARD_PIXEL_HEIGHT);

    QPalette pal = palette();
    // 背景色：木色
    pal.setColor(QPalette::Window, QColor(225, 195, 150));
    setAutoFillBackground(true);
    setPalette(pal);
}

QPoint BoardWidget::gridToPixel(int r, int c) const {
    return QPoint(INTERVAL + c * CHESS_GRID_SIZE, INTERVAL + r * CHESS_GRID_SIZE);
}

bool BoardWidget::pixelToGrid(const QPoint& p, int& r, int& c) const {
    int boardLeft = INTERVAL - CHESS_GRID_SIZE / 2;
    int boardTop = INTERVAL - CHESS_GRID_SIZE / 2;
    c = (p.x() - boardLeft) / CHESS_GRID_SIZE;
    r = (p.y() - boardTop) / CHESS_GRID_SIZE;
    return (r >= 0 && r < ROW && c >= 0 && c < COL);
}

void BoardWidget::paintEvent(QPaintEvent*) {
    if (!m_chess) return;
    QPainter painter(this);
    // 开启高质量抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    drawGrid(painter);
    // 注意：先画棋子再画高亮框，或者反过来，看你喜好。这里保持原逻辑。
    drawHighlights(painter);
    drawPieces(painter);

    // 绘制“将军”提示 - 更加醒目的书法风格
    if (m_chess->isCheck() && !m_chess->isGameOverFlag()) {
        painter.setPen(QColor(230, 0, 0)); // 鲜红
        // 尝试使用行楷或隶书，如果没有则回退到楷体
        QFont font("华文行楷", 80, QFont::Bold);
        if (!font.exactMatch()) font.setFamily("楷体");

        painter.setFont(font);
        // 绘制文字阴影
        painter.setPen(QColor(0, 0, 0, 100));
        painter.drawText(rect().translated(3, 3), Qt::AlignCenter, "将军");
        // 绘制文字主体
        painter.setPen(QColor(200, 30, 30));
        painter.drawText(rect(), Qt::AlignCenter, "将军");
    }
}

void BoardWidget::drawGrid(QPainter& painter) {
    // 网格线颜色：深褐色，比纯黑更柔和
    QColor gridColor(60, 40, 20);
    painter.setPen(QPen(gridColor, 2));

    // 画横线
    for (int i = 0; i < ROW; i++) {
        painter.drawLine(gridToPixel(i, 0), gridToPixel(i, COL - 1));
    }
    // 画竖线
    for (int i = 0; i < COL; i++) {
        if (i == 0 || i == COL - 1)
            painter.drawLine(gridToPixel(0, i), gridToPixel(ROW - 1, i));
        else {
            painter.drawLine(gridToPixel(0, i), gridToPixel(4, i));
            painter.drawLine(gridToPixel(5, i), gridToPixel(ROW - 1, i));
        }
    }
    // 画九宫格斜线
    painter.drawLine(gridToPixel(0, 3), gridToPixel(2, 5));
    painter.drawLine(gridToPixel(0, 5), gridToPixel(2, 3));
    painter.drawLine(gridToPixel(7, 3), gridToPixel(9, 5));
    painter.drawLine(gridToPixel(7, 5), gridToPixel(9, 3));

    // 楚河汉界 - 调整为深褐色楷体
    painter.setFont(QFont("楷体", 32, QFont::Bold));
    painter.setPen(gridColor);

    int riverTopY = INTERVAL + 4 * CHESS_GRID_SIZE;
    int riverHeight = CHESS_GRID_SIZE;

    QRect leftRect(INTERVAL, riverTopY, 4 * CHESS_GRID_SIZE, riverHeight);
    painter.drawText(leftRect, Qt::AlignCenter, "楚 河");

    QRect rightRect(INTERVAL + 4 * CHESS_GRID_SIZE, riverTopY, 4 * CHESS_GRID_SIZE, riverHeight);
    painter.drawText(rightRect, Qt::AlignCenter, "汉 界");
}

void BoardWidget::drawPieces(QPainter& painter) {
    // 棋子半径
    int r = 30;

    // 设置字体：楷体更具古风
    QFont font("楷体", 26, QFont::Bold);
    painter.setFont(font);

    for (int i = 0; i < ROW; i++) {
        for (int k = 0; k < COL; k++) {
            const auto& piece = m_chess->getPiece(i, k);
            if (piece.type != ' ') {
                QPoint center = gridToPixel(i, k);

                // --- 1. 绘制阴影 (增加立体感) ---
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(0, 0, 0, 80)); // 半透明黑
                // 阴影向右下偏移3像素
                painter.drawEllipse(center + QPoint(3, 3), r, r);

                // --- 2. 绘制棋子主体 (仿木纹立体渐变) ---
                QRadialGradient gradient(center - QPoint(5, 5), r * 2);
                // 中心亮，边缘暗，模拟光照
                gradient.setColorAt(0.0, QColor(255, 235, 200)); // 亮木色
                gradient.setColorAt(0.6, QColor(230, 190, 140)); // 中木色
                gradient.setColorAt(1.0, QColor(190, 140, 90));  // 深木色

                painter.setBrush(gradient);
                // 棋子边框
                painter.setPen(QPen(QColor(120, 80, 40), 2));
                painter.drawEllipse(center, r, r);

                // --- 3. 绘制内圈装饰 (雕刻感) ---
                painter.setPen(QPen(QColor(140, 100, 60), 1));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(center, r - 4, r - 4);

                // --- 4. 绘制文字 (带雕刻凹陷效果) ---
                // 颜色定义：朱砂红 vs 墨黑
                QColor textColor = (piece.type == 'R') ? QColor(220, 30, 30) : QColor(20, 20, 20);

                QRect textRect(center.x() - r, center.y() - r, r * 2, r * 2);

                // 先画一层白色的文字，稍微向右下偏移，作为高光，产生凹陷感
                painter.setPen(QColor(255, 255, 255, 150));
                painter.drawText(textRect.translated(1, 1), Qt::AlignCenter, piece.name);

                // 再画原本颜色的文字
                painter.setPen(textColor);
                painter.drawText(textRect, Qt::AlignCenter, piece.name);
            }
        }
    }
}

void BoardWidget::drawHighlights(QPainter& painter) {
    int sr = m_chess->getSelectedRow();
    int sc = m_chess->getSelectedCol();
    if (sr != -1) {
        // 选中框颜色：使用明亮的青色，在木纹上对比度更好
        QColor highlightColor(0, 180, 255);

        QPen pen(highlightColor, 3);
        pen.setCapStyle(Qt::RoundCap); // 线条圆头
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        int halfSize = 32;
        int lineLen = 12; // 稍微加长一点
        QPoint p = gridToPixel(sr, sc);

        // 绘制四个角的折线
        // 左上
        painter.drawLine(p.x() - halfSize, p.y() - halfSize, p.x() - halfSize + lineLen, p.y() - halfSize);
        painter.drawLine(p.x() - halfSize, p.y() - halfSize, p.x() - halfSize, p.y() - halfSize + lineLen);
        // 右上
        painter.drawLine(p.x() + halfSize, p.y() - halfSize, p.x() + halfSize - lineLen, p.y() - halfSize);
        painter.drawLine(p.x() + halfSize, p.y() - halfSize, p.x() + halfSize, p.y() - halfSize + lineLen);
        // 左下
        painter.drawLine(p.x() - halfSize, p.y() + halfSize, p.x() - halfSize + lineLen, p.y() + halfSize);
        painter.drawLine(p.x() - halfSize, p.y() + halfSize, p.x() - halfSize, p.y() + halfSize - lineLen);
        // 右下
        painter.drawLine(p.x() + halfSize, p.y() + halfSize, p.x() + halfSize - lineLen, p.y() + halfSize);
        painter.drawLine(p.x() + halfSize, p.y() + halfSize, p.x() + halfSize, p.y() + halfSize - lineLen);
    }
}

// 鼠标点击处理 (逻辑未动，保持原样)
void BoardWidget::mousePressEvent(QMouseEvent* event) {
    if (!m_chess || m_chess->isGameOverFlag() || event->button() != Qt::LeftButton) return;

    int clickR, clickC;
    if (!pixelToGrid(event->pos(), clickR, clickC)) return;

    char currentTurn = m_chess->getCurrentTurn();
    const ChessPiece& clickedPiece = m_chess->getPiece(clickR, clickC);
    int selectedR = m_chess->getSelectedRow();

    // --- 交互逻辑 ---
    if (selectedR == -1) {
        if (clickedPiece.type == currentTurn) {
            m_chess->selectChess(clickR, clickC);
            update();
        }
    }
    else {
        if (clickedPiece.type == currentTurn) {
            m_chess->selectChess(clickR, clickC);
            update();
        }
        else {
            int selectedC = m_chess->getSelectedCol();
            emit playerTriedMove(selectedR, selectedC, clickR, clickC);
        }
    }
}