#include "stdafx.h"
#include <QWebEngineSettings>
#include <QWebEngineScriptCollection>
#include <QWebEngineCertificateError>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QThread>
#include <QRegularExpression>
#include "webview.h"
#include "WebObject.h"

WebPage::WebPage(QWebEngineProfile *profile, QObject *parent)
	: QWebEnginePage(profile, parent)
	, m_channel(std::make_unique<QWebChannel>(this))
	, m_interceptor(std::make_unique<RequestInterceptor>())
	, m_webObject(std::make_unique<WebObject>(this))
	, m_loadFinished(false)
	, m_db(nullptr)
{
	m_view = qobject_cast<WebView*>(parent);
	m_profile = profile;
	m_profile->setUrlRequestInterceptor(m_interceptor.get());
	setWebChannel(m_channel.get());

	updateSettings();

	attachJavascriptObject();
}


WebPage::~WebPage()
{
	qDebug() << "~WebPage" << this;
}

void WebPage::updateSettings()
{
	settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
	settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
	settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
	settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
	settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
	settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
	settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
	settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
	//settings()->setAttribute(QWebEngineSettings::ReadingFromCanvasEnabled, false);

	//support flash, problem
	//settings()->globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);

	connect(this, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
	connect(this, SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChanged(const QUrl&)));

}

bool WebPage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
	
	qDebug() << "[WebPage::acceptNavigationRequest]" << url << type << isMainFrame;
	if (isMainFrame) {
		qDebug() << "[WebPage::acceptNavigationRequest] inject scripts!";
		scripts().insert(scriptChain());
	}
	
	//inject script for adding new Account

	return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

void WebPage::attachJavascriptObject()
{
	qDebug() << "[attachJavascriptObject]" << m_webObject.get();
	if (m_channel) {
		m_channel->registerObject("process", (QObject*)m_webObject.get());
	}
}

void WebPage::loadFinished(bool flag)
{
	qDebug() << "WebPage::loadFinished>>" << url();
	m_loadFinished = flag;
}

void WebPage::loadStarted()
{
}

void WebPage::updateZoomFactor(QUrl url)
{
	double objectRate = CApplication::screenRate();
	//manage, pay donot set roomfactor
	setZoomFactor(1.25*objectRate);
}

void WebPage::urlChanged(const QUrl & url)
{
	//QPointer is guarded pointer
	qDebug() << "[WebPage::urlChanged]" << url << url.host() << url.path().split("/").last();
	updateZoomFactor(url);

}

void WebPage::downloadRequested(QWebEngineDownloadItem *download) {
	qDebug() << "Format: " << download->savePageFormat();
	qDebug() << "Path: " << download->path();
	download->accept();
	if (!m_downloadItemsProperty.isEmpty()) {
		download->setProperty("info", m_downloadItemsProperty);
		download->setProperty("video", true);
		m_downloadItemsProperty = QJsonObject();
	}

	connect(download, &QWebEngineDownloadItem::stateChanged, [=](QWebEngineDownloadItem::DownloadState state) {
		QVariant info = download->property("info");
		if (state == QWebEngineDownloadItem::DownloadInterrupted) {
			//emit downloadCallback(info.toJsonObject()["did"].toString(), 0);
		}
		else if (state == QWebEngineDownloadItem::DownloadCancelled) {

		}
		download->deleteLater();

		});

	connect(download, &QWebEngineDownloadItem::finished, [=] {
		QVariant isVideo = download->property("video");
		if (isVideo.toBool()) {
			QString videoPath = download->path();
			QFileInfo videoFile(videoPath);
			QString name = videoFile.fileName();
			name.replace(QRegExp("\\.\\w+$"), "");
			QVariant info = download->property("info");

			QJsonObject videoObj = info.toJsonObject();
			//videoObj = json;
			videoObj["type"] = "video";
			videoObj["text"] = name;
			videoObj["filepath"] = videoPath;
			videoObj["status"] = 1;
			qDebug() << info;
			if (info.isNull()) {
				return;
			}
			QString id = info.toJsonObject()["id"].toString();
			if (id.isEmpty()) {
				if (videoObj.contains("timestamp")) {
					//addSchedule(videoObj["uid"].toString(), NULL, videoObj);
				}
				else {
					//addResource(videoObj);
				}
			}
			else {
				QJsonObject json;
				json["filepath"] = videoPath;
				json["status"] = 1;
				qDebug() << id << json;
				//updateResource(id, json);

			}

			QJsonObject json;
			json["status"] = 1;
			json["filepath"] = videoPath;
			//emit downloadCallback(info.toJsonObject()["did"].toString(), 1);
		}
		download->deleteLater();
		});
}


void WebPage::javaScriptAlert(const QUrl &securityOrigin, const QString &message)
{
	qDebug() << "~~~~~~~~~~~~~~~~~~javaScriptAlert" << message;
	//javascriptalert会弹出无效信息
	//QWebEnginePage::javaScriptAlert(securityOrigin, message);
	QMessageBox::information(0, QStringLiteral("CDR Hint"), message);
}

