#include "stdafx.h"
#include "networkaccessmanager.h"
#include <QEventLoop>

NetworkAccessManager::NetworkAccessManager(QObject *parent)
	: QNetworkAccessManager(parent)
	, m_isAbort(false)
{
}

NetworkAccessManager::~NetworkAccessManager()
{
	qDebug() << "NetworkAccessManager::~NetworkAccessManager" << this;
}

QNetworkReply* NetworkAccessManager::createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData)
{
    QNetworkRequest request = req; // copy so we can modify
    // this is a temporary hack until we properly use the pipelining flags from QtWebkit
    // pipeline everything! :)

	QString referrer = request.rawHeader("Referer"), origin = request.rawHeader("Origin");
	if (referrer.isEmpty()) {
		referrer = request.url().host().prepend(request.url().scheme().append("://"));
	}
	
	request.setAttribute(QNetworkRequest::CookieLoadControlAttribute, false);

	if (request.header(QNetworkRequest::UserAgentHeader).isNull()) {
		request.setHeader(QNetworkRequest::UserAgentHeader, AGENT);
	}

	if (request.rawHeader("Referer").isNull()) {
		qDebug() << "NetworkAccessManager::createRequest set default Referer value";
		request.setRawHeader(QByteArray("Referer"), referrer.toLatin1());
		request.setRawHeader(QByteArray("Origin"), referrer.toLatin1());
	}

	qDebug() << "[createRequest----------HTTP Request]:" << request.url() << referrer;

    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

QString NetworkAccessManager::saveFileName(const QUrl &url)
{
	QString path = url.path();
	QString basename = QFileInfo(path).fileName();

	if (basename.isEmpty())
		basename = "download";

	if (QFile::exists(basename)) {
		// already exists, don't overwrite
		int i = 0;
		basename += '.';
		while (QFile::exists(basename + QString::number(i)))
			++i;

		basename += QString::number(i);
	}

	return basename;
}

bool NetworkAccessManager::saveToDisk(const QString & filename, QIODevice * data)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		fprintf(stderr, "Could not open %s for writing: %s\n",
			qPrintable(filename),
			qPrintable(file.errorString()));
		return false;
	}

	file.write(data->readAll());
	file.close();

	return true;
}

QByteArray NetworkAccessManager::getUrl(const QUrl & url)
{
	QString referrer = url.host().prepend(url.scheme().append("://"));

	QNetworkRequest request(url);
	request.setRawHeader("Accept", "*/*");
	request.setRawHeader("Accept-Encoding", "deflate, br");
	request.setRawHeader("Accept-Language", "zh-CN,zh;q=0.9,en;q=0.8");
	request.setRawHeader("Cache-Control", "no-cache");
	request.setRawHeader("User-Agent", AGENT);
	request.setRawHeader("Origin", referrer.toUtf8());
	request.setRawHeader("Referer", referrer.toUtf8());

	QNetworkReply* reply = this->get(request);
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(progress(qint64, qint64)));
	connect(this, &NetworkAccessManager::abort, [=]() {
		reply->abort();
		m_isAbort = true;
		reply->deleteLater();
		qDebug() << ">>abort download!";
	});
	loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        return reply->readAll();
    }
    qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << reply->error() << reply->readAll().length();
    return QByteArray();
}

void NetworkAccessManager::progress(qint64 bytesReceived, qint64 bytesTotal)
{

}

QString NetworkAccessManager::download(QUrl url, QString path, QString filename)
{
    if (filename.isEmpty()) {
        filename = saveFileName(url);
    }
	QByteArray data = getUrl(url);
	//qDebug() << "Download Data " << data.isEmpty();
    QFile file(path+"/"+filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
            qPrintable(filename),
            qPrintable(file.errorString()));
        return QString();
    }

    file.write(data);
    file.close();

	return path + "/" + filename;
}

void NetworkAccessManager::downloadFinished(QNetworkReply * reply)
{
	QUrl url = reply->url();
	if (reply->error()) {
		fprintf(stderr, "Download of %s failed: %s\n",
			url.toEncoded().constData(),
			qPrintable(reply->errorString()));
	}
	else {
		QString filename = saveFileName(url);
		if (saveToDisk(filename, reply))
			printf("Download of %s succeded (saved to %s)\n",
				url.toEncoded().constData(), qPrintable(filename));
	}
}

