#pragma once
#include "ApiManager.h"

class WebPage;

class WebObject :public QObject
{
	Q_OBJECT
public:
	WebObject(const WebPage* page);
	~WebObject();

	void setApiManager(ApiManager* api) { m_apiManager = api; };

public slots:
	void openUrl(const QString&);
	void reload();

	//void login(QString sitenick, QString uid = 0, bool defaultShow = false);
	QString worker(QString workerId, QString script);

	void workerOperation(QString workerId, QString op);

	//QString uploadWorker(QString uid, QString url, QString script, QString wid);

	void login(QString sitenick, bool defaultShow = true);

	//return worker scirpt result
	// how to call:  window.svm.jsResult(json, "_scriptid_placeholder")
	// arg1: json result, arg2: placeholder default "_scriptid_placeholder"
	void jsResult(QJsonObject, QString scriptid = 0);
	
	void saveCookies(QString uid);

	void sendUser(QJsonObject user) { m_page->view()->sendUser(user); }
signals:
	//api call
	void call(QString api, QJsonObject params = QJsonObject());
	//worker send js result to app
	void notify(QString service, QJsonObject params = QJsonObject());
	//send result to worker, api callback
	void jsComplete(QJsonObject, QString);

	//currently save cookies
	void save(QString);

	void loginSite(QString, bool);

private:
	WebPage* m_page;
	ApiManager* m_apiManager;
};

