#pragma once
#include <qobject.h>

class UpdateManager : public QObject
{
	Q_OBJECT
public:
	UpdateManager(QObject *parent = nullptr);
	~UpdateManager() {};
	void checkForUpdates();
	void installUpdate();

protected:

	bool updateRequired(QString version);

	bool versionCompare(QString version1, QString version2);

	void markForUpdate(QString version);

	void removeUpdateFlag();

	bool checkUpdateFlag();

	QString getFlagVersion();

	void extract(const QString & archivePath, const QString & outputPath);

	bool performUpdate();

private:
	

};

