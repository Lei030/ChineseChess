# 中国象棋 (Chinese Chess)

基于 Qt 和 C++ 开发的中国象棋游戏，支持 AI 对战和网络对战。

## 项目结构

```
ChineseChess/
├── Chess-VS/              # Visual Studio 解决方案
│   ├── NewChessSource/    # 新版源码 (AI, 棋盘, 主窗口, 网络)
│   ├── SuperChineseChess/ # VS 项目工程文件
│   ├── resourse/          # 音频资源 (bgm, check)
│   └── SuperChineseChess.sln
└── ChineseChess/          # Qt 独立项目 (qmake 直接编译)
    ├── chessai.cpp/h      #  AI 引擎
    ├── chessboard.cpp/h   #  棋盘逻辑
    ├── chessman.cpp/h     #  棋子逻辑
    ├── chessnetwork.cpp/h #  网络对战
    ├── mainwindow.cpp/h   #  主窗口
    ├── main.cpp
    └── ChineseChess.pro
```

## 编译方式

### Visual Studio (Chess-VS)
1. 打开 `Chess-VS/SuperChineseChess.sln`
2. 配置 Qt VS Tools 插件
3. 编译运行

### Qt 独立项目 (ChineseChess)
```bash
cd ChineseChess
qmake ChineseChess.pro
make          # Linux/macOS
nmake         # Windows (MSVC)
mingw32-make  # Windows (MinGW)
```

## 功能

- : 人人对战
- : AI 对战（基于搜索算法）
- : 网络对战
- : 音效反馈
