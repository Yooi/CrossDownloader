#pragma once
#include "networkaccessmanager.h"
#include "type.h"
#include "Dbmanager.h"
#include "AccountManager.h"
#include "ViewManager.h"

class ResourceDialog;

class ApiManager :	public QObject
{
	Q_OBJECT
public:
	
	ApiManager(DBManager* db = nullptr, QWidget* parent = nullptr);
	~ApiManager() {};

	void setDB(DBManager* db) { m_db = db; }

	template<typename Functor>
	void search(int id, const QString& keyword, Functor functor) {
		QJsonObject item = m_db->getResource(QString::number(id));
		QJsonObject params;
		params["keyword"] = keyword;
		Api api = generator(item["site"].toString(), APIType::SEARCH, params);
		call(api, functor);
	}
	/*
	parser url, get params and api detail
	then call worker to implement the api and return results
	*/
	template<typename Functor>
	void call(const QString& url, APIType type, Functor functor, QJsonObject params = QJsonObject()) {
		NetworkAccessManager accessmanager;
		QString targetUrl = accessmanager.resolveUrl(url);
		QString site = siteName(targetUrl);
		Api api = apiParser(site, targetUrl, type, params);
		qDebug() << "call" << targetUrl << site << api.name << api.site << params;
		//api.params = Utils::mergeJsonObjects(api.params, params);
		//unknown api
		call(api, functor);
	};

	template<typename Functor>
	void call(int id, APIType type, Functor functor, QJsonObject params = QJsonObject()) {
		QJsonObject item = m_db->getResource(QString::number(id));
		Api api = generator(item["site"].toString(), type, item);
		//api.params = Utils::mergeJsonObjects(api.params, params);
		call(api, functor);
	}

	template<typename Functor>
	void call(Api api, Functor functor) {
		m_resDialog.release();
		m_isClose = false;
		m_resDialog = std::make_unique<ResourceDialog>(qobject_cast<CApplication*>(qApp)->newClient());
		//qDebug() << "Call API1111111:" << api.site << api.name << api.params;
		//if use close res dialog
		if (api.name.isEmpty()) {
			functor(m_resDialog.get(), QJsonObject());
			return;
		}
		
		m_viewmanager->permanentWorker(m_accountmanager->loadCookieJar(api.site), [=](WebView* view, QString scriptid) {
			qDebug() << "Call API:" << api.site << api.name << api.params;
			view->setApiManager(this);
			QJsonDocument params(api.params);
			QString site = api.site;

			if (view->isValid()) {
				view->runWorkerScript(QString("ApiDispather('%1', eval(%2), '_scriptid_placeholder')").arg(api.name, params.toJson()), scriptid);
			}
			else {
				QJsonObject ops;
				ops["onCreated"] = false;
				//add site script here
				view->addScript(site, CApplication::siteScript(site), ops);
				view->setUrl(covertWidToUrl(site));
				m_resDialog->setTitle(QStringLiteral("启动服务..."));
				connect(view, &WebView::loadFinished, [=]() {
					view->runWorkerScript(QString("ApiDispather('%1', eval(%2), '_scriptid_placeholder')").arg(api.name, params.toJson()), scriptid);
					if (errorHandler(view, site)) {
						return;
					}
					view->setValid(1);
					m_db->addAccount(site, QJsonObject(), view->getCookieString());
					emit info("Ready");
					});
			}
			//view->show();
			//when close resDialog, cancel script
			connect(m_resDialog.get(), &ResourceDialog::userClose, [=]() {
				m_isClose = true;
				view->cancelWorkerScript(scriptid);
				qDebug() << "resdialog is closed" << m_isClose;
				});

			connect(view->webObject(), &WebObject::jsComplete, [=](QJsonObject results, QString sid) {
				qDebug() << "[ApiManager] CALL result" << sid << m_isClose;
				//emit notify(QString("worker-%1").arg(sid), json);
				//m_db->addResource(json);
				//if user close res dialog manually
				if (m_isClose) {
					return;
				}
				functor(m_resDialog.get(), results);
				if (results.value("resourceView").toBool()) {
					//open resourseDialog
					m_resDialog->buildList(results);
				}
				else {
					//no resource dialog
					m_resDialog->close();
				}
				});

			connect(view, &WebView::closeView, [=]() {
				//refresh accounts
				qDebug() << "------------call close worker" << view;
				});
			}, api.site, m_parent);
	}

