#pragma once
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QMutex>
#include "ui_downloader.h"
#include <qnetworkreply.h>
#include "networkaccessmanager.h"

class DownloadManager : public QDialog
{
	Q_OBJECT
public:
	
	enum DownloadStatus
	{
		SUCCESS,
		INFO,
		ERR,
	};

	DownloadManager(QWidget *parent = nullptr);
	~DownloadManager();

	//void downloadFile(const QString &url, const QString &filePath);

	//void cancelDownloads();
	void downloadFile(const QString & url, const QString & filePath);
	void cancelDownloads();

	void setCookies(NetworkAccessManager* manager, QJsonObject item);

	QString elidedText(QString text, int width);

	void start(const QJsonObject & json);

	void setDownloadFileIcon(QJsonObject item);

	void buildRequestHeaders(QNetworkRequest * request, QJsonObject downloadObj);

	QString buildFileName(const QJsonObject & item);

	QString getUniqueFileName(const QString & filePath);

	QString humanReadableNum(qint64 size, bool isSpeed = false);


signals:
	void downloadProgress(int);
	void success(QJsonObject);
	void downloadError(const QString &errorString);


protected slots:
	void doDownload(const QJsonObject &, const QString & filePath);

	void onDownloadFinished(QNetworkReply * reply);

	void onReadyRead();

	void closeDialog();

	QByteArray decompressReply(QNetworkReply * reply);

	QPair<int, QByteArray> downloadSegment(int index, const QUrl & url);

	void onSegmentDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

	void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

	void onErrorOccurred(QNetworkReply::NetworkError error);

	void pauseDownload();

	void resumeDownload();

	QStringList m3u8Parser(QString m3u8Url, QString content);


	void downloadAndMergeTSFiles(const QJsonObject & item, const QString & outputFilePath);

	void downloadFiles(QStringList tsUrls, QString outputFilePath);

	void mergeFiles(QList<QByteArray> chunks, QString outputFilePath);

	void setStatusLabel(QString text, DownloadStatus type = INFO);

	void saveResource();
private:
	QList<QFuture<void>> m_futures;
	QMutex m_mutex;

	bool m_closeRequested = false;
	bool m_cancelled = false;
	NetworkAccessManager* m_networkManager;
	QFile* m_file;
	QString m_fileBaseName;
	QString m_outputFilePath;
	QNetworkReply *m_reply;
	QUrl m_url;
	QDateTime m_startTime;
	int m_finishedSegments;
	int m_downloadTime;
	qint64 m_downloadedBytes;
	qint64 m_downloadSpeed;
	qint64 m_downloadSize;
	qint64 m_resumeBytes;
	QElapsedTimer m_elapsedTimer;

	QFuture<QMap<int, QByteArray>> m_future;
	QFutureWatcher<QMap<int, QByteArray>> m_watcher;
	QJsonObject m_downloadObject;
	QNetworkCookieJar* m_cookiejar;

	QList<DWORD> m_threadIDs;
	int m_downloadFinishedCount;
	Ui::Downloader ui;
	
};

