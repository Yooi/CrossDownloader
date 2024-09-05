#include "stdafx.h"
#include "DownloadHandler.h"
#include <QNetworkReply>

DownloadHandler::DownloadHandler()
	:currentDownloads(0)
	, maxConcurrentDownloads(3)
	, m_stop(false)
{
}


DownloadHandler::~DownloadHandler()
{
	
}

int DownloadHandler::download(int id, const QString &url, const QString &filePath) 
{
	NetworkAccessManager manager;
	QUrl downloadUrl = manager.resolveUrl(url);

	QNetworkRequest request(downloadUrl);
	request.setRawHeader("Connection", "Keep-Alive");
	request.setRawHeader("User-Agent", AGENT);

	QNetworkReply *reply = manager.get(request);
	// Set up a timer for timeout
	/*
	QTimer timeoutTimer;
	timeoutTimer.setSingleShot(true);
	timeoutTimer.start(60 * 1000);  // Convert seconds to milliseconds

	connect(&timeoutTimer, &QTimer::timeout, this, [=]() {
		reply->abort();
		emit downloadError(id, "Download timeout");
		});
	*/
	emit downloadStart(id);

	QEventLoop loop;
	connect(reply, &QNetworkReply::downloadProgress, this, [&](qint64 bytesReceived, qint64 bytesTotal) {
		//timeoutTimer.start(60 * 1000);
		emit downloadProgress(id, bytesReceived, bytesTotal);
		});

	connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	connect(reply, &QNetworkReply::errorOccurred, [&](QNetworkReply::NetworkError code) {
		qDebug() << "errorOccurred" << code;
		emit downloadError(id, "error");
		reply->disconnect();
		loop.quit();
		});

	// Connect the canceled signal to handle cancellation
	connect(m_watchers.value(id), &QFutureWatcherBase::canceled, [&]() {
		qDebug() << "Download canceled for ID" << id;
		reply->abort();
		loop.quit();
		});

	connect(m_watchers.value(id), &QFutureWatcherBase::finished, [&]() {
		qDebug() << "Download finished for ID" << id;
		});
	// Wait for the download to finish
	loop.exec();

	if (saveFile(reply, filePath)) {
		emit downloadFinished(id);
	}

	//timeoutTimer.stop();
	reply->deleteLater();
	return id;
}

bool DownloadHandler::avaliable()
{
	return currentDownloads < maxConcurrentDownloads && !m_stop;
}

bool DownloadHandler::saveFile(QNetworkReply *reply, QString filepath)
{
	if (reply->error() != QNetworkReply::NoError) {
		return false;
	}
	QFile file(filepath);
	if (file.open(QIODevice::WriteOnly)){
		//file.write(reply->readAll());
		while (!reply->atEnd()) {
			file.write(reply->read(20485760)); // Öð¿éÐ´Èë20m
		}
		file.close();
		return true;
	}

	return false;
}

void DownloadHandler::removeWatcher(int id)
{
	qDebug() << "[DownloadHandler::removeWatcher]" << id;
	m_watchers.take(id)->deleteLater();
	m_queueIDs.removeAll(id);
}

void DownloadHandler::startDownload(int id, const QString &url, const QString &filePath) 
{
	qDebug() << "DownloadHandler::startDownload" << currentDownloads << id;
	// Use QtConcurrent to run the download function in a separate thread
	QFuture<int> future = QtConcurrent::run(this, &DownloadHandler::download, id, url, filePath);
	QFutureWatcher<int>* watcher = new QFutureWatcher<int>();
	m_watchers.insert(id, watcher);

	watcher->setFuture(future);

	currentDownloads++;
}


void DownloadHandler::processDownloads() 
{
	while (avaliable() && !downloadQueue.isEmpty() ) {
		DownloadInfo downloadInfo = downloadQueue.dequeue();
		//process waiting item
		if (downloadInfo.status != 2) {
			startDownload(downloadInfo.id, downloadInfo.url, downloadInfo.filePath);
		}
	}
}

void DownloadHandler::stopAll()
{
	m_stop = true;
	foreach(auto id, m_watchers.keys()) {
		stop(id);
	}
}

void DownloadHandler::stop(int id) 
{
	qDebug() << ">>>stop" << id;
	if (m_watchers.contains(id)) {
		QFutureWatcher<int>* watcher = m_watchers.take(id);
		watcher->cancel();
		watcher->deleteLater();
	}
}

void DownloadHandler::resume()
{
	m_stop = false;
	processDownloads();
}

void DownloadHandler::remove(int id)
{
	//remove from queue
	stop(id);
	foreach(DownloadInfo item, downloadQueue) {
		//if (id_ == id) {
		//	downloadQueue.removeOne(item);
		//}
	}

}
