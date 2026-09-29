#include "qchatclient.h"



QChatClient *QChatClient::instance()
{
    static QChatClient* client=new QChatClient();
    return client;
}

QChatClient::QChatClient(QObject *parent)
    :QTcpSocket(parent)
{
    setProxy(QNetworkProxy::NoProxy); //Qt会自动使用代理，关闭先
    connect(this, &QAbstractSocket::readyRead,    this, &QChatClient::onReadyRead);
    connect(this, &QAbstractSocket::stateChanged, this, &QChatClient::onSocketStateChanged);
    connect(this, &QAbstractSocket::errorOccurred, this, &QChatClient::onErrorOccurred);
}



void QChatClient::connectToServer(const QString &host, quint16 port)
{
    m_buf.clear();
    if(state()==QAbstractSocket::ConnectedState
        && peerAddress().toString()==host&&peerPort()==port){
        return;
    }       //已连接，不重复连接
    abort();
    connectToHost(host,port);
    //这里没有错误处理，错误处理在信号槽函数onSocketStateChanged触发
}

bool QChatClient::isReady() const
{
    return state()==QAbstractSocket::ConnectedState;
}

void QChatClient::sendlogin(const QString &name, const QString &pwd)
{
    QJsonObject js;
    js["msgid"]=LOGIN_MSG;
    js["name"]=name;
    js["password"]=pwd;
    write(packMsg(js));
}

void QChatClient::sendreg(const QString &name, const QString &pwd)
{
    QJsonObject js;
    js["msgid"]=REG_MSG;
    js["name"]=name;
    js["password"]=pwd;
    write(packMsg(js));
}

void QChatClient::sendlogout()
{
    if(m_myId==-1){
        return;
    }//没有登录，直接返回
    QJsonObject js;
    js["msgid"]=LOGOUT_MSG;
    js["id"]=m_myId;
    write(packMsg(js));

    m_myId = -1;                   // 本地状态更新
    m_myName.clear();
}

int QChatClient::myId() const
{
    return m_myId;
}

QString QChatClient::myName() const
{
    return m_myName;
}

void QChatClient::onReadyRead()
{
    QByteArray data=readAll();
    QJsonDocument doc=QJsonDocument::fromJson(data);
    if(!doc.isObject()){
        qDebug()<<"解析失败!"<<data;
        return;
    }
    dispatch(doc.object());
}

void QChatClient::onSocketStateChanged(QAbstractSocket::SocketState s)
{
    switch (s) {
    case QAbstractSocket::ConnectedState:
        emit linkStateChanged(true, "连接成功");
        break;
    case QAbstractSocket::UnconnectedState:
        m_myId = -1;                          // 断开时清登录态
        emit linkStateChanged(false, "未连接");
        break;
    default:
        break;
    }
}

void QChatClient::onErrorOccurred(SocketError e)
{
    Q_UNUSED(e);
    qDebug() << "socket 错误:" << e << errorString();
}



void QChatClient::dispatch(const QJsonObject &js)
{
    switch (js["msgid"].toInt()) {
    case LOGIN_MSG_ACK:    handleLoginAck(js);    break;
    case REG_MSG_ACK:      handleRegAck(js);      break;
    case PRIVATE_CHAT_MSG: handlePrivateChat(js); break;
    default:
        qDebug() << "暂未处理的消息类型:" << js;    break;
    }
}

void QChatClient::handleLoginAck(const QJsonObject &js)
{
    const int errno_ = js["errno"].toInt();
    const QString errmsg = js["errmsg"].toString();

    if (errno_ == 0) {
        m_myId = js["id"].toInt();
        m_myName = js["name"].toString();

        m_friends.clear();
        const QJsonArray arr = js["friends"].toArray();
        for(const QJsonValue &v:arr){
            if(!v.isObject()){
                continue;
            }
            const QJsonObject obj =v.toObject();
            User u;
            u.id    = obj["id"].toInt();
            u.name  = obj["name"].toString();
            u.state = obj["state"].toString();
            m_friends.append(std::move(u));
        }
        const QJsonArray offArr = js["offlinemsg"].toArray();
        for (const QJsonValue &v : offArr) {
            if (!v.isObject()) continue;
            const QJsonObject obj = v.toObject();
            if (obj["msgid"].toInt() != PRIVATE_CHAT_MSG) continue;   // 只处理私聊
            // 复用 handlePrivateChat 的组装逻辑
            ChatMessage msg;
            msg.msgid = obj["msgid"].toInt();
            msg.id    = obj["id"].toInt();
            msg.name  = obj["name"].toString();
            msg.toid  = obj["toid"].toInt();
            msg.msg   = obj["msg"].toString();
            msg.time  = obj["time"].toString();
            emit privateChatReceived(msg);
        }
    } else {
        m_myId = -1;
        m_myName.clear();
        m_friends.clear();
    }

    emit loginResult(errno_, errmsg);
}
void QChatClient::handleRegAck(const QJsonObject &js)
{
    const int errno_ = js["errno"].toInt();
    const QString errmsg = js["errmsg"].toString();
    emit regResult(errno_, errmsg);
    //注册之后暂时先不自动登录
}

void QChatClient::handlePrivateChat(const QJsonObject &js){
    ChatMessage msg;
    msg.msgid = js["msgid"].toInt();
    msg.id    = js["id"].toInt();
    msg.name  = js["name"].toString();
    msg.toid  = js["toid"].toInt();
    msg.msg   = js["msg"].toString();
    msg.time  = js["time"].toString();
    emit privateChatReceived(msg);
}

void QChatClient::sendPrivateChat(int toid, const QString &msg)
{
    QJsonObject js;
    js["msgid"] = PRIVATE_CHAT_MSG;
    js["id"]    = m_myId;
    js["name"]  = m_myName;
    js["toid"]  = toid;
    js["msg"]   = msg;
    js["time"]  = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    write(packMsg(js));

    //服务端只把私聊转发给接收方(toid), 不会回传给发送者,
    //所以这里自己回显一份给界面, 否则"我发的消息"在界面上永远不显示。
    //注意: 如果以后服务端也回传了, 这里要删掉, 否则会重复显示两条。
    ChatMessage self;
    self.msgid = PRIVATE_CHAT_MSG;
    self.id    = m_myId;
    self.name  = m_myName;
    self.toid  = toid;
    self.msg   = msg;
    self.time  = js["time"].toString();//用同一个时间, 跟发给服务端的一致
    emit privateChatReceived(self);
}

QList<User> QChatClient::friends() const
{
    return m_friends;
}

User QChatClient::friendById(int id) const
{
    for (const User &u : m_friends) {
        if (u.id == id)
            return u;
    }
    return User();//id 为默认值 -1 表示没找到
}
