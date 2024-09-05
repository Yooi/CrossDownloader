#include "stdafx.h"
#include "DownloadManager.h"
#include "CApplication.h"
#include <QNetworkCookie>
#include <qnetworkcookiejar.h>
#include <QtConcurrent>
#include "networkaccessmanager.h"
#include <QThread>

DownloadManager::DownloadManager(QWidget *parent)
	:QDialog(parent)
	, m_networkManager(new NetworkAccessManager)
	, m_downloadedBytes(0)
	, m_downloadSize(0)
	, m_resumeBytes(0)
	, m_finishedSegments(0)
	, m_downloadTime(0)
	, m_downloadSpeed(0)
	, m_file(nullptr)
	, m_reply(nullptr)
	, m_cookiejar(new QNetworkCookieJar)
{
	ui.setupUi(this);
	setWindowState(Qt::WindowActive | Qt::WindowMinimized );
	setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
	show();

	m_networkManager->setCookieJar(m_cookiejar);

	showNormal();
	QTimer::singleShot(0, this, &QWidget::activateWindow);
	
	//setAttribute(Qt::WA_DeleteOnClose, true);

	//connect(m_networkManager, &QNetworkAccessManager::finished, this, &DownloadManager::onDownloadFinished);
	connect(ui.pushButton_File, &QPushButton::clicked, [=]() {
		QDesktopServices::openUrl(QUrl::fromLocalFile(m_outputFilePath));
	});

	connect(ui.pushButton_Folder, &QPushButton::clicked, [=]() {
		QFileInfo fi(m_outputFilePath);
		//open folder
		//select target file
		QString explorerPath = "explorer.exe";
		QStringList args;
		args << "/select," + QDir::toNativeSeparators(m_outputFilePath);
		QProcess::startDetached(explorerPath, args);
	});

	connect(ui.pushButton_Ok, &QPushButton::clicked, this, &DownloadManager::closeDialog);
	connect(ui.pushButton_Cancel, &QPushButton::clicked, this, &DownloadManager::closeDialog);
}

DownloadManager::~DownloadManager()
{
	qDebug() << "DownloadManager::~DownloadManager";
}

void DownloadManager::downloadFile(const QString &url, const QString &filePath)
{
	//QFuture<void> future = QtConcurrent::run(this, &DownloadManager::doDownload, url, filePath);
	//m_futures.append(future);
}

void DownloadManager::cancelDownloads()
{
	m_cancelled = true;
	for (int i = 0; i < m_futures.size(); ++i) {
		m_futures[i].waitForFinished();
	}
}


void DownloadManager::setCookies(NetworkAccessManager* manager, QJsonObject item)
{
	if (!item["cookies"].toString().isEmpty()) {
		QString cookiestring = item["cookies"].toString();
		QList<QNetworkCookie> cookies;

		qDebug() << "DownloadManager::setCookies" << cookies;
		
		for each (auto splitcookie in cookiestring.split("; "))	{
			cookies.append(QNetworkCookie::parseCookies(splitcookie.toUtf8()));
		}
		if (cookies.isEmpty()) {
			qDebug() << "Failed to parse cookies";
			return;
		}
		m_cookiejar->setCookiesFromUrl(cookies, item["referer"].toString());
		manager->setCookieJar(m_cookiejar);
	}
}

QString DownloadManager::elidedText(QString text, int width)
{
	QFontMetrics metrics(ui.source->font());
	return metrics.elidedText(text, Qt::ElideRight, ui.source->width());
}

