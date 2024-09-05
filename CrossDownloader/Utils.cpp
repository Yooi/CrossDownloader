#include "stdafx.h"
#include "Utils.h"
#include "zlib.h"

QMap<QString, QIcon> g_typeMapping;
QString Utils::imageToBase64(QString path)
{
	QByteArray image;
	QString suffix = path.split(".")[1];
	if (!path.isEmpty()) {
		QFile file(path);
		file.open(QFile::ReadOnly);
		image = file.readAll();
		image = image.toBase64();
		file.close();
	}

	return "data:image/"+ suffix +";base64," + image;
}

bool Utils::removeFolderContent(const QString &folderDir)
{
	QDir dir(folderDir);
	QFileInfoList fileList;
	QFileInfo curFile;
	if (!dir.exists()) { return false; }//文件不存，则返回false
	fileList = dir.entryInfoList(QDir::Dirs | QDir::Files
		| QDir::Readable | QDir::Writable
		| QDir::Hidden | QDir::NoDotAndDotDot
		, QDir::Name);
	while (fileList.size() > 0)
	{
		int infoNum = fileList.size();
		for (int i = infoNum - 1; i >= 0; i--)
		{
			curFile = fileList[i];
			if (curFile.isFile())//如果是文件，删除文件
			{
				QFile fileTemp(curFile.filePath());
				fileTemp.remove();
				fileList.removeAt(i);
			}
			if (curFile.isDir())//如果是文件夹
			{
				QDir dirTemp(curFile.filePath());
				QFileInfoList fileList1 = dirTemp.entryInfoList(QDir::Dirs | QDir::Files
					| QDir::Readable | QDir::Writable
					| QDir::Hidden | QDir::NoDotAndDotDot
					, QDir::Name);
				if (fileList1.size() == 0)//下层没有文件或文件夹
				{
					dirTemp.rmdir(".");
					fileList.removeAt(i);
				}
				else//下层有文件夹或文件
				{
					for (int j = 0; j < fileList1.size(); j++)
					{
						if (!(fileList.contains(fileList1[j])))
							fileList.append(fileList1[j]);
					}
				}
			}
		}
	}
	return dir.rmdir(folderDir);
}


QString Utils::makeFolder(QString name)
{
	QDir dir;
	dir.mkpath(name);

	return name;
}

// covert file native icon to base64, in order to show it in the webpage
QString Utils::getNativeIcon(QFileInfo fileInfo)
{
	QFileIconProvider iconProvider;
	QIcon fileIcon = iconProvider.icon(fileInfo);

	QPixmap pixmap = fileIcon.pixmap(QSize(32, 32));
	QByteArray bytes;
	QBuffer buffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	QString base64Data = QString::fromLatin1(bytes.toBase64().data());
	return "data:image/png;base64," + base64Data;
}

QByteArray Utils::decompressGzip(const QByteArray& compressedData)
{
	const int CHUNK_SIZE = 1024;
	QByteArray decompressedData;
	char buffer[CHUNK_SIZE];

	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;

	int ret = inflateInit2(&stream, 16 + MAX_WBITS);; // Initialize the zlib stream for GZIP decoding

	if (ret != Z_OK) {
		qWarning() << "Failed to initialize zlib stream for GZIP decoding";
		return decompressedData;
	}

	stream.avail_in = compressedData.size();
	stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(compressedData.constData()));

	do {
		stream.avail_out = CHUNK_SIZE;
		stream.next_out = reinterpret_cast<Bytef*>(buffer);

		ret = inflate(&stream, Z_NO_FLUSH);
		if (ret < 0) {
			inflateEnd(&stream);
			return decompressedData;
		}

		int bytesRead = CHUNK_SIZE - stream.avail_out;
		decompressedData.append(buffer, bytesRead);
	} while (stream.avail_out == 0);

	inflateEnd(&stream); // Clean up the zlib stream

	return decompressedData;
}

QString Utils::makeValidWindowsFilename(const QString& filename, QChar replacementChar)
{
	QRegularExpression invalidCharPattern("[<>:\"/\\\\|?*\\x00-\\x1F\\s]");
	QString modifiedFilename = filename;
	modifiedFilename.replace(invalidCharPattern, replacementChar);
	return modifiedFilename;
}

QString Utils::getFileHash(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open file: " << filePath;
		return QString();
	}

	QCryptographicHash hash(QCryptographicHash::Md5);
	if (hash.addData(&file)) {
		QByteArray fileHash = hash.result();
		return fileHash.toHex();
	}
	else {
		qDebug() << "Failed to calculate hash for file: " << filePath;
		return QString();
	}
}


void Utils::combineVideoAndMp3(const QString& videoFilePath, const QString& mp3FilePath, const QString& outputFilePath)
{
	
}

