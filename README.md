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
- <img width="1489" height="1170" alt="b6600b28-d451-4f31-956c-17635fe537a7" src="https://github.com/user-attachments/assets/051c4385-0e99-486e-9588-18a37fcff367" />
- : 简单人机对战
<img width="1490" height="1170" alt="4f26a66c-6bea-47e4-9b82-99d4b90b1163" src="https://github.com/user-attachments/assets/be7c8090-a2b4-403d-a24a-d8808643e461" />
- : AI 对战（基于搜索算法）
- <img width="1490" height="1170" alt="4f26a66c-6bea-47e4-9b82-99d4b90b1163" src="https://github.com/user-attachments/assets/d77de2d5-88ec-418d-9a8e-2a3724efea88" />
- : 网络对战
- <img width="1491" height="1137" alt="45f0bfc9-bdc7-4abc-bae8-7781a7bb9f74" src="https://github.com/user-attachments/assets/d249d4e7-a5ee-404a-87f4-fabaaeea0bc9" />

- <img width="1493" height="1134" alt="cb8db62c-c0bd-44d3-8b05-3d6b7529a484" src="https://github.com/user-attachments/assets/67b264dd-7b23-4d86-afbd-fb80885050d4" />


- : 音效反馈
