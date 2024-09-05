#include "stdafx.h"
#include "WebSocketServer.h"
#include <qwebsocketserver.h>
#include <qwebsocket.h>
#include "DownloadManager.h"
#include "CApplication.h"

WebSocketServer::WebSocketServer(QObject *parent)
	: QObject(parent)
	, m_db(nullptr)
	, m_pWebSocketServer(new QWebSocketServer(QStringLiteral("CrossDownloader"), QWebSocketServer::NonSecureMode, this))
{
	qDebug() << "SERV Start.";
	//cause the port is occupied, it might cause a crash
	//startup socketserver firstly, if it crashes, wont cause wechat to crash
	if (start()) {
		//delay 3sec startup connector
		initConnector();
	}
}
WebSocketServer::WebSocketServer(DBManager* db, QObject *parent)
	:WebSocketServer(parent)
{
	qDebug() << "init socket db" << db;
	m_db = db;
}


WebSocketServer::~WebSocketServer()
{
	//m_pWebSocketServer->deleteLater();
}

bool WebSocketServer::start()
{
	qDebug() << "Start Listen Port.." << PORT;
	if (m_pWebSocketServer->listen(QHostAddress::LocalHost, PORT)) {
		qDebug() << "Echoserver listening on port" << PORT;
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
		connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &WebSocketServer::closed);
	}

	return m_pWebSocketServer->isListening();
}

bool WebSocketServer::isPortAvailable(int port)
{
	QTcpSocket socket;
	socket.connectToHost("localhost", port);
	if (socket.waitForConnected(1000))
	{
		// Port is in use
		socket.disconnectFromHost();
		return false;
	}
	// Port is available
	return true;
}


void WebSocketServer::initConnector()
{
	
}

void WebSocketServer::setupConnConnections()
{

}

void WebSocketServer::onNewConnection()
{
	QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
	qDebug() << "WebSocketServer::onNewConnection" << pSocket->requestUrl();
	if (pSocket->requestUrl().path() != "/crossdownloader") {
		pSocket->close();
	}
	connect(pSocket, &QWebSocket::textMessageReceived, this, &WebSocketServer::processTextMessage);
	connect(pSocket, &QWebSocket::disconnected, this, &WebSocketServer::socketDisconnected);

	//m_clients << pSocket;
}

void WebSocketServer::processTextMessage(QString message)
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	qDebug() << "Message received:" << message ;
	if (pClient) {
		//pClient->sendTextMessage(m_cmd->process(message));
		commandHandler(message);
	}
}

void WebSocketServer::commandHandler(QString message)
{
	auto jsondoc = QJsonDocument::fromJson(message.toUtf8());
	qDebug() << ">>" << jsondoc << jsondoc.isNull() << jsondoc.isEmpty();
	if (!jsondoc.isNull()) {
		
		auto downloader = new DownloadManager;
		downloader->start(jsondoc.object());
		QObject::connect(downloader, &DownloadManager::success, [=](QJsonObject obj) {
			//qDebug() << obj << m_db;
			int id = m_db->addResource(obj);
			obj["id"] = id;
			emit downloadFinish(obj);
		});
	}
}

void WebSocketServer::socketDisconnected()
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (pClient) {
		//m_clients.removeAll(pClient);
		pClient->deleteLater();
	}
}