void DownloadManager::start(const QJsonObject & json)
{	
	m_downloadObject = json;
	QString subject = json["title"].toString().isEmpty() ? json["pageTitle"].toString() : json["title"].toString();
	ui.source->setText(elidedText(json["source"].toString(), ui.source->width()));
	ui.subject->setText(subject);
	setDownloadFileIcon(json);
	
	//generate filename
	QString path = QUrl(json["source"].toString()).path();
	QString filename = buildFileName(json);
	m_outputFilePath = getUniqueFileName(qobject_cast<CApplication*>(qApp)->downloadPath() + "/" + filename);
	m_startTime = QDateTime::currentDateTime();
	m_elapsedTimer.start();

	m_downloadObject["filepath"] = m_outputFilePath;

	//streaming file, e.g. ts
	if (json["type"].toString() == "hls") {
		downloadAndMergeTSFiles(json, m_outputFilePath);
	}
	else {
		doDownload(json, m_outputFilePath);
	}
	
}

void DownloadManager::setDownloadFileIcon(QJsonObject item)
{
	// Get the MIME type of the file
	QString suffix = item["fileType"].toString();

	QTemporaryFile tempFile;
	tempFile.setFileTemplate("tempfile.XXXXXX." + suffix);
	tempFile.open();
	
		

	// Get the file icon provider                                               
	QFileIconProvider iconProvider;

	// Get the file icon based on the MIME type
	QIcon fileIcon = iconProvider.icon(tempFile);

	//show icon in dialog
	QPixmap pixmap = fileIcon.pixmap(QSize(42, 42));
	ui.typeIcon->setPixmap(pixmap);

	// Clean up the temporary file
	tempFile.close();
	tempFile.remove();
}

void DownloadManager::buildRequestHeaders(QNetworkRequest* request, QJsonObject downloadObj)
{
	request->setRawHeader("Accept", "*/*");
	request->setRawHeader("Accept-Encoding", "gzip, deflate");
	request->setRawHeader("Accept-Language", "en,zh-CN;q=0.9,zh;q=0.8");
	request->setRawHeader("Cache-Control", "no-cache");
	request->setRawHeader("User-Agent", downloadObj["agent"].toString().toUtf8());
	request->setRawHeader("Origin", downloadObj["origin"].toString().toUtf8());
	request->setRawHeader("Referer", downloadObj["referer"].toString().toUtf8());
}

QString DownloadManager::buildFileName(const QJsonObject & item)
{
	QString path = QUrl(item["source"].toString()).path();
	QString filename;
	QString suffix = item["fileType"].toString();
	if (filename.isEmpty()) {
		filename = QString("%1.%2")
			.arg(item["title"].toString().isEmpty()? item["pageTitle"].toString(): item["title"].toString())
			.arg(suffix);

	}else if (!filename.endsWith("."+suffix)) {
		filename += "." + suffix;
	}
	
	return Utils::makeValidWindowsFilename(filename);
}

QString DownloadManager::getUniqueFileName(const QString& filePath)
{
	QString baseName = QFileInfo(filePath).baseName();
	QString suffix = QFileInfo(filePath).suffix();
	QString path = QFileInfo(filePath).absolutePath();

	int counter = 1;
	QString uniqueFilePath = filePath;

	while (QFile::exists(uniqueFilePath)) {
		QString newBaseName = QString("%1(%2)").arg(baseName).arg(counter);
		uniqueFilePath = QDir(path).filePath(newBaseName + "." + suffix);
		counter++;
	}
	m_fileBaseName = QFileInfo(filePath).baseName() + "." + suffix;
	return uniqueFilePath;
}


QString DownloadManager::humanReadableNum(qint64 size, bool isSpeed)
{
	static const QStringList units = { "B", "KB", "MB", "GB", "TB", "PB" };
	int unitIndex = 0;
	double fileSize = static_cast<double>(size);

	while (fileSize >= 1024 && unitIndex < units.size() - 1) {
		fileSize /= 1024;
		++unitIndex;
	}

	if (isSpeed) {
		return QString("%1 %2/s").arg(fileSize, 0, 'f', 2).arg(units[unitIndex]);
	}
	// Format the file size to display up to 2 decimal places
	return QString("%1 %2").arg(fileSize, 0, 'f', 2).arg(units[unitIndex]);
}


