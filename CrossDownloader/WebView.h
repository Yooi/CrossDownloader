#pragma once
#include <qwebengineview.h>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QWebEngineScript>
#include <QNetworkCookieJar>
#include <qwebchannel.h>
#include "CApplication.h"
#include "cookiejar.h"
#include "requestinterceptor.h"
#include "Dbmanager.h"

class WebObject;
class WebView;
class ApiManager;

class WebPage : public QWebEnginePage {
	Q_OBJECT

public:
	WebPage(QWebEngineProfile *profile, QObject *parent = nullptr);
	~WebPage();

	WebObject* webObject() { return m_webObject.get(); }
	WebView* view() { return m_view; }

	void setDB(DBManager * db);
	DBManager* db() { return m_db; }

public slots:
	//QVariant webAgent() { return AGENT;	}
	
	void refreshPage();
	void addScript(QString sitename, QString script, QJsonObject injectOps);

	QWebEngineScript injectScript(QString js, QString name = 0, bool injectOnCreated = false);
	void downloadRequested(QWebEngineDownloadItem * download);
	bool isLoadFinished() { return m_loadFinished; }
signals:
	void jsSync(QVariant);

protected:
	void updateSettings();
	QWebEngineScript webChannelScript();
	QList<QWebEngineScript> scriptChain();
	bool acceptNavigationRequest(const QUrl & url, NavigationType type, bool isMainFrame);
	void attachJavascriptObject();
	void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID);
	void javaScriptAlert(const QUrl & securityOrigin, const QString & message);
	bool javaScriptConfirm(const QUrl & securityOrigin, const QString & message);
	bool certificateError(const QWebEngineCertificateError &certificateError);
	QWebEnginePage *createWindow(QWebEnginePage::WebWindowType type);

private slots:
	void loadFinished(bool flag);
	void loadStarted();
	void updateZoomFactor(QUrl url);
	void urlChanged(const QUrl &url);

private:
	friend class WebView;
	QHash<QString, QString> m_webcache;
	QJsonObject m_downloadItemsProperty;
	QWebEngineProfile* m_profile;
	std::unique_ptr<QWebChannel> m_channel;
	//DBManager* m_dbmanager;
	std::unique_ptr<RequestInterceptor> m_interceptor;
	QList<QJsonObject> m_uploadedImage;
	QVariant m_syncStatus;
	//ScheduleManager* m_pSchedule;
	QString m_token;
	std::unique_ptr<WebObject> m_webObject;
	QString m_extendScript;
	QString m_site;
	QJsonObject m_injectOps;
	bool m_loadFinished;
	WebView* m_view;
	DBManager* m_db;
};

class WebView : public QWebEngineView
{
	Q_OBJECT

public:
	//if set profilename is 0, its off-the-record model
	WebView(QWidget *parent = 0, QString profileName = "default");
	WebView(QString viewType, QWidget *parent = 0, QString prifileName = "default");
	WebView(QString viewType, QString workerid, QWidget *parent = 0, QString prifileName = "default");
	~WebView();

	void setDB(DBManager * db);
	void setApiManager(ApiManager* api);
	ApiManager* apiManager() { return m_apiManager; };
	WebObject* webObject() { return m_page->webObject(); };
	void addScript(QString sitename, QString script, QJsonObject injectOps);
	QString type() { return m_viewType; };
	void sendUser(const QJsonObject& info) { emit user(info); };
	void setTrigger(QString service) { emit trigger(service); };
	void sendOperation(QString op, QString uid) { emit operation(op, uid); };
	void sendClose() { emit closeView(); };
	void sendInitView(QString uid) { emit initUserView(uid); };
	//QWebEngineProfile* profile() { return m_profile.get(); };
	void insertCookiejar(QNetworkCookie cookie);
	void syncCookies(CookieJar* cookiejar);
	CookieJar* cookiejar() { return m_cookiejar.get(); };
	QString profileName() { return m_profilename; }
	void cleanPage();

	bool isWorker() { return m_viewType == "worker"; }

	void setCookies(QList<QNetworkCookie> cookies);
	//save target cookie, uid currently is site
	void saveCookies(const QString uid);
	void delayClose(int sec);
	void runWorkerScript(QString script, QString scriptid);
	void cancelWorkerScript(QString scriptid);
	QString getCookieString();
	//login win status
	void setSuccess() { success = true; }
	bool getSuccess() { return success; }

	QString workerId() { return m_workerid; }

	void setValid(int sts) { m_status = sts; }
	bool isValid() { return m_status == 1; };
	bool error() { return m_status < 0; }

	//send params to view
	void sendParams(QString service, QJsonObject params = QJsonObject());

protected:
	void closeEvent(QCloseEvent* event) override;

signals:
	void user(const QJsonObject&);
	void trigger(QString);
	void closeView();
	void operation(QString op, QString uid = 0);
	void reminder(QString, QString);
	void initUserView(QString);
	void cookieSync(QString uid);
	void recycle();

private:
	std::unique_ptr<WebPage> m_page;
	std::unique_ptr<QWebEngineProfile> m_profile;
	//QWebEngineProfile* m_profile;
	//CookieJar* m_cookiejar;
	std::unique_ptr<CookieJar> m_cookiejar;
	ApiManager* m_apiManager;
	//NetworkAccessManager *m_networkAccessManager;
	QString m_profilename;
	// null is normall, auth window or login window
	QString m_viewType;
	bool success;
	QTimer m_timer;
	QString m_workerid;
	int m_status;
};
