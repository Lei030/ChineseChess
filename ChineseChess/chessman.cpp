// chessman.cpp
#include "chessman.h"

ChessMan::ChessMan() {}

ChessMan::ChessMan(ChessType type, QPoint pos)
    : m_type(type), m_pos(pos)
{}

ChessColor ChessMan::color() const
{
    switch (m_type) {
    case RedJiang: case RedShi: case RedXiang: case RedMa:
    case RedChe: case RedPao: case RedBing:
        return Red;
    case BlackJiang: case BlackShi: case BlackXiang: case BlackMa:
    case BlackChe: case BlackPao: case BlackZu:
        return Black;
    default:
        return NoColor;
    }
}

QString ChessMan::getName() const
{
    switch (m_type) {
    case RedJiang: return "帅";
    case RedShi: return "士";
    case RedXiang: return "相";
    case RedMa: return "马";
    case RedChe: return "车";
    case RedPao: return "炮";
    case RedBing: return "兵";
    case BlackJiang: return "将";
    case BlackShi: return "士";
    case BlackXiang: return "象";
    case BlackMa: return "马";
    case BlackChe: return "车";
    case BlackPao: return "炮";
    case BlackZu: return "卒";
    default: return "";
    }
}
