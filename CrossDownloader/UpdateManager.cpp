#include "stdafx.h"
#include "UpdateManager.h"
#include "networkaccessmanager.h"
#include <QtConcurrent>

UpdateManager::UpdateManager(QObject *parent)
{
	
}

void UpdateManager::checkForUpdates()
{
	QtConcurrent::run([=]() {
		NetworkAccessManager manager;
		QByteArray ret = manager.getUrl(QUrl(CHECKUPDATE_URL));
		//if already downloaded, prevent download again
		//only if update version is gt flag version, it will download update pack
		if (!ret.isEmpty()) {
			QJsonObject json = QJsonDocument::fromJson(ret).object();
			QString updateVersion = json["version"].toString();
			if (!versionCompare(updateVersion, getFlagVersion())) {
				return;
			}
			
			qDebug() << "UpdateManager::checkForUpdates" << json;
			if (updateRequired(updateVersion) && !json["upgrade"].isUndefined()) {
				//update
				QString filepath = manager.download(json["upgrade"].toString(), qobject_cast<CApplication*>(qApp)->downloadPath());
				extract(filepath, qobject_cast<CApplication*>(qApp)->upgradePath());
				markForUpdate(updateVersion);
				//QMessageBox::information(0, QStringLiteral("CrossDownloader HINT"), QStringLiteral("发现新版本:%1，下次启动会自动升级！").arg(updateVersion));
			}
		}
		});

}
/*
Test case:
在测试地址修改update.json verion 大于当前版本，并且设置下载文件地址如 /test.7z
Expected:
App start up：
1、updateflat更新为新的地址
2、下载文件在download folder
3、解压文件进入upgrade folder
App close:
1、解压文件拷贝到app安装目录
2、upgrade folder should be removed?
*/

void UpdateManager::installUpdate()
{
	if (checkUpdateFlag()) {
		performUpdate();
	}
}

bool UpdateManager::updateRequired(QString version)
{
	qDebug() << "UpdateManager::updateRequired" << version << VERSION;
	return versionCompare(version, VERSION);
}

bool UpdateManager::versionCompare(QString version1, QString version2)
{
	QVersionNumber checkVersion = QVersionNumber::fromString(version1);
	QVersionNumber currVersion = QVersionNumber::fromString(version2);
	if (version1 > version2) {
		return true;
	}
	return false;

}

void UpdateManager::markForUpdate(QString version)
{
	// 在本地标记应用程序需要更新
	QFile updateFlag(qobject_cast<CApplication*>(qApp)->upgradeFlag());
	if (updateFlag.open(QIODevice::WriteOnly))
	{
		updateFlag.write(version.toUtf8());
		updateFlag.close();
	}
}

void UpdateManager::removeUpdateFlag()
{
	QFile updateFlag(qobject_cast<CApplication*>(qApp)->upgradeFlag());
	updateFlag.remove();
}

bool UpdateManager::checkUpdateFlag()
{
	QString flagVersion = getFlagVersion();
	if (updateRequired(flagVersion)) {
		return true;
	}
	return false;
}

QString UpdateManager::getFlagVersion()
{
	QFile updateFlag(qobject_cast<CApplication*>(qApp)->upgradeFlag());
	if (updateFlag.exists() && updateFlag.open(QIODevice::ReadOnly)) {
		QString version = updateFlag.readAll();
		updateFlag.close();
		return version;
	}
	return QString();
}

void UpdateManager::extract(const QString &archivePath, const QString &outputPath)
{
	// 构建命令行参数
	QStringList arguments;
	//arguments << QString("-aoa x \"%1\"").arg(archivePath);  // 解压命令
	//arguments << QString("-o\"%1\"").arg(outputPath);  // 指定输出目录
	//arguments << "> NUL";
	arguments << "x";  // 解压命令
	arguments << "-o" + outputPath;  // 指定输出目录
	arguments << archivePath;  // 指定压缩文件路径
	arguments << "-aoa"; //overwrite mode

	// 启动7z.exe进程
	QProcess process;
	QString cmd = QCoreApplication::applicationDirPath() + "/plugins/Zip/7z.exe ";
	qDebug() << "UpdateManager::extract" << cmd;
	process.startDetached(cmd, arguments);
	//process.waitForFinished(-1);  // 等待进程完成

	// 输出进程的标准输出和标准错误信息
	qDebug() << "Standard Output: " << process.readAllStandardOutput();
	qDebug() << "Standard Error: " << process.readAllStandardError();
	//process.close();
}

bool UpdateManager::performUpdate()
{
	// Update folder path (replace this with your actual update folder path)
	QString updateFolderPath = QDir::toNativeSeparators(qobject_cast<CApplication*>(qApp)->upgradePath());

	// App folder path (replace this with your actual app folder path)
	QString appFolderPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());

	// 构造 copy 命令
	QString copyCommand = "xcopy /y /q \"" + updateFolderPath + "\\*.*\" \"" + appFolderPath + "\"";
	// rmdir command, remove upgrade folder
	QString rmdirCommand = "rmdir /s /q \"" + updateFolderPath + "\"";

	// 启动命令行进程执行 copy 命令
	if (QProcess::startDetached(copyCommand))
	{
		qDebug() << "Update files copied successfully." << copyCommand + " && " + rmdirCommand;

		// 移除升级文件夹内的内容
		//QDir updateDir(updateFolderPath);
		//updateDir.removeRecursively();

		//removeUpdateFlag();
		return true;
	}
	else
	{
		qDebug() << "Failed to copy update files.";
		return false;
	}

}
