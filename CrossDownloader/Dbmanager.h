#pragma once
#include <QSqlDatabase>
#include <QSqlTableModel>

struct TableInfo
{
	QString table = 0;
	int rowCount = 0;
	int curser = 0;
	int pageSize = 0;
};

class DBConnection {
public:
	DBConnection(QString path)
		: m_name(QString::number(QRandomGenerator::global()->generate(), 36))
	{
		if (QSqlDatabase::contains(m_name)) {
			return;
		}

		auto database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_name);
		database.setDatabaseName(path);
		//database.open();
	}

	~DBConnection()
	{
		QSqlDatabase::removeDatabase(m_name);
	}

	QSqlDatabase database()
	{
		return QSqlDatabase::database(m_name);
	}

private:
	QString m_name;
};

class SqlTableModel : public QSqlTableModel
{
public:
	SqlTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
	~SqlTableModel();

	void setSqlQuery(const QSqlQuery &);
	void setSqlQuery(const QString & query);
};

class DBManager : public QObject
{
	Q_OBJECT
public:
	DBManager(const QString path);
	~DBManager();

	QSqlDatabase currentDatabase();

	bool isValid();

	QJsonObject addAccount(QString uid, QJsonObject userObject, QString cookies);

	bool setSyncAccount(QString uid, bool state);

	bool updateOpenTime(QString uid);

	void setAccountLogin(QString uid, QVariant sts);

	bool updateAccountField(QString uid, QString field, QVariant val);

	bool updateAccount(QString uid, QJsonObject obj);

	int addSchedule(QString ownerid, QJsonObject obj);

	int addComment(QJsonObject obj);

	bool removeComments(QString videoId);

	int addResource(QJsonObject obj);

	QJsonObject getResource(QString id);

	bool updateResource(qint64 id, QJsonObject obj);

	//clone tweetid and update with obj
	int cloneResource(QString targetid, QString ownerid, QJsonObject obj);

	bool removeResource(QString id);

	bool cancelSchedule(QString id, QString ownerid); //remove scheduled timestamp

	QJsonObject updateSchedule(QString id, QString ownerid, QJsonObject obj);

	bool removeSchedulebyTimestamp(QString t);

	bool addMessage(QString uid, QString cate, QVariant content);

	bool removeMessage(QString id);

	QJsonObject querySchedules(QString ownerid, QString filter=0, int page = 0);

	QJsonArray querySites(QString filter, int page);

	QJsonArray queryResources(QString ownerid, QString filter=0, int page = 0);

	QJsonArray queryMessages(QString ownerid, QString filter, int page);

	QJsonObject queryValidSchedule(bool withTweet = true);

	QStringList queryScheduledAccounts();

	QJsonObject queryScheduledTweets(QString ownerid);

	//hours = 0, all tweets, limit = 0, unlimit
	QJsonObject queryAllScheduledTweets(int hours = 0, bool ascending=true, int limit=0);


	void setErrorCode(QString id, QString code);

	int getRowCounts(const QString & tableName);

	bool removeAccount(QString uid);

	QJsonArray queryAccounts(QString uid = 0, QString filter=0, int page=0);
	QList<QJsonObject> queryAccountsList();

	QJsonObject queryAccount(QString uid);

	bool hasScheduledItems();

	void add_or_updateSite(QString nick, QJsonObject obj);

	QJsonObject querySite(QString sitename);

	QJsonArray queryGroups(QString ownerid, QString filter, int page);

	void addGroup(QString ownerid, QJsonObject obj);

	void removeGroup(QString id);

	int tableCount(QString table, QString uid = 0);

	QJsonArray queryTable(QString tablename, QString ownerid, QString filter, int page);

	QJsonArray queryTasks(QString ownerid, QString filter = 0, int page = 0);

	bool removeTask(QString id);

	int addTask(QString ownerid, QJsonObject obj);

	bool updateTask(QString id, QJsonObject obj);
	bool setTaskStatus(QString id, int status);

	void executeSqlFile(QString file);

	bool updateTableItem(QString table, QString id, QJsonObject obj);

protected:
	void setupTables();

	bool prepareSqlData(QString sql);

	void alterColumns();

	QStringList parseSQLFile(const QString & sqlScript);

	bool execSql(const QString & sql);

	bool queryExec(const QString & sql);

	QList<QJsonObject> query(const QString & table, const QString & filter, int page = 0, int page_size = 30);

	void updateTableCount(QString table);

	int tableRowCount(QString table, QString uid = 0);

	bool update(const QString & table, const QMap<QString, QVariant>& values, const QString & filter);

	int add(const QString & table, const QMap<QString, QVariant>& values);

	bool remove(const QString & table, const QString & filter);

	QMap<QString, QVariant> setDefaultValues(QMap<QString, QVariant> data);

	QJsonObject checkAndUpdateResource(QJsonObject item);


private:
	QSqlDatabase m_db;
	//QSqlTableModel* m_pModel;
	QString m_db_path;
	QHash<QString, TableInfo> m_tableInfo;
	//QThreadPool* m_pool;
};

