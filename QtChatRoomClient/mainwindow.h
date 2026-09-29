#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "user.h"
#include <QMainWindow>
#include <QListWidget>
#include <QCloseEvent>
#include <QMessageBox>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QScrollBar>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_listWidgetFriends_itemClicked(QListWidgetItem *item);

    void on_btnSend_clicked();
    void on_privateChatReceived(const ChatMessage &msg);
    void onLinkStateChanged(bool connected, const QString &text);
    void on_btnLogout_clicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private:
    void initSignalsAndSlots();
    void loadFriends();
    void addFriendItem(const User &u);
    void refreshChatView();
    void appendMessage(int friendId, const QString &time,
                       const QString &who, const QString &text);
    Ui::MainWindow *ui;
    QHash<int,QString> m_history;
    QHash<int,QListWidgetItem*> m_friendItem; //friendid映射缓存，方便查找好友
    int m_currentFriendID = -1;//当前会话的好友id, -1表示还没选中任何好友
};

#endif // MAINWINDOW_H
