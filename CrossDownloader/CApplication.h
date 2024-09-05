#ifndef GAPPLICATION_H
#define GAPPLICATION_H

#pragma once
#include "crossdownloader.h"
#include <QWebEngineProfile>
#include "UpdateManager.h"

class CApplication : public QApplication
{
	Q_OBJECT
public:
	CApplication(int &argc, char **argv);
	~CApplication();

	static bool localDebug();

	static double screenRate();

	CrossDownloader *newClient();

	static QString storageDirPath();
	static QString appDirPath();
	static QString dataPath() { return storageDirPath() + "/data"; }
	static QString resourcePath() { return dataPath() + "/resource"; }
	static QString upgradePath() { return storageDirPath() + "/upgrade"; }
	static QString extendPath() { return storageDirPath() + "/extend"; }
	static QString profilePath() { return dataPath() + "/profile"; }
	static QString downloadPath();
	static QString upgradeFlag() { return dataPath() + "/update.flag"; }
	static QString settingFile() { return dataPath() + "/config"; }

	QWebEngineProfile * profile(void);
	QWebEngineProfile * profile(QString profileName);

	void updateStyleSheet();

	QString ajaxHookScript() { return m_ajaxhook_script; };
	QString fingerPrint() { return m_fingerprint_script; };
	QString emptyPage() { return m_empty_page; }

	static QString siteScript(const QString& site);

protected:

	void initStoragePath();
	void start();

	CApplication * instance();

	void initScript();

private:
	CrossDownloader* m_client;
	UpdateManager* m_updateManager;

	QString m_ajaxhook_script;
	QString m_sync_script;
	QString m_empty_page;
	QString m_fingerprint_script;

	static double s_objectRate;
};

#endif // GAPPLICATION_H