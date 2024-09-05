#pragma once

#include "WebView.h"

class ViewManager :	public QObject
{
public:

	ViewManager();
	//default type worker, [worker, permanentWorker]
	WebView * createWorker(QString workerid, QString workerType = "worker", QWidget* parent = nullptr);
	void setupConnection(WebView* view);
	bool getOrCreateWorker(QString workerid, QString workerType, QWidget* parent, WebView*& view);
	bool getOrCreateWorker(QString workerid, bool isPermanent, QWidget* parent, WebView*& view);

	WebView * getWorker(QString workerid);

	void removeWorker(QString workerid);
	bool exists(QString workerid);
	void cleanWorkers();

	~ViewManager() {
		qDebug() << "~~ViewManager";
		//cleanWorkers();
	}

	/***********************************
	example for how to call worker in client
	JS code

	// uid : target userid, used for init the worker via use info
	// jsFunction : the function that will be called in the worker
	// args : the arguments of the function, format: [arg1, arg2, ...]
	export const worker = function(uid, jsFunction, args, callback){
		var serialized = JSON.stringify({
			fn: jsFunction.toString(),
			args: args
		});
		//attendtion, remotescript should be wrapped by setTimeout, otherwise the script will not work
		const remoteScript = `var data = ${serialized}; var fn = new Function('return ' + data.fn)();fn.apply(null, data.args);`
		//`setTimeout(() => {(${jsFunction.toString()})(${args})}, 1);`
		//because of the worker cannot impliment the function imidiately, so we need to use setTimeout to call the function
		window.svm.worker(uid, remoteScript, scriptid=>{
			emitter.on("worker-"+scriptid, (res)=>{
				callback(res)
				emitter.off("worker-"+scriptid)
			})
		})
	}

	***********************************/

	//worker is used for runjavascript cross site
	//step 1, setcookie
	//step 2, runJavascript
	//step 3, get Results and cleanup
	//Input: 
	// cookie
	// functor --- callback function
	// workerid -- identify which worker, usually set to be userid
	// parent
	//Output:
	// scriptid -- identify which user script was running
	template<typename Functor>
	QString worker(QList<QNetworkCookie> cookies, Functor functor, QString workerid = "worker", QWidget* parent = nullptr) {
		QString scriptid = QString::number(QDateTime::currentMSecsSinceEpoch());
		WebView* view = nullptr;
		bool isCreated = getOrCreateWorker(workerid, false, parent, view);

		setupConnection(view);

		if (isCreated) {
			//disconnect signals, because worker will be called mulipe-times
			//forbidden to recreate connection
			view->setCookies(cookies);
		}
		
		functor(view, scriptid);

		return scriptid;
	};

	//workerid is uid or siteId
	template<typename Functor>
	QString permanentWorker(QList<QNetworkCookie> cookies, Functor functor, QString workerid, QWidget* parent = nullptr) {
		QString scriptid = QString::number(QDateTime::currentMSecsSinceEpoch());
		WebView* view = nullptr;
		bool isCreated = getOrCreateWorker(workerid, true, parent, view);

		setupConnection(view);

		if (isCreated) {
			view->setCookies(cookies);
		}

		// if view is error, just return
		if (view->error()) {
			return scriptid;
		}

		functor(view, scriptid);
		return scriptid;
	}

	template<typename Functor>
	QString loginView(QString uid, Functor functor, QWidget * parent = nullptr) {
		QString workerid = QString::number(QDateTime::currentMSecsSinceEpoch());
		auto view = new WebView("newAccount", parent, 0);
		//view->deleteLater();
		//return 0;
		//auto dialog = getOrCreateWorker(uid + "login", "newAccount", parent);

		//qDebug() << ">>>>is off-the-record" << view->profile()->isOffTheRecord() << objectRate;

		view->setWindowFlags(Qt::Dialog);
		view->setWindowModality(Qt::WindowModal);
		view->setFixedSize(1600, 900);

		functor(view);
		return workerid;
	}

private:
	QMap<QString, WebView*> m_workers; //worker id, worker ptr
};

