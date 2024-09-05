#include "stdafx.h"
#include "AccountManager.h"

AccountManager::AccountManager(const DBManager* db)
	:m_db((DBManager*)db)
{

}

void AccountManager::openSiteLogin(QString nickname)
{
	
}

QList<QNetworkCookie> AccountManager::updateCookies(QString uid, QList<QNetworkCookie> newcookies)
{
	QJsonObject account = m_db->queryAccount(uid);
	qDebug() << "AccountManager::updateCookies" << account;
	if (account.empty() || account["cookie"].isNull()) {
		return newcookies;
	}

	QList<QNetworkCookie> oldcookies;
	for each (QString cookie in account["cookie"].toString().split(COOKIE_SEPAROR))
	{
		oldcookies << QNetworkCookie::parseCookies(cookie.toUtf8());
	}

	CookieJar cookiejar;
	cookiejar.setCookies(oldcookies);
	//merge old cookies and new cookies
	for each (QNetworkCookie cookie in newcookies)
	{
		cookiejar.insertCookie(cookie);
	}

	return cookiejar.all();
}

void AccountManager::refreshCookie()
{
}

QList<QNetworkCookie> AccountManager::loadCookieJar(QString uid)
{
	//qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");
	QJsonObject account = m_db->queryAccount(uid);
	//QList<QNetworkCookie> cookies = qvariant_cast<QList<QNetworkCookie>>(account["cookies"].toString());
	QList<QNetworkCookie> cookies;
	for each (QString cookie in account["cookie"].toString().split(COOKIE_SEPAROR))
	{
		cookies << QNetworkCookie::parseCookies(cookie.toUtf8());
	}

	//qDebug() << cookies;
	return cookies;
}