void DownloadManager::doDownload(const QJsonObject & item, const QString &outputFilePath)
{
	qDebug() << "DownloadManager::doDownload" << outputFilePath;
	// download the file here
	// emit signals for progress, finished, and error

	QFile outputFile;
	m_url = item["source"].toString();
	m_file = new QFile(outputFilePath);
	qDebug() << "<<" << outputFilePath;
	if (!m_file->open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open file for writing:" << outputFilePath;
		delete m_file;
		m_file = nullptr;
		return;
	}

	
	QNetworkRequest request(m_url);
	buildRequestHeaders(&request, item);

	//resume file download
	if (m_file->size() > 0) {
		m_resumeBytes = m_file->size();
		// Set the Range header to resume the download
		//request.setRawHeader("Range", QString("bytes=%1-").arg(m_resumeBytes).toUtf8());
	}
	request.setRawHeader("CD", "1");
	if (!item["cookies"].toString().isEmpty()) {
		request.setRawHeader("Cookie", item["cookies"].toString().toUtf8());
	}
	//setCookies(m_networkManager, item);
	
	m_reply = m_networkManager->get(request);
	qint64 downloadedBytes = 0;
	QEventLoop loop;
	connect(m_reply, &QNetworkReply::readyRead, this, &DownloadManager::onReadyRead);
	connect(m_reply, &QNetworkReply::downloadProgress, this, &DownloadManager::onDownloadProgress);
	connect(m_reply, &QNetworkReply::finished, [&]() {
		loop.quit();
	});
	connect(m_reply, &QNetworkReply::errorOccurred, [&](QNetworkReply::NetworkError code) {
		loop.quit();
		});
	loop.exec();

	onDownloadFinished(m_reply);
	m_reply->deleteLater();
}

void DownloadManager::onDownloadFinished(QNetworkReply *reply)
{
	bool success = true;
	m_file->close();
	if (reply->error() == QNetworkReply::NoError) {
		//m_file->write(m_downloadData);
		//download success
		setStatusLabel("Success", SUCCESS);
	}
	else {
		qDebug() << "Download failed:" << reply->errorString();
		m_file->remove();
		setStatusLabel("Failed", ERR);
		success = false;
	}
	
	if (success) {
		ui.pushButton_File->setEnabled(true);
		ui.pushButton_Folder->setEnabled(true);
	}

	m_downloadTime = m_startTime.secsTo(QDateTime::currentDateTime());
}

void DownloadManager::onReadyRead()
{
	if (m_reply) {
		//qDebug() << "DownloadManager::onReadyRead";
		QByteArray data = decompressReply(m_reply);
		m_downloadedBytes += data.size();

		//m_downloadData.append(data);
		m_file->write(data);

		m_downloadSpeed = m_downloadedBytes * 1000 / m_startTime.msecsTo(QDateTime::currentDateTime());
		ui.speed->setText(humanReadableNum(m_downloadSpeed, true));
		ui.downloaded->setText(humanReadableNum(m_downloadedBytes));
	}
}

void DownloadManager::closeDialog()
{
	//download TS file
	if (m_watcher.isRunning()) {
		m_watcher.cancel();
		reject();
	}
	//dowload normal file
	else {
		saveResource();
		//m_reply->abort();
		close();
	}
	m_closeRequested = true;
}

QByteArray DownloadManager::decompressReply(QNetworkReply* reply)
{
	QVariant contentEncoding = reply->rawHeader("Content-Encoding");
	bool isGzipCompressed = (contentEncoding.toString().toLower() == "gzip");
	QByteArray content = reply->readAll();
	
	if (isGzipCompressed) {
		// Decompress the GZIP-compressed data
		return Utils::decompressGzip(content);
		// Process the decompressed data
		// ...
	}
	return content;
}

void DownloadManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{	
	if (m_downloadSize == 0) {
		m_downloadSize = bytesTotal;
		if (m_downloadSize > 0) {
			ui.filesize->setText(humanReadableNum(bytesTotal));
		}
		else {
			ui.filesize->setText("--");
		}
		setStatusLabel("Downloading...");
	}
	ui.progressBar->setRange(m_resumeBytes, bytesTotal);
	ui.progressBar->setValue(bytesReceived);
	
	if (bytesTotal > 0) {
		setWindowTitle(QString("%1% %2").arg(int((static_cast<double>(bytesReceived) / bytesTotal) * 100)).arg(m_fileBaseName));
	}
	else {
		setWindowTitle(QString("%1 %2").arg(humanReadableNum(m_downloadedBytes)).arg(m_fileBaseName));
	}
	//emit downloadProgress(bytesReceived, bytesTotal);
}

void DownloadManager::onErrorOccurred(QNetworkReply::NetworkError error)
{
	ui.status->setText("Error");
}

void DownloadManager::pauseDownload()
{
	if (m_reply && m_reply->isRunning()) {
		m_reply->abort();
		m_file->flush();
		setStatusLabel("Pause");
	}
}

void DownloadManager::resumeDownload()
{
	if (m_reply && !m_reply->isRunning()) {
		QNetworkRequest request(m_reply->url());

		buildRequestHeaders(&request, m_downloadObject);

		request.setRawHeader("Range", QString("bytes=%1-").arg(m_resumeBytes).toUtf8());
		m_reply = m_networkManager->get(request);

		connect(m_reply, &QNetworkReply::downloadProgress, this, &DownloadManager::onDownloadProgress);
		connect(m_reply, &QNetworkReply::readyRead, this, &DownloadManager::onReadyRead);
	}
}

QStringList DownloadManager::m3u8Parser(QString m3u8Url, QString m3u8Content)
{
	//qDebug() << ">>>>check" << m3u8Content;
	// 解析M3U8文件内容
	QStringList lines = m3u8Content.split("\n");
	QStringList tsUrls;
	for (auto line : lines) {
		if (line.trimmed().isEmpty() || line.startsWith("#")) {
			continue; // 跳过注释行
		}
		else {
			// 将相对URL转换为绝对URL
			QUrl url(line);
			if (url.isRelative()) {
				QUrl absoluteUrl = QUrl(m3u8Url);
				// if url such as /test.ts
				QString replacedPath = line;

				if (!line.startsWith("/")) {
					// if url such as test.ts
					QRegularExpression regex("/([^/]+)(\\?.*)?$");
					QString currentPath = absoluteUrl.path();
					replacedPath = currentPath.replace(regex, "/" + line);
				}
				qDebug() << line;
				absoluteUrl.setPath(replacedPath);
				url = absoluteUrl;
			}

			tsUrls.append(url.toString());
		}
	}
	return tsUrls;
}

//Issues: some server wont send header info by m_networkManager->head, so cannot get total size of ts files
//Handle: we donot show File Size for ts files
void DownloadManager::downloadAndMergeTSFiles(const QJsonObject & item, const QString& outputFilePath)
{
	QString m3u8Url = item["source"].toString();

	QNetworkRequest request(m3u8Url);
	buildRequestHeaders(&request, item);
	setCookies(m_networkManager, item);

	ui.label_7->setText("Segments");

	// 创建网络请求和回复对象
	QNetworkReply* reply = m_networkManager->get(request);

	// 等待网络请求完成
	connect(reply, &QNetworkReply::finished, [=]() {
		if (reply->error() != QNetworkReply::NoError) {
			qDebug() << "Error: " << reply->errorString();
			return;
		}

		// 读取M3U8文件内容
		QString m3u8Content = decompressReply(reply);
		reply->deleteLater();

		QStringList tsUrls = m3u8Parser(m3u8Url, m3u8Content);
		qDebug() << "downloadAndMergeTSFiles" << m_outputFilePath;
		downloadFiles(tsUrls, m_outputFilePath);
		});

}

