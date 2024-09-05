#pragma once
#include <qnetworkaccessmanager.h>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QMutex>

struct DownloadInfo {
	int id;
	int status;
	QString url;
	QString filePath;
	QString site;
};

class DownloadHandler :	public QObject
{
	Q_OBJECT
public:
	DownloadHandler();
	~DownloadHandler();

	void addDownload(int id, const QString &url, const QString &filePath)
	{
		if (m_queueIDs.contains(id) || url.isEmpty()) {
			return;
		}
		m_queueIDs.append(id);
		DownloadInfo downloadInfo;
		downloadInfo.id = id;
		//0 wait, 1 downloading, 2 success, 3 stop, -1 error
		downloadInfo.status = avaliable() ? 1 : 0;

		downloadInfo.url = url;
		downloadInfo.filePath = filePath;
		downloadQueue.enqueue(downloadInfo);
		qDebug() << "addDownload" << filePath << id;
		processDownloads();
	}

	template<typename StartFunctor, typename ProgressFunctor, typename FinishFunctor, typename ErrorFunctor>
	void events(StartFunctor startFunctor, ProgressFunctor progressFunctor, FinishFunctor finishFunctor, ErrorFunctor errorFunctor) {
		disconnect();
		
		connect(this, &DownloadHandler::downloadStart, startFunctor);
		connect(this, &DownloadHandler::downloadProgress, progressFunctor);
		connect(this, &DownloadHandler::downloadError, [=](int id, const QString& error) {
			errorFunctor(id, error);
			currentDownloads--;
			removeWatcher(id);
			});
		connect(this, &DownloadHandler::downloadFinished, [=](int id) {
			finishFunctor(id);
			currentDownloads--;
			removeWatcher(id);
			// Additional handling if needed
			processDownloads();
			});

	}

	void processDownloads();

	void setMaxDownloads(int count) { maxConcurrentDownloads = count; };

	void stopAll();

	void stop(int id);

	void resume();

	void remove(int id);
	

signals:
	void downloadStart(int id);
	void downloadFinished(int id);
	void downloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal);
	void downloadError(int id, const QString &errorString);

public slots:
	void startDownload(int id, const QString &url, const QString &filePath);

protected:
	int download(int id, const QString &url, const QString &filePath);
	bool avaliable();
	bool saveFile(QNetworkReply * reply, QString filepath);
	void removeWatcher(int id);

private:


	int maxConcurrentDownloads;
	int currentDownloads;
	QQueue<DownloadInfo> downloadQueue;
	QList<int> m_queueIDs;
	bool m_stop;
	QMap<int, QFutureWatcher<int>*> m_watchers;


};

