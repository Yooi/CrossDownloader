#include "stdafx.h"
#include "WebWidget.h"

WebWidget::WebWidget(DBManager* db, QWidget *parent)
	: WebView(parent, 0)
	, m_accountmanager(std::make_unique<AccountManager>(db))
{
	double objectRate = CApplication::screenRate();
	//manage, pay donot set roomfactor
	setZoomFactor(1.3*objectRate);

	connect(webObject(), &WebObject::save, [=](QString site) {
		qDebug() << "WebWidget::saveCookies";
		db->addAccount(site, QJsonObject(), getCookieString());
		});
}

WebWidget::WebWidget(DBManager* db, QString site, QWidget *parent)
	:WebWidget(db, parent)
{
	loadCookies(site);
}

WebWidget::~WebWidget()
{
}

void WebWidget::loadCookies(QString uid)
{
	setCookies(m_accountmanager->loadCookieJar(uid));
}