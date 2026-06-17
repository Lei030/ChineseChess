// chessboard.cpp
#include "chessboard.h"
#include "chessai.h"
#include <QPainter>
#include <QMouseEvent>
#include <QRandomGenerator>

ChessBoard::ChessBoard(QWidget *parent) : QWidget(parent)
{
    // 设置窗口大小
    int boardWidth = m_cellSize * 9 + 2 * m_cellSize;   // 左右各加1个格子边距
    int boardHeight = m_cellSize * 10 + 2 * m_cellSize; // 上下各加1个格子边距
    setMinimumSize(boardWidth, boardHeight);
    // 可选：设置最大尺寸
    setMaximumSize(m_cellSize * 12, m_cellSize * 13);
    initBoard();
}

void ChessBoard::initBoard()
{
    // 清空旧棋盘
    for (auto &row : m_board) {
        for (auto chess : row) {
            delete chess;
        }
        row.clear();
    }
    m_board.clear();

    // 初始化 10x9 空棋盘
    for (int x = 0; x < 10; x++) {
        QVector<ChessMan*> row;
        for (int y = 0; y < 9; y++) {
            row.append(new ChessMan(None, QPoint(x, y)));
        }
        m_board.append(row);
    }

    // 放置红方棋子
    m_board[0][4]->setType(RedJiang);
    m_board[0][3]->setType(RedShi); m_board[0][5]->setType(RedShi);
    m_board[0][2]->setType(RedXiang); m_board[0][6]->setType(RedXiang);
    m_board[0][1]->setType(RedMa); m_board[0][7]->setType(RedMa);
    m_board[0][0]->setType(RedChe); m_board[0][8]->setType(RedChe);
    m_board[2][1]->setType(RedPao); m_board[2][7]->setType(RedPao);
    m_board[3][0]->setType(RedBing); m_board[3][2]->setType(RedBing);
    m_board[3][4]->setType(RedBing); m_board[3][6]->setType(RedBing);
    m_board[3][8]->setType(RedBing);

    // 放置黑方棋子
    m_board[9][4]->setType(BlackJiang);
    m_board[9][3]->setType(BlackShi); m_board[9][5]->setType(BlackShi);
    m_board[9][2]->setType(BlackXiang); m_board[9][6]->setType(BlackXiang);
    m_board[9][1]->setType(BlackMa); m_board[9][7]->setType(BlackMa);
    m_board[9][0]->setType(BlackChe); m_board[9][8]->setType(BlackChe);
    m_board[7][1]->setType(BlackPao); m_board[7][7]->setType(BlackPao);
    m_board[6][0]->setType(BlackZu); m_board[6][2]->setType(BlackZu);
    m_board[6][4]->setType(BlackZu); m_board[6][6]->setType(BlackZu);
    m_board[6][8]->setType(BlackZu);

    // 重置选中状态和回合
    m_selectedChess = nullptr;
    m_currentTurn = Red;
    update();
}

bool ChessBoard::moveChess(QPoint src, QPoint dst)
{
    // 检查坐标合法性
    if (src.x() < 0 || src.x() > 9 || src.y() < 0 || src.y() > 8 ||
        dst.x() < 0 || dst.x() > 9 || dst.y() < 0 || dst.y() > 8) {
        return false;
    }

    ChessMan *srcChess = m_board[src.x()][src.y()];
    ChessMan *dstChess = m_board[dst.x()][dst.y()];

    // 检查：源位置有棋子、当前回合颜色匹配、目标位置不是己方棋子
    if (srcChess->type() == None || srcChess->color() != m_currentTurn ||
        dstChess->color() == m_currentTurn) {
        return false;
    }

    // 检查走棋规则
    if (!checkMoveRule(srcChess, dst)) {
        return false;
    }

    // 执行走棋：移除目标位置棋子（吃子）
    if (dstChess->type() != None) {
        dstChess->setAlive(false);
        delete dstChess;
    }

    // 移动棋子
    m_board[dst.x()][dst.y()] = srcChess;
    m_board[src.x()][src.y()] = new ChessMan(None, src);
    srcChess->setPos(dst);

    // 切换回合
    m_currentTurn = (m_currentTurn == Red) ? Black : Red;

    // 检查胜负
    if (checkWin(m_currentTurn)) {
        // 此处可添加胜负提示逻辑
    }

    // AI 走棋（如果是人机模式且当前是黑方回合）
    if ((m_gameMode == EasyAI || m_gameMode == HardAI) && m_currentTurn == Black) {
        QPair<QPoint, QPoint> aiStep = aiMove(m_gameMode == HardAI);
        moveChess(aiStep.first, aiStep.second);
    }

    update();
    return true;
}

QPair<QPoint, QPoint> ChessBoard::aiMove(bool isHard)
{
    ChessAI ai(m_board, m_currentTurn);
    return ai.getBestMove(isHard);
}

