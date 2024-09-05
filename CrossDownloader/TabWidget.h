#pragma once
#include "ui_commentWidget.h"
#include "Dbmanager.h"

class CommentWidget : public QWidget
{
	Q_OBJECT
public:
	
	CommentWidget(DBManager* db, QWidget *parent = nullptr);
	~CommentWidget();
	
	ResourceTableWidget* table() { return ui.tableWidget; };

	int key() { return property("key").toInt(); }

	void setupConnections();
	void setKey(int key);
	//set base info
	void setInfo(int parentid);
	int addComments(const QJsonObject& results);

signals:
	void commentUrl(const QString & url, int total = 100);
	void removeTab(int);

private:
	Ui::commentWidget ui;
	QJsonObject commentData;
	DBManager* m_db;
};

/*
class WebWidget : public WebView
{
	Q_OBJECT
public:
	WebWidget(DBManager* db, QWidget *parent = nullptr);
	~WebWidget() {};

	void loadCookies(QString uid);

private:
	//std::unique_ptr<AccountManager> m_accountmanager;
};*/