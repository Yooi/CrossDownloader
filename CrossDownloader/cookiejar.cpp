#include "stdafx.h"
#include "cookiejar.h"

CookieJar::CookieJar(QObject *parent)
	: QNetworkCookieJar(parent)
{
}

CookieJar::~CookieJar()
{
}

void CookieJar::clear()
{
	setAllCookies(QList<QNetworkCookie>());
}

void CookieJar::setCookies(QList<QNetworkCookie> cookies)
{
	setAllCookies(cookies);
}

QString CookieJar::getCookie(QString key, QString url)
{
	for (auto cookie: cookiesForUrl(url))
	{
		//qDebug() << "CookieJar::getCookie" << cookie << cookie.name() << key;
		if (cookie.name() == key.toUtf8()) {
			return cookie.value();
		}
	}
	return QString();
}

void CookieJar::setCookies(QString key, QString val, bool secure)
{
	QNetworkCookie cookie(key.toUtf8(), val.toUtf8());
	cookie.setSecure(true);
	cookie.setDomain("weibo.com");
	cookie.setPath("/");

	insertCookie(cookie);
}

QList<QNetworkCookie> CookieJar::all()
{
	return allCookies();
}