#ifndef STYLE_H
#define STYLE_H

#include <QString>

//全局样式表
//放在这里而不是散落在各个窗口的构造函数里, 方便统一调整配色。
//用法: 在 main() 里 qApp->setStyleSheet(APP_STYLE);
inline QString appStyle()
{
    return QStringLiteral(R"QSS(

/* ---------- 基础 ---------- */
QMainWindow, QDialog {
    background: #f5f6f8;
}
QLabel {
    color: #333;
    font-size: 13px;
}

/* ---------- 左栏: 联系人列表 ---------- */
QWidget#leftPanel {
    background: #fafbfc;
    border-right: 1px solid #e4e6eb;
}
QLabel#labelContacts {
    color: #8a8f99;
    font-size: 12px;
    font-weight: bold;
    padding-left: 4px;
}
QLabel#labelUser {
    color: #5a6270;
    font-size: 12px;
    padding: 6px 0;
    border-top: 1px solid #e4e6eb;
}

QListWidget#listWidgetFriends {
    background: transparent;
    border: none;
    outline: none;
    font-size: 13px;
}
QListWidget#listWidgetFriends::item {
    padding: 9px 8px;
    border-radius: 5px;
    color: #2c3038;
}
QListWidget#listWidgetFriends::item:hover {
    background: #eef2f7;
}
QListWidget#listWidgetFriends::item:selected {
    background: #4a90e2;
    color: #ffffff;
}

/* ---------- 右栏: 聊天区 ---------- */
QLabel#labelChatTitle {
    color: #1f2329;
    font-size: 14px;
    font-weight: bold;
    padding-bottom: 4px;
    border-bottom: 1px solid #e4e6eb;
}

QTextBrowser#textChat {
    background: #ffffff;
    border: 1px solid #e4e6eb;
    border-radius: 6px;
    padding: 10px;
    font-size: 13px;
}

QTextEdit#textEditMsg {
    background: #ffffff;
    border: 1px solid #d8dce3;
    border-radius: 6px;
    padding: 7px;
    font-size: 13px;
}
QTextEdit#textEditMsg:focus {
    border: 1px solid #4a90e2;
}

/* ---------- 按钮 ---------- */
QPushButton {
    background: #4a90e2;
    color: #ffffff;
    border: none;
    border-radius: 6px;
    padding: 6px 14px;
    font-size: 13px;
}
QPushButton:hover   { background: #3d7ec9; }
QPushButton:pressed { background: #2f6aa8; }
QPushButton:disabled {
    background: #c8ccd2;
    color: #f0f0f0;
}

/* 注销按钮用中性灰, 不跟"发送"抢视觉焦点 */
QPushButton#btnLogout {
    background: #eceef1;
    color: #5a6270;
    font-size: 12px;
}
QPushButton#btnLogout:hover   { background: #e0e3e8; }
QPushButton#btnLogout:pressed { background: #d2d6dc; }

/* ---------- 滚动条 ---------- */
QScrollBar:vertical {
    background: transparent;
    width: 8px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: #ccd1d9;
    border-radius: 4px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover { background: #aab1bb; }
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical,
QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    height: 0;
    background: none;
    border: none;
}

QScrollBar:horizontal {
    background: transparent;
    height: 8px;
    margin: 0;
}
QScrollBar::handle:horizontal {
    background: #ccd1d9;
    border-radius: 4px;
    min-width: 30px;
}
QScrollBar::handle:horizontal:hover { background: #aab1bb; }
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal,
QScrollBar::add-page:horizontal,
QScrollBar::sub-page:horizontal {
    width: 0;
    background: none;
    border: none;
}

)QSS");
}

#endif // STYLE_H