void NetworkAccessManager::uploadVideo(QString filepath, QJsonObject params) 
{
	if (m_video.retry >= 1) {
		m_video.retry = 0;
		return;
	}
	m_video.filepath = filepath;
	m_video.params = params;

	if (!QFile::exists(filepath)) {
		return;
	}
	QFile file(filepath);
	if (!file.open(QIODevice::ReadOnly))
		return;
	QByteArray rawData = file.readAll();
	file.close();

	int rawSize = rawData.count();
	m_chunks.clear();
	QString uid = params["uid"].toString();
	if (uid.isEmpty()) {
		return;
	}
	int chunkSize = params["chunkSize"].toInt();
	if (chunkSize == 0) {
		chunkSize = 8388608;
	}
	int totalChunkCount = rawSize / chunkSize;
	if (rawSize % chunkSize > 0) {
		totalChunkCount++;
	}
	qDebug() << "uploadVideo>" << totalChunkCount << chunkSize;
	for (size_t chunkIndex = 0; chunkIndex < totalChunkCount; chunkIndex++)
	{
		int start = chunkIndex * chunkSize;
		int end = qMin(start + chunkSize, rawSize);
		QByteArray data = rawData.mid(chunkIndex*chunkSize, chunkSize);
		VideoChunk chunk;
		chunk.chunkIndex = chunkIndex;
		chunk.chunkSize = chunkSize;
		chunk.data = data;
		chunk.status = 0;
		chunk.url = params["url"].toString();
		m_chunks.append(chunk);
	}

	//QUrl url = params["url"].toString();
	QNetworkRequest request;
	request.setRawHeader("Accept", "application/json");
	request.setRawHeader("Accept-Encoding", "deflate, br");
	request.setRawHeader("Accept-Language", "zh-CN,zh;q=0.9,en;q=0.8");
	request.setRawHeader("Cache-Control", "no-cache");
	request.setRawHeader("X-Requested-With", "XMLHttpRequest");
	request.setRawHeader("User-Agent", AGENT);
	request.setRawHeader("Origin", "https://weibo.com/");
	request.setRawHeader("Referer", "https://weibo.com/");
	
	QJsonObject headers = params["headers"].toObject();
	foreach (QString key , headers.keys())
	{
		request.setRawHeader(key.toUtf8(), headers.value(key).toString().toUtf8());
	}
	uploadChunk(request, uid);
}

void NetworkAccessManager::uploadChunk(QNetworkRequest request, QString uid)
{
	VideoChunk chunk = m_chunks.takeFirst();
	QUrlQuery query;
	query.addQueryItem("startloc", QString::number(chunk.chunkIndex * chunk.chunkSize));
	query.addQueryItem("sectioncheck", QString(QCryptographicHash::hash(chunk.data, QCryptographicHash::Md5).toHex()));
	request.setUrl(chunk.url + "&" + query.toString());
	
	QNetworkReply* reply = this->post(request, chunk.data);
	connect(reply, &QNetworkReply::finished, [=]() {
		if (m_chunks.isEmpty()) {
			//success
			if (reply->error() == QNetworkReply::NoError) {
				QByteArray ret = reply->readAll();
				QJsonObject retObj = QJsonDocument::fromJson(ret).object();
				qDebug() << "result>>>" << retObj << retObj["fid"].isNull();
				if (retObj["fid"].isNull()) {
					//fail and retry once
					//uploadVideo(m_video.filepath, m_video.params);
					//m_video.retry++;
				}
				else {
					m_video.retry = 0;
					emit uploadSuccess(uid, retObj);
				}
			}
			reply->deleteLater();
			return;
		}
		reply->deleteLater();
		//run next
		uploadChunk(request, uid);
	});
}

//convert short url to long url
QString NetworkAccessManager::resolveUrl(const QUrl &url)
{
	QNetworkRequest request(url);
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	QNetworkReply *reply = this->get(request);

	// Wait for the request to finish
	QEventLoop loop;
	connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	loop.exec();

	if (reply->error() == QNetworkReply::NoError) {
		// Get the final URL after following redirects
		QUrl resolvedUrl = reply->url();
		return resolvedUrl.toString();
	}
	else {
		qDebug() << "Error:" << reply->errorString();
		return url.toString();
	}
}