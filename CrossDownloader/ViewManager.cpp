#include "stdafx.h"
#include "ViewManager.h"

ViewManager::ViewManager()
{
	
}

WebView* ViewManager::createWorker(QString workerid, QString workerType, QWidget* parent)
{
	//use off-the-record model
	auto view = new WebView(workerType, workerid, parent, 0);
	view->setWindowFlags(Qt::Window);
	view->setWindowModality(Qt::WindowModal);
	view->setFixedSize(1200, 900);
	view->setWindowTitle("Worker");

	const QSize MAIN_SIZE_MAX = QSize(16777215, 16777215);
	view->setMaximumSize(MAIN_SIZE_MAX);

	//dialog->show();
	qDebug() << "===================create worker" << workerid << view << this << QThread::currentThreadId();
	m_workers.insert(workerid, view);

	return view;
}

void ViewManager::setupConnection(WebView* view)
{
	view->webObject()->disconnect();
	view->disconnect();

	connect(view, &WebView::recycle, [=]() {
		qDebug() << "==worker recycle" << view->workerId();
		removeWorker(view->workerId());
		});
}

bool ViewManager::getOrCreateWorker(QString workerid, QString workerType, QWidget* parent, WebView*& view)
{
	auto val = getWorker(workerid);
	if (val) {
		view = val;
		return false;
	}

	view = createWorker(workerid, workerType, parent);
	return true;
}

/* create true, get false */
bool ViewManager::getOrCreateWorker(QString workerid, bool isPermanent, QWidget* parent, WebView*& view)
{
	auto val = getWorker(workerid);
	if (val) {
		view = val;
		return false;
	}

	view = createWorker(workerid, isPermanent ?"permanentWorker":"worker", parent);
	return true;
}

WebView* ViewManager::getWorker(QString workerid)
{
	return m_workers.value(workerid);
}

void ViewManager::removeWorker(QString workerid)
{
	if (!exists(workerid)) {
		return;
	}
	//because sometimes use will close view manually
	try
	{
		qDebug() << "ViewManager::removeWorker" << workerid << QThread::currentThreadId();
		auto view = m_workers.take(workerid);
		view->deleteLater();
	}
	catch (const std::exception&)
	{

	}
}

bool ViewManager::exists(QString workerid)
{
	return m_workers.contains(workerid);
}

void ViewManager::cleanWorkers()
{
	for (QString workerid : m_workers.keys()) {
		removeWorker(workerid);
	}
}