void mergeFilesInOrder(QMap<int, QByteArray> &result, const QPair<int, QByteArray> &segment) {
	result.insert(segment.first, segment.second);
}

/*
QPair<int, QByteArray> DownloadManager::downloadSegment(int index, const QUrl &url) {

	QNetworkAccessManager manager;
	QNetworkRequest request(url);
	buildRequestHeaders(&request, m_downloadObject);
	QScopedPointer<QNetworkReply> reply(manager.get(request));

	QEventLoop loop;
	connect(reply.data(), &QNetworkReply::finished, this, [&]() {
		QMutexLocker locker(&m_mutex);
		++m_finishedSegments;
		
		//emit downloadProgress(m_finishedSegments);

		//issues: You should never access widgets and GUI related things directly 
		//from a thread other than the main thread, ui will be crashed
		//ui.progressBar->setValue(m_finishedSegments);
		qDebug() << "quit==== finish" << (DWORD)QThread::currentThreadId();
		loop.quit();
		});
	//connect(reply.data(), &QNetworkReply::downloadProgress, this, &DownloadManager::onSegmentDownloadProgress);
	connect(reply.data(), &QNetworkReply::downloadProgress, this, [&](qint64 bytesReceived, qint64 bytesTotal) {
		QMutexLocker locker(&m_mutex);
		if (m_downloadedBytes == 0) {
			setStatusLabel("Downloading...");
			setWindowTitle(QString("%1 %2").arg("Downloading..").arg(m_fileBaseName));
		}

		m_downloadedBytes = bytesReceived;

		int downloadSpeed = m_downloadedBytes * 1000 / m_elapsedTimer.elapsed();
		ui.speed->setText(humanReadableNum(downloadSpeed, true));
		ui.downloaded->setText(humanReadableNum(m_downloadedBytes));
		});

	connect(reply.data(), &QNetworkReply::readyRead, [&]() {
		//QMutexLocker locker(&m_mutex);
		//m_downloadedBytes += reply.data()->bytesAvailable();
		//locker.unlock();
		});

	QObject::connect(reply.data(), QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [&](QNetworkReply::NetworkError code) {
		//QMutexLocker locker(&m_mutex);
		//m_downloadedBytes += reply.data()->bytesAvailable();
		//locker.unlock();
		qDebug() << "Download ERR:" << reply.data()->errorString();
		});

	QObject::connect(&m_watcher, &QFutureWatcherBase::canceled, [&]() {
		//loop.quit(); // Cancel the computation
		//reply.data()->abort();
		qDebug() << "quit====" << (DWORD)QThread::currentThreadId() << reply.data();
		});
	
	loop.exec();
	qDebug() << "DownloadManager::downloadSegment" << index << (DWORD)QThread::currentThreadId();

	if (m_future.isCanceled()) {
		return qMakePair(index, QByteArray());
	}
	return qMakePair(index, reply.data()->readAll());
}*/