	//worker ID is site
	//return scriptId
	template<typename Functor>
	QString callScript(QString workerId, const QString script, Functor functor) {
		return m_viewmanager->permanentWorker(m_accountmanager->loadCookieJar(workerId), [=](WebView* view, QString scriptid) {
			qDebug() << "callScript" << workerId;
			view->setApiManager(this);
			if (view->isValid()) {
				//donot consider empty script
				view->runWorkerScript(script, scriptid);
			} else {
				view->setUrl(covertWidToUrl(workerId));
				connect(view, &WebView::loadFinished, [=]() {
					view->runWorkerScript(script, scriptid);
					if (errorHandler(view, workerId)) {
						return;
					}
					view->setValid(1);
					m_db->addAccount(workerId, QJsonObject(), view->getCookieString());
					});
			}

			connect(view->webObject(), &WebObject::jsComplete, [=](QJsonObject json, QString _scriptid) {
				qDebug() << "------------sid" << _scriptid;
				//emit notify(QString("worker-%1").arg(sid), json);
				functor(json, _scriptid);
				});

			connect(view, &WebView::closeView, [=]() {
				//refresh accounts
				qDebug() << "------------callScript close worker" << view;
				});

			}, workerId, m_parent);
	}

	template<typename CallBackFunctor>
	void login(const QString & uid, CallBackFunctor functor, bool show = true, QWidget * parent = nullptr){
		m_viewmanager->loginView(uid, [=](WebView* dialog) {
			//dialog->setApiManager(this);

			if (!uid.isEmpty()) {
				dialog->setCookies(m_accountmanager->loadCookieJar(uid));
			}

			dialog->setWindowTitle(QStringLiteral("登录验证窗口-请登录成功后再关闭！"));

			//dialog->addScript(siteObj["nickname"].toString(), siteObj["verifyScript"].toString(), QJsonDocument::fromJson(siteObj["injectOptions"].toString().toUtf8()).object());
			dialog->setUrl(m_landing.value(uid));
			dialog->show();
			//new account login
			//save icon
			connect(dialog, &QWebEngineView::iconChanged, [=](QIcon icon) {
				qDebug() << "=== save icon";
				QString iconpath = CApplication::resourcePath() + "/" + uid + ".png";
				if (!QFileInfo::exists(iconpath)) {
					QPixmap pixmap = icon.pixmap(QSize(32, 32));
					pixmap.save(iconpath, "png");
				}
				});
			//user already login in

			//TODO: fix crash
			connect(dialog, &WebView::closeView, [=]() {
				qDebug() << "=Login Windows CLose";
				//uid format sitenick_realuid
				m_db->addAccount(uid, QJsonObject(), dialog->getCookieString());
				functor();
				});
			}, parent);
	}

	void login(const QString & uid, bool show = true, QWidget * parent = nullptr);

	void workerOperation(WorkerAction act, QString workerId);

	bool errorHandler(WebView* view, const QString site);

	void workerClose(const QString & uid);

	void showWorkerView(const QString workerId);

signals:
	void error(QString);
	void info(QString);

protected:
	QString siteName(const QString& url);
	Api generator(const QString & site, APIType type, const QJsonObject & record, QJsonObject params = QJsonObject());
	Api apiParser(const QString & site, const QString& url, APIType type = APIType::NONE, QJsonObject param = QJsonObject());

	QString typeToName(APIType type);

	QString covertWidToUrl(const QString workerId);

	void parepareWorkers();

private:
	bool m_isClose;
	DBManager* m_db;
	QWidget* m_parent;
	std::unique_ptr<ViewManager> m_viewmanager;
	std::unique_ptr<AccountManager> m_accountmanager;
	std::unique_ptr<ResourceDialog> m_resDialog;

	QMap<QString, QString> m_landing = {
		{"douyin", "https://www.douyin.com/user/self?showTab=like"},
		{"douyinLive", "https://live.douyin.com/error/404"}, // douyin live api
		//{"douyin", "https://live.douyin.com/222"},
		{"xiaohongshu", "https://www.xiaohongshu.com/explore"},
	};
};
