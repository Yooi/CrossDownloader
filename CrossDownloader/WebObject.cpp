#include "stdafx.h"
#include "WebObject.h"
#include "WebView.h"


WebObject::WebObject(const WebPage* page)
	: m_page((WebPage*)page)
	, m_apiManager(nullptr)
{

}

WebObject::~WebObject()
{
	qDebug() << "~WebObject" << this;
}

//workerid should be taskId, so multi-tasks support is valid
//if workerid set to be uid, means each account is able to run single task one by one

QString WebObject::worker(QString workerId, QString script)
{
	if (!m_apiManager) {
		return QString();
	}
	return m_apiManager->callScript(workerId, script, [=] (const QJsonObject results, const QString scriptId){
		emit notify(QString("worker-%1").arg(scriptId), results);
		});
}

void WebObject::workerOperation(QString workerId, QString op)
{
	if (op == "close") {
		m_apiManager->workerOperation(WorkerAction::CLOSE, workerId);
	}
	else if (op == "reload") {
		m_apiManager->workerOperation(WorkerAction::RELOAD, workerId);
	}
}

/*
QString WebObject::uploadWorker(QString uid, QString url, QString script, QString wid)
{
	qDebug() << "worker2" << this << uid;
	QJsonObject account = m_page->db()->queryAccount(uid);
	QJsonObject siteObj = m_page->db()->querySite(account["sitenick"].toString());
	//qDebug() << "script " << script;
	QString workerid = uid;
	if (!wid.isEmpty()) {
		workerid = wid;
	}

	return m_viewManager->worker(m_accountManager->loadCookieJar(uid), [=](WebView* dialog, QString scriptid) {
		qDebug() << "----uploadWorker--------" << script;

		//sometimes the loaded pageurl will be changed by site
		//set empty url, run script directly
		if (!url.isEmpty() && dialog->url().toString() != url) {
			QString monitorScript = siteObj["statsScript"].toString();
			//worker inject script after document created
			QJsonObject ops;
			ops["onCreated"] = false;
			dialog->addScript(siteObj["nickname"].toString(), monitorScript.replace("svm_scriptid", scriptid), ops);
			dialog->setUrl(url);

			//when page loadfinished, run worker scirpt
			dialog->disconnect(SIGNAL(loadFinished(bool)));
			connect(dialog, &WebView::loadFinished, [=]() {
				if (!script.isEmpty()) {
					//set scriptid
					dialog->runWorkerScript(script, scriptid);
				}
				});
		}
		else {
			dialog->runWorkerScript(script, scriptid);
		}
		if (CD_DEBUG) {
			dialog->show();
		}


		connect(qobject_cast<WebPage*>(dialog->page())->webObject(), &WebObject::jsComplete, [=](QJsonObject json, QString sid) {
			qDebug() << "------------" << sid;
			emit notify(QString("worker-%1").arg(sid), json);
			//update cookies
			m_page->db()->addAccount(uid, QJsonObject(), dialog->getCookieString());

			if (json["taskSuccess"].toBool()) {
				qDebug() << "---taskSuccess";
				//dialog recycle
				dialog->recycle();
			}
			});

		connect(dialog, &WebView::closeView, [=]() {
			//refresh accounts
			qDebug() << "------------close uploadWorker" << dialog->workerId();
			});


		}, workerid, m_page->view());

}
*/

void WebObject::login(QString uid, bool defaultShow)
{	
	m_apiManager->login(uid, defaultShow, m_page->view());
	//emit loginSite(uid, defaultShow);
}

void WebObject::jsResult(QJsonObject json, QString scriptid)
{
	//qDebug() << ">>>>>>>>>>get fetch result" << json;
	emit jsComplete(json, scriptid);
}

void WebObject::saveCookies(QString uid)
{
	emit save(uid);
}

void WebObject::openUrl(const QString& url)
{
	if (url.startsWith("http")) {
		QDesktopServices::openUrl(QUrl(url));
	}
	else {
		QDesktopServices::openUrl(QUrl::fromLocalFile(url));
	}
}

void WebObject::reload()
{
	emit notify("refresh");
}