QPair<int, QByteArray> DownloadManager::downloadSegment(int index, const QUrl &url) {
	const int maxRetryCount = 1;  // Adjust as needed
	int retryCount = 0;

	while (retryCount <= maxRetryCount) {
		QNetworkAccessManager manager;
		QNetworkRequest request(url);
		buildRequestHeaders(&request, m_downloadObject);

		QScopedPointer<QNetworkReply> reply(manager.get(request));

		QEventLoop loop;
		QTimer timeoutTimer;
		timeoutTimer.setSingleShot(true);
		timeoutTimer.start(10000);  // 10 seconds timeout

		// Connect the timeout signal to handle the timeout
		connect(&timeoutTimer, &QTimer::timeout, [&]() {
			qDebug() << "Download timeout for segment" << index << ". Retrying...";
			reply->abort();
			loop.quit();
			});

		// Connect the finished signal to handle the completion
		connect(reply.data(), &QNetworkReply::finished, [&]() {
			QMutexLocker locker(&m_mutex);
			++m_finishedSegments;

			// Handle download progress
			// emit downloadProgress(m_finishedSegments);

			qDebug() << "Segment" << index << "download completed";

			// Clean up resources
			loop.quit();
			});

		// Connect the error signal to handle network errors
		connect(reply.data(), &QNetworkReply::errorOccurred, [&](QNetworkReply::NetworkError code) {
			qDebug() << "Download error for segment" << index << ":" << reply.data()->errorString();
			loop.quit();
			});

		// Connect the canceled signal to handle cancellation
		connect(&m_watcher, &QFutureWatcherBase::canceled, [&]() {
			qDebug() << "Download canceled for segment" << index;
			reply->abort();
			loop.quit();
			});

		// Connect the downloadProgress signal to handle progress updates
		connect(reply.data(), &QNetworkReply::downloadProgress, [&](qint64 bytesReceived, qint64 bytesTotal) {
			QMutexLocker locker(&m_mutex);
			if (m_downloadedBytes == 0) {
				setStatusLabel("Downloading...");
				setWindowTitle(QString("%1 %2").arg("Downloading..").arg(m_fileBaseName));
			}

			m_downloadedBytes = bytesReceived;

			int downloadSpeed = m_downloadedBytes * 1000 / m_elapsedTimer.elapsed();
			ui.speed->setText(humanReadableNum(downloadSpeed, true));
			ui.downloaded->setText(humanReadableNum(m_downloadedBytes));
			});

		// Start the event loop
		loop.exec();

		qDebug() << "DownloadManager::downloadSegment" << index;

		// Check if the download was successful
		if (m_future.isFinished() || reply->error() == QNetworkReply::NoError) {
			return qMakePair(index, reply.data()->readAll());
		}

		// Increment retry count
		++retryCount;
	}

	// If all retries failed, return an empty QByteArray
	qDebug() << "All retries failed for segment" << index;
	return qMakePair(index, QByteArray());
}


void DownloadManager::onSegmentDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
	QMutexLocker locker(&m_mutex);
	if (m_downloadedBytes == 0) {
		setStatusLabel("Downloading...");
		setWindowTitle(QString("%1 %2").arg("Downloading..").arg(m_fileBaseName));
	}

	m_downloadedBytes = bytesReceived;

	int downloadSpeed = m_downloadedBytes * 1000 / m_elapsedTimer.elapsed();
	ui.speed->setText(humanReadableNum(downloadSpeed, true));
	ui.downloaded->setText(humanReadableNum(m_downloadedBytes));
}

//reduce function format: V function(T &result, const U &intermediate)
// return void, first args is result, second is intermediate


