#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H
#include "stdafx.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(DBManager)

class WebSocketServer :public QObject
{
	Q_OBJECT
public:
	explicit WebSocketServer(QObject *parent = nullptr);
	WebSocketServer(DBManager * db, QObject * parent);
	~WebSocketServer();

	bool start();

	bool isPortAvailable(int port);
	
	void initConnector();

protected:
	void setupConnConnections();

Q_SIGNALS:
	void closed();
	void downloadFinish(QJsonObject obj);

private slots:
	void onNewConnection();

	void processTextMessage(QString content);
	void commandHandler(QString message);
	void socketDisconnected();

private:
	QWebSocketServer* m_pWebSocketServer;
	QList<QWebSocket*> m_clients;
	DBManager* m_db;
};
#endif