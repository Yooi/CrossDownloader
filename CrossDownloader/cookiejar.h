#pragma once

#include <QNetworkCookieJar>
#include <QNetworkCookie>

class CookieJar : public QNetworkCookieJar
{
	Q_OBJECT

public:
	CookieJar(QObject *parent = nullptr);
	~CookieJar();
	void clear();
	void setCookies(QList<QNetworkCookie>);
	QString getCookie(QString key, QString url);
	void setCookies(QString key, QString val, bool secure);
	QList<QNetworkCookie> all();
};
