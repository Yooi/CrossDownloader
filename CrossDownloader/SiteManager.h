#pragma once

class SiteManager : public QObject
{
	Q_OBJECT
public:
	SiteManager();
	~SiteManager() {};
	QMap<QString, QStringList> actions(const QString site);

};