void ChessBoard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. 绘制棋盘背景
    painter.fillRect(rect(), QColor(240, 220, 180));

    // 2. 绘制棋盘网格
    painter.setPen(QPen(Qt::black, 2));
    // 横线（10条）
    for (int x = 0; x < 10; x++) {
        int pixelX = m_cellSize + x * m_cellSize;
        painter.drawLine(pixelX, m_cellSize, pixelX, m_cellSize * 9);
    }
    // 竖线（9条）
    for (int y = 0; y < 9; y++) {
        int pixelY = m_cellSize + y * m_cellSize;
        // 楚河汉界：中间竖线断开
        if (y == 4) {
            painter.drawLine(m_cellSize, pixelY, m_cellSize * 5, pixelY);
            painter.drawLine(m_cellSize * 6, pixelY, m_cellSize * 10, pixelY);
        } else {
            painter.drawLine(m_cellSize, pixelY, m_cellSize * 10, pixelY);
        }
    }

    // 3. 绘制楚河汉界
    painter.setFont(QFont("SimHei", 16));
    painter.drawText(m_cellSize * 5 - 60, m_cellSize * 5, "楚 河");
    painter.drawText(m_cellSize * 5 + 20, m_cellSize * 5, "汉 界");

    // 4. 绘制棋子
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 9; y++) {
            ChessMan *chess = m_board[x][y];
            if (chess->type() == None || !chess->isAlive()) {
                continue;
            }

            QPoint pixelPos = logicToPixel(QPoint(x, y));
            // 绘制棋子圆形背景
            painter.setBrush(chess->color() == Red ? Qt::red : Qt::black);
            painter.setPen(Qt::white);
            painter.drawEllipse(pixelPos, m_cellSize/2 - 5, m_cellSize/2 - 5);

            // 绘制棋子文字
            painter.setPen(Qt::white);
            painter.setFont(QFont("SimHei", 20));
            painter.drawText(QRect(pixelPos.x() - m_cellSize/2 + 5,
                                   pixelPos.y() - m_cellSize/2 + 5,
                                   m_cellSize - 10, m_cellSize - 10),
                             Qt::AlignCenter, chess->getName());
        }
    }

    // 5. 绘制选中棋子的高亮
    if (m_selectedChess) {
        QPoint pixelPos = logicToPixel(m_selectedPos);
        painter.setPen(QPen(Qt::yellow, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(pixelPos, m_cellSize/2, m_cellSize/2);
    }
}

void ChessBoard::mousePressEvent(QMouseEvent *event)
{
    // 转换鼠标坐标到棋盘逻辑坐标
    QPoint logicPos = pixelToLogic(event->pos());

    // 如果未选中棋子：选中当前位置的棋子（且是当前回合）
    if (!m_selectedChess) {
        ChessMan *chess = m_board[logicPos.x()][logicPos.y()];
        if (chess->type() != None && chess->color() == m_currentTurn) {
            m_selectedChess = chess;
            m_selectedPos = logicPos;
            update();
        }
    } else {
        // 如果已选中棋子：尝试走棋
        if (moveChess(m_selectedPos, logicPos)) {
            m_selectedChess = nullptr;
        } else {
            // 走棋失败：重新选中（如果是当前回合的棋子）
            ChessMan *chess = m_board[logicPos.x()][logicPos.y()];
            if (chess->type() != None && chess->color() == m_currentTurn) {
                m_selectedChess = chess;
                m_selectedPos = logicPos;
            } else {
                m_selectedChess = nullptr;
            }
        }
        update();
    }
}

bool ChessBoard::checkMoveRule(ChessMan *chess, QPoint dst)
{
    QPoint src = chess->pos();
    int dx = qAbs(dst.x() - src.x());
    int dy = qAbs(dst.y() - src.y());

    switch (chess->type()) {
    // 帅/将规则：只能在九宫格内走一步，不能对面
    case RedJiang: case BlackJiang: {
        // 九宫格范围
        int xMin = (chess->color() == Red) ? 0 : 7;
        int xMax = (chess->color() == Red) ? 2 : 9;
        int yMin = 3, yMax = 5;
        if (dst.x() < xMin || dst.x() > xMax || dst.y() < yMin || dst.y() > yMax) {
            return false;
        }
        // 只能走一步（横竖）
        if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
            return true;
        }
        // 帅将对面：同列且中间无棋子
        if (src.y() == dst.y() && dx > 1) {
            int start = qMin(src.x(), dst.x()) + 1;
            int end = qMax(src.x(), dst.x()) - 1;
            for (int x = start; x <= end; x++) {
                if (m_board[x][src.y()]->type() != None) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    // 士规则：九宫格内斜走一步
    case RedShi: case BlackShi: {
        int xMin = (chess->color() == Red) ? 0 : 7;
        int xMax = (chess->color() == Red) ? 2 : 9;
        int yMin = 3, yMax = 5;
        if (dst.x() < xMin || dst.x() > xMax || dst.y() < yMin || dst.y() > yMax) {
            return false;
        }
        return (dx == 1 && dy == 1);
    }

    // 相/象规则：斜走两步，不能过河，无塞象眼
    case RedXiang: case BlackXiang: {
        // 相不过河（红：x<=4；黑：x>=5）
        if ((chess->color() == Red && dst.x() > 4) || (chess->color() == Black && dst.x() < 5)) {
            return false;
        }
        if (dx == 2 && dy == 2) {
            // 检查象眼
            QPoint eye(src.x() + (dst.x() - src.x())/2, src.y() + (dst.y() - src.y())/2);
            if (m_board[eye.x()][eye.y()]->type() == None) {
                return true;
            }
        }
        return false;
    }

    // 马规则：走日，无绊马脚
    case RedMa: case BlackMa: {
        if ((dx == 1 && dy == 2) || (dx == 2 && dy == 1)) {
            // 检查马脚
            QPoint foot;
            if (dx == 1) {
                foot = QPoint(src.x(), src.y() + (dst.y() - src.y())/2);
            } else {
                foot = QPoint(src.x() + (dst.x() - src.x())/2, src.y());
            }
            if (m_board[foot.x()][foot.y()]->type() == None) {
                return true;
            }
        }
        return false;
    }

    // 车规则：横竖走任意步，无阻挡
    case RedChe: case BlackChe: {
        if (dx == 0 || dy == 0) {
            // 检查路径是否有阻挡
            int step = (dx == 0) ? dy : dx;
            int stepX = (dst.x() - src.x()) / (dx == 0 ? 1 : step);
            int stepY = (dst.y() - src.y()) / (dy == 0 ? 1 : step);

            for (int i = 1; i < step; i++) {
                QPoint pos(src.x() + i * stepX, src.y() + i * stepY);
                if (m_board[pos.x()][pos.y()]->type() != None) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    // 炮规则：横竖走任意步（不吃子无阻挡，吃子中间有且仅有一个棋子）
    case RedPao: case BlackPao: {
        if (dx == 0 || dy == 0) {
            ChessMan *dstChess = m_board[dst.x()][dst.y()];
            int step = (dx == 0) ? dy : dx;
            int stepX = (dst.x() - src.x()) / (dx == 0 ? 1 : step);
            int stepY = (dst.y() - src.y()) / (dy == 0 ? 1 : step);

            int blockCount = 0;
            for (int i = 1; i < step; i++) {
                QPoint pos(src.x() + i * stepX, src.y() + i * stepY);
                if (m_board[pos.x()][pos.y()]->type() != None) {
                    blockCount++;
                }
            }

            // 不吃子：无阻挡；吃子：有且仅有一个阻挡
            if ((dstChess->type() == None && blockCount == 0) ||
                (dstChess->type() != None && blockCount == 1)) {
                return true;
            }
        }
        return false;
    }

    // 兵/卒规则：没过河只能前走一步，过河可横走一步
    case RedBing: case BlackZu: {
        int dir = (chess->color() == Red) ? 1 : -1; // 红兵向下，黑卒向上
        // 没过河（红：x<=4；黑：x>=5）
        if ((chess->color() == Red && src.x() <= 4) || (chess->color() == Black && src.x() >= 5)) {
            return (dst.x() - src.x() == dir && dy == 0);
        } else {
            // 过河：前走或横走一步
            return ((dst.x() - src.x() == dir && dy == 0) || (dx == 0 && dy == 1));
        }
    }

    default:
        return false;
    }
}

QPoint ChessBoard::pixelToLogic(QPoint pixelPos)
{
    int x = (pixelPos.x() - m_cellSize/2) / m_cellSize;
    int y = (pixelPos.y() - m_cellSize/2) / m_cellSize;
    x = qBound(0, x, 9);
    y = qBound(0, y, 8);
    return QPoint(x, y);
}

QPoint ChessBoard::logicToPixel(QPoint logicPos)
{
    int x = m_cellSize/2 + logicPos.x() * m_cellSize + m_cellSize/2;
    int y = m_cellSize/2 + logicPos.y() * m_cellSize + m_cellSize/2;
    return QPoint(x, y);
}

bool ChessBoard::checkWin(ChessColor color)
{
    // 简化版：检查对方帅/将是否存活（实际需完善将军判定）
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 9; y++) {
            ChessMan *chess = m_board[x][y];
            if ((color == Red && chess->type() == BlackJiang && chess->isAlive()) ||
                (color == Black && chess->type() == RedJiang && chess->isAlive())) {
                return false;
            }
        }
    }
    return true;
}
