#pragma once
#include "WebView.h"
#include "AccountManager.h"

class WebWidget : public WebView
{
	Q_OBJECT
public:
	WebWidget(DBManager* db, QWidget *parent = nullptr);
	WebWidget(DBManager * db, QString site, QWidget * parent = nullptr);
	~WebWidget();

	void loadCookies(QString uid);

private:
	std::unique_ptr<AccountManager> m_accountmanager;
};

