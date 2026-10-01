#include "mainwindow.h"
#include "qchatclient.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEditMsg->installEventFilter(this);//事件过滤器，方便Enter键发送
    initSignalsAndSlots();                    //连接信号槽
    loadFriends();                            //填充好友列表
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFriends()
{
    QChatClient *c =QChatClient::instance();
    ui->listWidgetFriends->clear();
    m_friendItem.clear();
    for (const User &u : c->friends()) {
        addFriendItem(u);
    }
    ui->labelUser->setText(QString("%1(%2)").arg(c->myName()).arg(c->myId()));
}

void MainWindow::addFriendItem(const User &u)
{
    auto *item = new QListWidgetItem;
    item->setData(Qt::UserRole, u.id);
    item->setText((u.isOnline() ? "● " : "○ ") + u.name);
    ui->listWidgetFriends->addItem(item);
    m_friendItem.insert(u.id, item);
}

void MainWindow::on_listWidgetFriends_itemClicked(QListWidgetItem *item)
{
    m_currentFriendID = item->data(Qt::UserRole).toInt();
    ui->labelChatTitle->setText("会话: " + item->text().remove(QRegularExpression("^[●○] ")));
    refreshChatView();
}

void MainWindow::refreshChatView()
{
    ui->textChat->setHtml(m_history.value(m_currentFriendID));
    //把滚动条拉到底部, 看最新消息
    QScrollBar *bar = ui->textChat->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::appendMessage(int friendId, const QString &time,
                               const QString &who, const QString &text)
{
    const bool isMe = (who == QStringLiteral("我"));
    const QString bubbleBg = isMe ? "#4a90e2" : "#ffffff";
    const QString textColor = isMe ? "#ffffff" : "#2c3038";
    const QString align = isMe ? "right" : "left";
    const QString nameColor = isMe ? "#3d7ec9" : "#5a6270";

    //用单行表格实现左右对齐: QTextBrowser 对 text-align 支持不稳, 表格可靠
    const QString line = QString(
        "<table width='100%' cellspacing='0' cellpadding='0' style='margin:6px 0'>"
        "<tr><td align='%1'>"
        "<span style='color:#9aa0a6;font-size:9pt'>%2</span> "
        "<b style='color:%3;font-size:9pt'>%4</b><br>"
        "<span style='background:%5;color:%6;padding:6px 10px'>%7</span>"
        "</td></tr></table>")
        .arg(align, time, nameColor,
             who.toHtmlEscaped(), bubbleBg, textColor,
             text.toHtmlEscaped());

    m_history[friendId] += line;
}

void MainWindow::on_privateChatReceived(const ChatMessage &msg)
{
    const bool isMine = (msg.id == QChatClient::instance()->myId());

    // 自己发的 -> 会话对象是 toid; 别人发的 -> 会话对象是 id
    const int chatId = isMine ? msg.toid : msg.id;
    const QString who = isMine ? "我" : msg.name;

    appendMessage(chatId, msg.time, who, msg.msg);

    if (chatId == m_currentFriendID)
        refreshChatView();

}

void MainWindow::onLinkStateChanged(bool connected, const QString &text)
{
    //暂时不写连接状态改变之后怎么操作
}


void MainWindow::on_btnSend_clicked()
{
    auto *item = ui->listWidgetFriends->currentItem();
    if(!item){
        QMessageBox::warning(this,"提示","请先选择一个好友");
        return;
    }
    const QString text = ui->textEditMsg->toPlainText().trimmed();
    if (text.isEmpty())
        return;
    QChatClient::instance()->sendPrivateChat(item->data(Qt::UserRole).toInt(),text);
    ui->textEditMsg->clear();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->textEditMsg && event->type() == QEvent::KeyPress) {
        auto *ke = static_cast<QKeyEvent *>(event);
        if ((ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
            && !(ke->modifiers() & Qt::ShiftModifier)) {
            on_btnSend_clicked();
            return true;   // 吃掉事件，不插入换行
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::initSignalsAndSlots()
{
    QChatClient *c = QChatClient::instance();
    connect(c, &QChatClient::privateChatReceived, this, &MainWindow::on_privateChatReceived);
    connect(c, &QChatClient::linkStateChanged,    this, &MainWindow::onLinkStateChanged);
}


void MainWindow::on_btnLogout_clicked()
{
    if (QMessageBox::question(this, "提示", "确定要注销吗?",
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    close();//关闭窗口, 注销在 closeEvent 里统一处理
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //关窗时通知服务端, 否则服务端会一直认为你在线,
    //下次登录会被判成重复登录(errno=2)
    QChatClient::instance()->sendlogout();
    QMainWindow::closeEvent(event);
}

