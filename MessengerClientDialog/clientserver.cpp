#include "clientserver.h"

ClientServer::ClientServer(int id):nextBlockSize(0), ownId(id){
    pClientSocket = new QTcpSocket(this);
    clientServer = new QTcpServer(this);
    if ( !clientServer->listen(QHostAddress::Any, 2324) ){
        qDebug() << "Server error. Unable to start the server: " + clientServer->errorString();
        clientServer->close();
        return;
    }
    connect(clientServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

ClientServer::~ClientServer(){
    clientServer->close();
    delete clientServer;
}


void ClientServer::answerToClient(QTcpSocket* pClientSocket, const QString& str){
    QByteArray buff;
    QDataStream out(&buff, QIODevice::WriteOnly);
    out << quint16(0) << ownId << str;
    out.device()->seek(0);
    out << quint16(buff.size() - sizeof(quint16));

    pClientSocket->write(buff);
}

//Slots
void ClientServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = clientServer->nextPendingConnection();
    connect(this, SIGNAL( signalDeleteLater() ),
            pClientSocket, SLOT(deleteLater())
            );
    connect(pClientSocket, SIGNAL(disconnected()),
            this, SLOT( slotDisconnected() )
            );
    connect(pClientSocket, SIGNAL(readyRead()),
            this, SLOT(slotReadClient())
            );
    answerToClient(pClientSocket, "Server response: Connected!");
}

void ClientServer::slotReadClient(){
    QTcpSocket* pClientSocket = static_cast<QTcpSocket*>(sender());
    QDataStream in(pClientSocket);
    while (true) {
        if (!nextBlockSize){
            if (pClientSocket->bytesAvailable() < sizeof(quint16))
                break;
            in >> nextBlockSize;
        }
        if (pClientSocket->bytesAvailable() < nextBlockSize){
            break;
        }
        QString str;
        int id = 0;
        in >> id >> str;
        emit readMsg(id,str);
    }
}

void ClientServer::sendMsg(const QString &ip, const QString &str){
    QHostAddress addr(ip);
    QString k = addr.toString();
    QByteArray buff;
    pClientSocket->connectToHost(addr,2324);
    pClientSocket->waitForConnected();
    QDataStream out(&buff, QIODevice::WriteOnly);
    out << quint16(0) << ownId << str;
    out.device()->seek(0);
    out << quint16(buff.size() - sizeof(quint16));

    pClientSocket->write(buff);
    //pClientSocket->disconnectFromHost();
}