bool WebPage::javaScriptConfirm(const QUrl &securityOrigin, const QString &message)
{
	qDebug() << "~~~~~~~~~~~~~~~~~~javaScriptConfirm" << message;
	//javascriptalert会弹出无效信息
	//QWebEnginePage::javaScriptAlert(securityOrigin, message);
	QMessageBox::StandardButton btn = QMessageBox::information(0, QStringLiteral("CDR HINT"), message, QMessageBox::Cancel | QMessageBox::Ok);
	if (btn == QMessageBox::Ok) {
		return true;
	}
	return false;
}

bool WebPage::certificateError(const QWebEngineCertificateError & certificateError)
{
	//qCritical() << "~~~~~~~~~~~~~~~certificateError" << certificateError.isOverridable() << certificateError.error() << certificateError.url();
	return certificateError.isOverridable();
}

QWebEnginePage * WebPage::createWindow(QWebEnginePage::WebWindowType type)
{
	qDebug() << "WebPage::createWindow" << type;

	return NULL;
}

void WebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
	if (level == JavaScriptConsoleMessageLevel::ErrorMessageLevel) {
		qDebug() << "\033[31m[ConsoleMessage ERR]:" << QString("Level:%1, Line:%2, %3, %4").arg(level).arg(lineNumber).arg(message).arg(sourceID);
	}
	else if (level == JavaScriptConsoleMessageLevel::WarningMessageLevel) {
		qDebug() << "\033[32m[ConsoleMessage WARN]:" << QString("Level:%1, Line:%2, %3, %4").arg(level).arg(lineNumber).arg(message).arg(sourceID);
	}
	else {
		qDebug() << "\033[37m[ConsoleMessage INFO]:" << QString("Level:%1, Line:%2, %3, %4").arg(level).arg(lineNumber).arg(message).arg(sourceID);
	}
	//QWebEnginePage::javaScriptAlert(requestedUrl(), QString("Level:%1, Line:%2, %3").arg(level).arg(lineNumber).arg(message));
}

QWebEngineScript WebPage::webChannelScript()
{
	QFile webChannelJsFile(":///qtwebchannel/qwebchannel.js");
	if (!webChannelJsFile.open(QIODevice::ReadOnly)) {
		qDebug() << QString("Couldn't open qwebchannel.js file: %1").arg(webChannelJsFile.errorString());
	}
	else {
		qDebug() << "OK webChannelScript";
		QByteArray webChannelJs = webChannelJsFile.readAll();

		return injectScript(webChannelJs, "qwebchannel.js", true);
	}
	return QWebEngineScript();
}

QWebEngineScript WebPage::injectScript(QString js, QString name, bool onCreated)
{
	QWebEngineScript script;
	script.setSourceCode(js);
	script.setName(name);
	script.setWorldId(QWebEngineScript::MainWorld);
	if (onCreated) {
		script.setInjectionPoint(QWebEngineScript::DocumentCreation);
	}
	else {
		script.setInjectionPoint(QWebEngineScript::DocumentReady);
	}
	
	script.setRunsOnSubFrames(false);

	return script;
}

QList<QWebEngineScript> WebPage::scriptChain()
{
	QList<QWebEngineScript> list;
	qDebug() << "Inject WebPage::scriptChain" << m_injectOps;

	list.append(webChannelScript());
	QString webChannelJs =
		"\n"
		"new QWebChannel(qt.webChannelTransport, function(channel) {"
		"     window.process = channel.objects.process;"
		"     console.log('>> create CrossDownloader object successful');"
		"});";

	// inital webchannel and finger print control
	list.append(injectScript(webChannelJs, "channel", false));
	list.append(injectScript(qobject_cast<CApplication*>(qApp)->fingerPrint(), "init", false));
	// run customized scripts
	if (!m_extendScript.isEmpty()) {
		list.append(injectScript(qobject_cast<CApplication*>(qApp)->ajaxHookScript() + m_extendScript,"inject", m_injectOps["onCreated"].toBool(false)));
	}
	return list;
}

void WebPage::addScript(QString sitename, QString script, QJsonObject injectOps)
{
	m_extendScript = script;
	m_site = sitename;
	m_injectOps = injectOps;
}

void WebPage::refreshPage()
{
	//m_webObject->notify("refresh");
}

void WebPage::setDB(DBManager * db)
{
	m_db = db;
}

