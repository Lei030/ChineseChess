// chessman.h
#ifndef CHESSMAN_H
#define CHESSMAN_H

#include <QPoint>
#include <QString>

// 棋子类型
enum ChessType {
    None,          // 空位置
    RedJiang,      // 红帅
    RedShi,        // 红士
    RedXiang,      // 红相
    RedMa,         // 红马
    RedChe,        // 红车
    RedPao,        // 红炮
    RedBing,       // 红兵
    BlackJiang,    // 黑将
    BlackShi,      // 黑士
    BlackXiang,    // 黑象
    BlackMa,       // 黑马
    BlackChe,      // 黑车
    BlackPao,      // 黑炮
    BlackZu        // 黑卒
};

// 棋子颜色
enum ChessColor {
    Red,
    Black,
    NoColor
};

class ChessMan
{
public:
    ChessMan();
    ChessMan(ChessType type, QPoint pos);

    // 获取棋子属性
    ChessType type() const { return m_type; }
    ChessColor color() const;
    QPoint pos() const { return m_pos; }
    bool isAlive() const { return m_isAlive; }

    // 设置棋子属性
    void setType(ChessType type) { m_type = type; }
    void setPos(QPoint pos) { m_pos = pos; }
    void setAlive(bool alive) { m_isAlive = alive; }

    // 获取棋子名称（用于显示）
    QString getName() const;

private:
    ChessType m_type = None;    // 棋子类型
    QPoint m_pos;               // 棋子位置（棋盘坐标，如(0,0)到(9,8)）
    bool m_isAlive = true;      // 是否存活
};

#endif // CHESSMAN_H
