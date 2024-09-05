#pragma once
class Utils
{
public:
	Utils() {};
	~Utils() {};

	static QString imageToBase64(QString path);
	static bool removeFolderContent(const QString & folderDir);
	static QString makeFolder(QString name);
	static QString getNativeIcon(QFileInfo fileinfo);
	static QByteArray decompressGzip(const QByteArray& compressedData);
	static QString makeValidWindowsFilename(const QString & filename, QChar replacementChar = '_');
	static QString getFileHash(const QString & filePath);
	static void combineVideoAndMp3(const QString& videoFilePath, const QString& mp3FilePath, const QString& outputFilePath);
	static QString readFile(QString path);
	static QIcon getLocalFileIcon(QString suffix);
	static QStringList extractHashtags(const QString & input);
	static QString getUniqueFileName(const QString & filePath);
	static QString renameFile(QString filepath, QString newname);
	static QString humanReadableSize(qint64 size);
	static void exportToCsv(const QList<QStringList>& data, const QString & filePath = 0);
	static bool siteVerfiy(const QString & text);
	static QStringList extractUrls(const QString & inputString);
	QJsonObject mergeJsonObjects(const QJsonObject & obj1, const QJsonObject & obj2);
};

