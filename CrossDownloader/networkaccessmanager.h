#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

struct VideoChunk {
	int chunkIndex = 0;
	int status = 0; // 1 success
	int retries = 0;
	int chunkSize = 0;
	QString url;
	QByteArray data;
};

struct VideoObject
{
	QString filepath;
	QJsonObject params;
	int retry = 0;
};

class NetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

public:
	NetworkAccessManager(QObject *parent = nullptr);
	~NetworkAccessManager();

	virtual QNetworkReply* createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );
	QString saveFileName(const QUrl & url);
	bool saveToDisk(const QString &filename, QIODevice *data);
	QByteArray getUrl(const QUrl &url);

	QString download(QUrl url, QString path, QString filename = 0);
	void uploadVideo(QString filepath, QJsonObject params);
	void sendAbort() { emit abort(); }
	bool isAbort() { return m_isAbort; }

	QString resolveUrl(const QUrl & url);

protected:
	void uploadChunk(QNetworkRequest request, QString uid);

signals:
	void abort();
	void uploadSuccess(QString uid, QJsonObject response);
	void uploadFailed(QString uid);

public slots:
	void downloadFinished(QNetworkReply *reply);
	void progress(qint64 bytesReceived, qint64 bytesTotal);

private:
	bool m_isAbort;
	QList<VideoChunk> m_chunks;
	VideoObject m_video;
};

#endif // NETWORKACCESSMANAGER_H