void DownloadManager::downloadFiles(QStringList tsUrls, QString outputFilePath)
{

	// 创建QtConcurrent的线程池
	QThreadPool::globalInstance()->setMaxThreadCount(16);

	m_finishedSegments = 0;
	ui.progressBar->setRange(0, tsUrls.count());
	ui.progressBar->setValue(0);

	//QFuture<DownloadResult> future = QtConcurrent::mappedReduced(tsUrls, downloadChunk, reduceDownloads);

	std::function<QPair<int, QByteArray>(const QString &)> download = [=](const QString &url) {
		int index = tsUrls.indexOf(url);
		return downloadSegment(index, url);
	};

	m_future = QtConcurrent::mappedReduced(
		tsUrls,
		download,
		mergeFilesInOrder
	);

	m_watcher.setFuture(m_future);

	/*
	Problem: 
	1 if i define a local future, when watcher get signal finished, local future will be destoried
	before watcher???
	2 if i use local QString save unicodes, in the connect lambda funnction the unicode will be garbled??
	*/

	//watcher should be new instance otherwise watcher object is destroyed when leaving function
	connect(&m_watcher, &QFutureWatcher<QMap<int, QByteArray>>::finished, this, [&]() {
		if (m_future.isCanceled()) {
			// Clean up and return early
			qDebug() << "===quit m_future.isCanceled";
			if (m_closeRequested) {
				close();
			}
			
			return;
		}
		QMap<int, QByteArray> downloadedSegments = m_future.result();
		qDebug() << "Crash check 000000000000";
		QFile outputFile(m_outputFilePath);
		if (!outputFile.open(QIODevice::WriteOnly)) {
			qDebug() << "Failed to open file for writing:" << m_outputFilePath;
			return;
		}

		for (const QByteArray &data : downloadedSegments) {
			outputFile.write(data);
		}
		qDebug() << "Crash check 1111111111";
		outputFile.close();
		qDebug() << "Crash check 22222222222";

		setStatusLabel("Success", SUCCESS);

		ui.pushButton_File->setEnabled(true);
		ui.pushButton_Folder->setEnabled(true);

		//save download item into db
		//saveResource();
		});
	// 等待所有下载和合并任务完成

	//issues: You should never access widgets and GUI related things directly 
	//from a thread other than the main thread 
	//so connect(this, &DownloadManager::downloadProgress, >>>this should be set
	connect(this, &DownloadManager::downloadProgress, this, [=](int val) {
		
		//ui.progressBar->setValue(val);
		});

	connect(&m_watcher, &QFutureWatcher<QMap<int, QByteArray>>::progressValueChanged, this, [=](int val) {
		ui.filesize->setText(QString("%1/%2").arg(val).arg(tsUrls.count()));
		ui.progressBar->setValue(val);
		});

	QObject::connect(&m_watcher, &QFutureWatcherBase::canceled, [&]() {
		m_future.cancel(); // Cancel the computation
		qDebug() << "quit====111";
		//close();
		});
}

void DownloadManager::mergeFiles(QList<QByteArray> chunks, QString outputFilePath)
{
	QFile file(outputFilePath);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open file for writing:" << outputFilePath;
		return;
	}

	for (auto chunk : chunks)
	{
		file.write(chunk);
	}

	// 关闭目标文件
	file.close();
}

void DownloadManager::setStatusLabel(QString text, DownloadStatus type)
{
	QString color = "";

	switch (type)
	{
	case DownloadManager::SUCCESS:
		color = "QLabel { color : green; }";
		break;
	case DownloadManager::ERR:
		color = "QLabel { color : red; }";
		break;
	default:
		break;
	}
	ui.status->setStyleSheet(color);
	ui.status->setText(text);
}

void DownloadManager::saveResource()
{
	QString filepath = m_downloadObject["filepath"].toString();
	QString filename = ui.subject->text();
	
	
	QJsonObject fileObj;
	fileObj["filepath"] = filepath;
	if (!filename.isEmpty()) {
		//what if user modify filename
		fileObj["filepath"] = Utils::renameFile(filepath, filename + "." + m_downloadObject["fileType"].toString());
	}
	QFileInfo fi(fileObj["filepath"].toString());
	fileObj["name"] = fi.baseName();
	fileObj["subject"] = fi.baseName();
	fileObj["size"] = fi.size();
	fileObj["type"] = m_downloadObject["fileType"].toString();
	fileObj["lastModified"] = fi.lastModified().toString("yyyy-MM-dd hh:mm:ss");
	fileObj["file_created"] = fi.birthTime().toString("yyyy-MM-dd hh:mm:ss");
	fileObj["url"] = m_downloadObject["source"].toString();
	fileObj["pageurl"] = m_downloadObject["pageUrl"].toString();
	fileObj["pagetitle"] = m_downloadObject["pageTitle"].toString();
	fileObj["status"] = 2;
	fileObj["site"] = QUrl(m_downloadObject["pageUrl"].toString()).host();
	fileObj["create_time"] = fileObj["file_created"];
	QJsonDocument doc(m_downloadObject);
	fileObj["object"] = QString(doc.toJson(QJsonDocument::Compact));

	emit success(fileObj);

}