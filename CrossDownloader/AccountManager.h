#pragma once
#include <qobject.h>
#include "WebView.h"
#include "Dbmanager.h"
#include <QNetworkCookie>

class AccountManager :	public QObject
{
	Q_OBJECT
public:
	AccountManager(const DBManager* db);
	~AccountManager(){}

public slots:
	void openSiteLogin(QString site);
	QList<QNetworkCookie> updateCookies(QString uid, QList<QNetworkCookie> newcookies);
	void refreshCookie();

	QList<QNetworkCookie> loadCookieJar(QString uid);

private:
	DBManager* m_db;
};

