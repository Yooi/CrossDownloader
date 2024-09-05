#include "stdafx.h"
#include "CApplication.h"
#include <QtNetwork/QNetworkProxy>

//Global
QString s_storage_path;
double CApplication::s_objectRate = 0;

CApplication::CApplication(int &argc, char **argv)
	: QApplication(argc, argv)
	, m_client(0)
	, m_updateManager(new UpdateManager)
{
	QCoreApplication::setOrganizationName(QLatin1String("CrossDownloader"));
	QCoreApplication::setApplicationName(QLatin1String("CrossDownloader"));
	QCoreApplication::setApplicationVersion(VERSION);

	initScript();
	initStoragePath();
	start();
	//
	m_updateManager->checkForUpdates();

	connect(qApp, &QCoreApplication::aboutToQuit, [=]() {
		qDebug() << "Quit Application!";
		m_updateManager->installUpdate();
		});

}

CApplication::~CApplication()
{
}

bool CApplication::localDebug()
{
	return CD_DEBUG;
}

void CApplication::initScript()
{
	m_ajaxhook_script = Utils::readFile(":/CrossDownloader/Resources/ajaxhook.js");
	m_empty_page = Utils::readFile(":/CrossDownloader/Resources/empty.html");
	m_fingerprint_script = Utils::readFile(":/CrossDownloader/Resources/fingerprint.js");
}

double CApplication::screenRate()
{
	if (!s_objectRate) {
		QList<QScreen*> screens = QApplication::screens();
		QScreen* screen = screens[0];

		HDC dc = GetDC(NULL);
		float dpi = GetDeviceCaps(dc, LOGPIXELSX);
		ReleaseDC(NULL, dc);

		qreal dpi1 = screen->logicalDotsPerInch();
		qreal dpiSystem = screen->physicalDotsPerInch();
		//qreal dpiSystem = screen->logicalDotsPerInch();

		if (dpi / 120.0 == 1) {
			s_objectRate = 1;
		}
		else {

			//计算dpi对应的缩放比例
			/*
			s_objectRate = 120.0 / dpiSystem;
			if (s_objectRate < 1) {
				s_objectRate -= 0.2;
			}
			*/
			s_objectRate = dpiSystem / 96.0 - 0.25;
			if (s_objectRate < 0.8 && dpi1 == dpi) {
				s_objectRate = 0.8;
			}
			//QRect scale = screen->geometry();

		}
		qDebug() << "[GApplication::screenRate]" << dpiSystem << dpi1 << dpi << s_objectRate << screen->availableGeometry();
	}
	return s_objectRate;
}

CrossDownloader * CApplication::newClient()
{
	if (!m_client) {
		try {
			m_client = new CrossDownloader();
		}
		catch (const std::runtime_error&) {
			QMessageBox::about(0, QStringLiteral("CrossDownloader"), QStringLiteral("数据创建或链接失败，请联系客服！"));
		}

		m_client->show();
		//m_client->upgradeInfo(m_upgradeObj);
		
		m_client->loadProxy();
		//QCoreApplication::processEvents();
		//QNetworkProxyFactory::setUseSystemConfiguration(CD_DEBUG);

	}
	return m_client;
}

QString CApplication::siteScript(const QString & site)
{	
	return Utils::readFile(QString(":/CrossDownloader/Resources/scripts/%1.js").arg(site));
}

void CApplication::initStoragePath()
{
	QDir dir;
	dir.mkpath(dataPath());
	dir.mkpath(upgradePath());
	dir.mkpath(extendPath());
	dir.mkpath(downloadPath());
	dir.mkpath(resourcePath());
	dir.mkpath(profilePath());
}

QString CApplication::storageDirPath()
{
	if (!s_storage_path.isEmpty()) {
		return s_storage_path;
	}

	s_storage_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
	qDebug() << "[GApplication::storageDirPath]" << s_storage_path;
	return s_storage_path;
}

QString CApplication::appDirPath()
{
	QString path = QCoreApplication::applicationDirPath();
	qDebug() << "[GApplication::appDirPath]" << path;
	return path;
}

QString CApplication::downloadPath()
{
	QSettings settings(settingFile(), QSettings::IniFormat);
	QString downloadPath = settings.value("downloadPath").toString();
	//if downloadpath was removed by user
	QDir dir(downloadPath);

	if (downloadPath.isEmpty() || !dir.exists()) {
		QString defaultpath = storageDirPath() + "/download";
		settings.setValue("downloadPath", defaultpath);
		return defaultpath;
	}
	return downloadPath;
}

void CApplication::start()
{
	updateStyleSheet();
	newClient();
}

CApplication *CApplication::instance()
{
	return (static_cast<CApplication *>(QCoreApplication::instance()));
}

QWebEngineProfile *CApplication::profile()
{
	QWebEngineProfile* profile = new QWebEngineProfile();
	profile->setHttpUserAgent(AGENT);
	profile->setHttpAcceptLanguage("en,zh-CN;q=0.9,zh;q=0.8");
	QString profilePath = CApplication::profilePath();
	profile->setCachePath(profilePath);
	profile->setPersistentStoragePath(profilePath);

	//off-the-record profile
	return profile;
}

QWebEngineProfile * CApplication::profile(QString profileName)
{
	qDebug() << "[GApplication::profile profileName]" << profileName;
	QWebEngineProfile* profile = new QWebEngineProfile(profileName);
	profile->setHttpUserAgent(AGENT);
	profile->setHttpAcceptLanguage("en,zh-CN;q=0.9,zh;q=0.8");
	QString profilePath = CApplication::profilePath() + "/" + profileName;
	profile->setCachePath(profilePath);
	profile->setPersistentStoragePath(profilePath);

	return profile;
}

void CApplication::updateStyleSheet()
{
	QFile file(":/CrossDownloader/Resources/main.css");
	file.open(QFile::ReadOnly);
	QString mainStyle = file.readAll();
	setStyleSheet(mainStyle);
	file.close();
}
