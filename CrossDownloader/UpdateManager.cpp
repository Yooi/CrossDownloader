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
				//QMessageBox::information(0, QStringLiteral("CrossDownloader HINT"), QStringLiteral("�����°汾:%1���´��������Զ�������").arg(updateVersion));
			}
		}
		});

}
/*
Test case:
�ڲ��Ե�ַ�޸�update.json verion ���ڵ�ǰ�汾���������������ļ���ַ�� /test.7z
Expected:
App start up��
1��updateflat����Ϊ�µĵ�ַ
2�������ļ���download folder
3����ѹ�ļ�����upgrade folder
App close:
1����ѹ�ļ�������app��װĿ¼
2��upgrade folder should be removed?
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
	// �ڱ��ر��Ӧ�ó�����Ҫ����
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
	// ���������в���
	QStringList arguments;
	//arguments << QString("-aoa x \"%1\"").arg(archivePath);  // ��ѹ����
	//arguments << QString("-o\"%1\"").arg(outputPath);  // ָ�����Ŀ¼
	//arguments << "> NUL";
	arguments << "x";  // ��ѹ����
	arguments << "-o" + outputPath;  // ָ�����Ŀ¼
	arguments << archivePath;  // ָ��ѹ���ļ�·��
	arguments << "-aoa"; //overwrite mode

	// ����7z.exe����
	QProcess process;
	QString cmd = QCoreApplication::applicationDirPath() + "/plugins/Zip/7z.exe ";
	qDebug() << "UpdateManager::extract" << cmd;
	process.startDetached(cmd, arguments);
	//process.waitForFinished(-1);  // �ȴ��������

	// ������̵ı�׼����ͱ�׼������Ϣ
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

	// ���� copy ����
	QString copyCommand = "xcopy /y /q \"" + updateFolderPath + "\\*.*\" \"" + appFolderPath + "\"";
	// rmdir command, remove upgrade folder
	QString rmdirCommand = "rmdir /s /q \"" + updateFolderPath + "\"";

	// ���������н���ִ�� copy ����
	if (QProcess::startDetached(copyCommand))
	{
		qDebug() << "Update files copied successfully." << copyCommand + " && " + rmdirCommand;

		// �Ƴ������ļ����ڵ�����
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