QString Utils::readFile(QString path)
{
	QString data;
	QFile file;
	file.setFileName(path);
	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		data = file.readAll();
		file.close();
	}

	return data;
}

QIcon Utils::getLocalFileIcon(QString suffix)
{
	if (g_typeMapping.contains(suffix)) {
		return g_typeMapping.value(suffix);
	}
	QTemporaryFile tempFile;
	tempFile.setFileTemplate("tempfile.XXXXXX." + suffix);
	tempFile.open();



	// Get the file icon provider                                               
	QFileIconProvider iconProvider;

	// Get the file icon based on the MIME type
	QIcon fileIcon = iconProvider.icon(tempFile);

	// Clean up the temporary file
	tempFile.close();
	tempFile.remove();

	g_typeMapping.insert(suffix, fileIcon);
	return fileIcon;
}

QStringList Utils::extractHashtags(const QString &input)
{
	QRegularExpression hashtagRegex("#[\\w\\p{L}]+");
	QRegularExpressionMatchIterator matchIterator = hashtagRegex.globalMatch(input);

	QStringList hashtags;
	while (matchIterator.hasNext()) {
		QRegularExpressionMatch match = matchIterator.next();
		QString tag = match.captured().mid(1);
		if (!hashtags.contains(tag)) {
			hashtags << tag;
		}
	}

	return hashtags;
}

QString Utils::getUniqueFileName(const QString& filePath)
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
	return uniqueFilePath;
}

QString Utils::renameFile(QString filepath, QString newname)
{
	QFile file(filepath);
	QFileInfo fi(filepath);
	//new file should include whole path of the file
	QString newpath = getUniqueFileName(fi.absolutePath() + "/" + newname);
	if (file.rename(newpath)) {
		return newpath;
	}
	return filepath;
}

QString Utils::humanReadableSize(qint64 size) {
	const QStringList units = { "B", "KB", "MB", "GB", "TB" };

	double sizeInBytes = static_cast<double>(size);

	int unitIndex = 0;
	while (sizeInBytes >= 1024.0 && unitIndex < units.size() - 1) {
		sizeInBytes /= 1024.0;
		unitIndex++;
	}

	QString result = QString("%1 %2").arg(sizeInBytes, 0, 'f', 2).arg(units[unitIndex]);
	return result;
}

void Utils::exportToCsv(const QList<QStringList> &data, const QString &filePath) {
	// 创建文件对话框
	QString defaultFileName = filePath.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) : filePath;
	QString selectedFileName = QFileDialog::getSaveFileName(nullptr, "Save CSV File", defaultFileName, "CSV Files (*.csv)");

	if (!selectedFileName.isEmpty()) {
		// 打开文件并写入数据
		QFile file(selectedFileName);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			// Create a QTextStream to write to the file
			QTextStream out(&file);
			out.setCodec("UTF-8");

			// 写入数据到文件
			for (const QStringList &row : data) {
				// 处理每个字段的 Unicode 字符
				
				out << row.join(",") << "\n";
			}

			// Close the file
			file.close();
		}
	}
}

bool Utils::siteVerfiy(const QString& text)
{
	QRegExp httpOrHttpsRegex("(https?://)");
	httpOrHttpsRegex.setCaseSensitivity(Qt::CaseInsensitive);

	if (text.contains(httpOrHttpsRegex))
	{
		//input websites:  xiaohongshu / douyin
		QStringList urlList = SUPPORT_SITES;
		QRegExp urlRegex(urlList.join("|"));
		urlRegex.setCaseSensitivity(Qt::CaseInsensitive);

		return text.contains(urlRegex);
	}
	return false;
}

QStringList Utils::extractUrls(const QString &inputString) {
	QList<QString> urls;

	// Regular expression to match URLs
	QRegularExpression urlRegex(R"(https?://\S+)");

	// Iterator over matches in the input string
	auto matchIterator = urlRegex.globalMatch(inputString);

	// Extract URLs from matches
	while (matchIterator.hasNext()) {
		auto match = matchIterator.next();
		urls.append(match.captured());
	}

	return urls;
}

QJsonObject Utils::mergeJsonObjects(const QJsonObject &obj1, const QJsonObject &obj2) {
	QJsonObject result = obj1;

	// Iterate over the keys of obj2
	for (auto it = obj2.constBegin(); it != obj2.constEnd(); ++it) {
		const QString &key = it.key();
		const QJsonValue &value = it.value();

		// Insert the key-value pair into the result object
		if (!result.contains(key)) {
			result[key] = value; // If the key doesn't exist in result, insert it
		}
		else {
			// Key already exists, decide whether to overwrite or ignore it
			// For this example, we'll overwrite the existing value
			result[key] = value;
		}
	}

	return result;
}