// webview
WebView::WebView(QWidget *parent, QString profileName)
	: QWebEngineView(parent)
	, m_apiManager(nullptr)
	, m_cookiejar(std::make_unique<CookieJar>())
	, success(false)
	, m_status(0)
{
	//newAccount is off-the-record profile
	if (profileName.isEmpty() || m_viewType == "newAccount") {
		qDebug() << "off-the-record profile==================";
		m_profile = std::unique_ptr<QWebEngineProfile>(qobject_cast<CApplication*>(qApp)->profile());
	}
	else {
		m_profile = std::unique_ptr<QWebEngineProfile>(qobject_cast<CApplication*>(qApp)->profile(profileName));
	}
	
	//m_profile = qobject_cast<CApplication*>(qApp)->profile();
	m_profilename = profileName;
	
	
	if (profileName.isEmpty() || m_viewType == "newAccount") {
		m_profile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
		m_profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
	}
	else {
		m_profile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
		m_profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
	}
	
	qDebug() << "Agent" << m_profile->httpUserAgent() << "isOffTheRecord" << m_profile->isOffTheRecord();

	m_profile->setHttpCacheMaximumSize(0);
	
	m_page = std::make_unique<WebPage>(m_profile.get());
	setPage(m_page.get());

	this->setContextMenuPolicy(Qt::NoContextMenu);
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	

	connect(m_profile.get(), SIGNAL(downloadRequested(QWebEngineDownloadItem*)), m_page.get(), SLOT(downloadRequested(QWebEngineDownloadItem*)));

	connect(m_profile->cookieStore(), &QWebEngineCookieStore::cookieAdded, [=](QNetworkCookie cookie) {
		//qDebug() << "cookieAdded" << cookie;
		insertCookiejar(cookie);
	});

	connect(this, &QWebEngineView::renderProcessTerminated, [this](QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode) {
		//QTimer::singleShot(0, [this] { reload(); });
		qDebug() << "QWebEngineView::renderProcessTerminated";
	});

}

//default profileName = 0, means work with current profile
//if set profileName, will create new profile
WebView::WebView(QString viewType, QWidget *parent, QString profileName)
	: WebView(parent, profileName)
{
	m_viewType = viewType;
}

WebView::WebView(QString viewType, QString workerid, QWidget *parent, QString profileName)
	: WebView(viewType, parent, profileName)
{
	m_workerid = workerid;
	setProperty("workerid", m_workerid);
	// permanentWorker wont be closed
	if (viewType != "permanentWorker") {
		m_timer.setSingleShot(true);
		m_timer.start(WORKER_TIMEOUT);
	}

	//worker recycle
	m_timer.callOnTimeout([=]() {
		qDebug() << "Timeout " << m_workerid << this;
		if (isWorker()) {
			//m_page->db()->setTaskStatus(m_workerid, 3);
		}
		emit recycle();
		//close();
	});
}

WebView::~WebView()
{
	//m_cookiejar->deleteLater();
	qDebug() << "WebView::~WebView" << this << parent() << QThread::currentThreadId();
	//m_timer.stop();
	//cleanPage();
	//Refer QT Assistant:
	//A disk-based QWebEngineProfile should be destroyed on or before application exit, 
	//otherwise the cache and persistent data may not be fully flushed to disk.
	//m_profile->deleteLater();
	
	//m_networkAccessManager->deleteLater();
	emit closeView();
	emit recycle();
}

void WebView::setDB(DBManager * db)
{
	m_page->setDB(db);
}

void WebView::setApiManager(ApiManager * api)
{
	m_apiManager = api;
	webObject()->setApiManager(api);
}

void WebView::addScript(QString sitename, QString script, QJsonObject injectOps)
{
	m_page->addScript(sitename, script, injectOps);
}

void WebView::cleanPage()
{
	m_profile->clearHttpCache();
	m_profile->cookieStore()->deleteAllCookies();
}

void WebView::setCookies(QList<QNetworkCookie> cookies)
{
	for (auto cookie : cookies)
	{
		m_profile->cookieStore()->setCookie(cookie);
	}

	//if worker is active, restart timer
	if (isWorker()) {
		m_timer.start(WORKER_TIMEOUT);
	}
}

void WebView::saveCookies(const QString uid)
{
	qDebug() << "WebView::saveCookies" << uid;
	m_page->db()->addAccount(uid, QJsonObject(), getCookieString());
}

void WebView::delayClose(int sec)
{
	m_timer.start(1000 * sec);
}

void WebView::runWorkerScript(QString script, QString scriptid)
{
	QString targetScript = script;
	//window.svm.jsResult used for return worker scirpt result
	//replace svm_scriptid to real scriptid
	targetScript.replace("_scriptid_placeholder", scriptid);
	page()->runJavaScript(targetScript);
}

void WebView::cancelWorkerScript(QString scriptid)
{
	runWorkerScript("ApiDispather('cancel', '', '_scriptid_placeholder')", scriptid);
}

QString WebView::getCookieString()
{
	QList<QNetworkCookie> cookies = cookiejar()->all();
	QString cookiestring = "";

	foreach (QNetworkCookie cookie, cookies)
	{
		//qDebug() << cookie;
		cookiestring += cookie.toRawForm() + COOKIE_SEPAROR;
	}
	return cookiestring;
}

void WebView::sendParams(QString service, QJsonObject params)
{
	webObject()->notify(service, params);
}

void WebView::closeEvent(QCloseEvent* event)
{
	qDebug() << "WebView::closeEvent" << this;
	if (m_viewType == "permanentWorker") {
		hide();
		event->ignore();
	}
	else {
		event->accept();
	}
	//page()->deleteLater();
}

void WebView::insertCookiejar(QNetworkCookie cookie)
{
	//BUG FIX: when webview is closed, cookiejar is deleted, but cookie still try to add 
	if (m_cookiejar) {
		m_cookiejar->insertCookie(cookie);
	}
}