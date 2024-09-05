#include "stdafx.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QtConcurrent>
#include "sql.h"
#include "Dbmanager.h"
#include "CApplication.h"

static QThreadStorage<DBConnection *> g_databaseConnections;

DBManager::DBManager(QString path)
{	
	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName(path);
	m_db_path = path;
	if (!m_db.open())
	{
		qDebug() << "Error: connection with database failed";
		throw std::runtime_error("connection with database failed");
	}
	else
	{
		
		setupTables();
		//m_pModel = new QSqlTableModel(NULL, m_db);
		qDebug() << "Database: connection ok";

	}
}


DBManager::~DBManager()
{

	m_db.close();
	QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

QSqlDatabase DBManager::currentDatabase()
{
	if (!g_databaseConnections.hasLocalData()) {
		g_databaseConnections.setLocalData(new DBConnection(m_db_path));
	}
	return g_databaseConnections.localData()->database();
}

bool DBManager::isValid()
{
	return currentDatabase().isValid();
}

void DBManager::setupTables()
{
	QSqlQuery query;
	
	//active - current account (only one active now), enable - if account init agentView, online - if account is enable on, check online status
	prepareSqlData(accounts_sql_);

	//args is JSON objects for extra params
	//e.g. if type is Video, args is video obj
	//ownerid is which weibo user schedule it
	//FIX collector, time NOT NULL -> NULL
	//status, 0 normal, 1 need to be processed(video upload / image MD5)
	//oid ----> target super topic ID
	//sync ---->  if sync server or not
	
	prepareSqlData(resource_sql_);

	prepareSqlData(user_sql_);

	if (prepareSqlData(site_sql_)) {
		//prepareData
		//executeSqlFile(":/svm/Resources/site.sql");
	}

	prepareSqlData(comment_sql_);
	
	alterColumns();
}

bool DBManager::prepareSqlData(QString sql)
{
	QSqlQuery query;
	query.prepare(sql);
	if (!query.exec()) {
		qDebug() << query.lastError();
		query.finish();
		return false;
	}

	query.finish();
	return true;
}

void DBManager::alterColumns()
{
	QSqlQuery query;
	query.prepare("ALTER TABLE resource ADD COLUMN content TEXT");
	if (!query.exec()) {
		qDebug() << query.lastError();
	}
}

QStringList DBManager::parseSQLFile(const QString &sqlScript) {
	QStringList statements;
	QString statement;
	bool inString = false;
	QChar stringChar;

	for (int i = 0; i < sqlScript.size(); ++i) {
		QChar c = sqlScript.at(i);

		if (c == '\'' || c == '\"') {
			if (inString && c == stringChar) {
				inString = false;
			}
			else if (!inString) {
				inString = true;
				stringChar = c;
			}
		}

		if (c == ';' && !inString) {
			statements << statement;
			statement.clear();
		}
		else {
			statement += c;
		}
	}

	if (!statement.isEmpty()) {
		statements << statement;
	}

	return statements;
}

void DBManager::executeSqlFile(QString filename) {
	QString sqlString = Utils::readFile(filename);
	//Replace comments and tabs and new lines with space
	//sqlString = sqlString.replace(QRegularExpression("(\\/\\*(.|\\n)*?\\*\\/|^--.*\\n|\\t|\\n)", QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption), " ");
	//Remove waste spaces
	sqlString = sqlString.trimmed();

	QStringList sqlList = parseSQLFile(sqlString);

	for each (QString sql in sqlList)
	{
		QString trimmedStatement = sql.trimmed();
		//qDebug() << ">>>" << trimmedStatement << m_db.isOpen();
		execSql(trimmedStatement);
	}
}

bool DBManager::updateTableItem(QString table, QString id, QJsonObject obj)
{
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}

	return update(table, data, QString("id='%1'").arg(id));
}

bool DBManager::execSql(const QString& sql)
{
	if (!m_db.isOpen())
		return false;

	return queryExec(sql);
}

bool DBManager::queryExec(const QString& sql)
{
	qDebug() << "DBManager::queryExec" << sql;
	QSqlQuery query(m_db);
	//query.prepare(sql);
	bool ret = query.exec(sql);

	if (!ret) {
		qDebug() << "DBManager::queryExec" << query.lastError();
	}
	else {

	}
	query.clear();
	query.finish();
	
	return ret;
}

/*
QList<QVariantList> DBManager::query(const QString& table, QStringList& columns, const QString& where)
{
	QList<QList<QVariant>> rets;
	if (!m_db.isOpen())
		return rets;

	QSqlQuery query(m_db);
	QString _columns;
	if (columns.count()>0)
		_columns = columns.join(",");
	else
		_columns = "*";

	QString sql;
	if (where.isEmpty())
		sql = QString("SELECT %1 FROM %2").arg(_columns).arg(table);
	else
		sql = QString("SELECT %1 FROM %2 WHERE %3").arg(_columns).arg(table).arg(where);
	if (queryExec(query, sql))
	{
		int colNum = query.record().count();

		for (int i = 0; i<colNum; ++i)
		{
			columns << query.record().fieldName(i);
		}

		while (query.next())
		{
			QList<QVariant> row;
			for (int i = 0; i<colNum; ++i)
			{
				row.append(query.value(i));
			}

			rets.append(row);
		}
	}
	return rets;
}


bool DBManager::update(const QString & table, const QMap<QString, QVariant>& values, const QString & _where)
{
	if (!m_db.isOpen())
		return false;

	QSqlQuery query(m_db);
	QString datas;
	QList<QString> keyList = values.keys();
	foreach(QString key, keyList) {
		if (!datas.isEmpty())
			datas += ",";
		datas += QString("%1=?").arg(key);
	}

	QString sql;
	if (_where.isEmpty())
		sql = QString("UPDATE %1 SET %2").arg(table).arg(datas);
	else
		sql = QString("UPDATE %1 SET %2 WHERE %3").arg(table).arg(datas).arg(_where);
	if (!query.prepare(sql)) {
		qDebug() << "DBManager::update" << query.lastError();
		return false;
	}

	for (int i = 0; i<keyList.count(); ++i)
	{
		query.bindValue(i, values.value(keyList.at(i)));
	}

	return queryExec(query, sql);
}

bool DBManager::remove(const QString& table, const QString& _where)
{
	if (!m_db.isOpen())
		return false;

	QSqlQuery query;
	QString sql = QString("DELETE FROM %1 WHERE %2").arg(table).arg(_where);
	return queryExec(query, sql);
}

bool DBManager::add(const QString& table, const QMap<QString, QVariant>& data)
{
	if (!m_db.isOpen())
		return false;

	QMap<QString, QVariant> values = setDefaultValues(data);
	QSqlQuery query(m_db);
	QString columns, datas;
	QList<QString> keyList = values.keys();
	foreach(QString key, keyList) {
		if (!columns.isEmpty())
			columns += ",";
		columns += key;

		if (!datas.isEmpty())
			datas += ",";
		datas += "?";
	}
	
	QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(table).arg(columns).arg(datas);
	query.prepare(sql);

	for (int i = 0; i<keyList.count(); ++i)
	{
		query.bindValue(i, values.value(keyList.at(i)));
	}

	return queryExec(query, sql);
}
*/

void DBManager::updateTableCount(QString table)
{
	TableInfo stTableinfo = m_tableInfo.value(table);
	stTableinfo.rowCount = tableRowCount(table);
	m_tableInfo[table] = stTableinfo;
}

int DBManager::tableRowCount(QString table, QString uid)
{
	QString innerfilter = "1=1 ";
	if (!uid.isEmpty()) {
		innerfilter += "ownerid=" + uid;
	}
	QSqlQuery query(QString("SELECT count(*) as total FROM %1 where %2").arg(table).arg(innerfilter));
	query.exec();
	query.next();
	QSqlRecord record = query.record();
	return record.value(0).toInt();
}

bool DBManager::update(const QString& table, const QMap<QString, QVariant>& data, const QString& filter)
{
	QFuture<bool> future = QtConcurrent::run([=]() {
		SqlTableModel model(NULL, currentDatabase());

		model.setTable(table);
		model.setFilter(filter);
		model.select();

		for (int i = 0; i < model.rowCount(); i++) {
			QSqlRecord record = model.record(i);
			QMapIterator<QString, QVariant> j(data);
			while (j.hasNext()) {
				j.next();
				record.setValue(j.key(), j.value());
			}
			model.setRecord(i, record);
		}
		return true;
	});
	return future.result();
}

int DBManager::add(const QString& table, const QMap<QString, QVariant>& data)
{
	QFuture<int> future = QtConcurrent::run([=]() {
		SqlTableModel model(NULL, currentDatabase());

		model.setTable(table);
		model.select();

		//m_pModel->setTable(table);
		//m_pModel->select();

		//qDebug() << "DBManager::add" << model.rowCount();
		
		QSqlRecord record = model.record();
		QMap<QString, QVariant> values = setDefaultValues(data);

		QMapIterator<QString, QVariant> j(values);
		while (j.hasNext()) {
			j.next();
			record.setValue(j.key(), j.value());
		}

		bool ret = model.insertRecord(-1, record);
		//updateTableCount(table);
		if (ret) {
			return model.query().lastInsertId().toInt();
		}
		return -1;
	});
	return future.result();
}

bool DBManager::remove(const QString& table, const QString& filter)
{
	tableRowCount(table);
	QFuture<bool> future = QtConcurrent::run([=]() {
		SqlTableModel model(NULL, currentDatabase());

		model.setTable(table);
		model.setFilter(filter);
		model.select();

		for (int i = 0; i < model.rowCount(); i++) {
			model.removeRow(i);
		}
		return true;
	});

	updateTableCount(table);
	return future.result();
}

//page 1....
QList<QJsonObject> DBManager::query(const QString& table, const QString& filter, int page, int page_size)
{
	QFuture<QList<QJsonObject>> future = QtConcurrent::run([=]() {
		QString innerFilter = filter;
		if (!innerFilter.isEmpty() && !innerFilter.contains("LIMIT") && page) {
			innerFilter += QString(" LIMIT %1,%2").arg((page - 1)*page_size).arg(page_size);
		}
		QList<QJsonObject> rets;

		qDebug() << "0000000000";
		if (!isValid()) {
			return rets;
		}
		qDebug() << "22222222";
		SqlTableModel model(NULL, currentDatabase());
		qDebug() << "11111111111";
		model.setTable(table);
		model.setFilter(innerFilter);
		model.select();

		
		for (int i = 0; i < model.rowCount(); i++) {
			QSqlRecord record = model.record(i);
			QJsonObject row;
			for (int i = 0; i < record.count(); i++) {
				row[record.fieldName(i)] = record.value(i).toJsonValue();
			}
			rets.append(row);
		}
		qDebug() << "[DBManager::query count]" << rets.count();
		return rets;
	});
	return future.result();
}

QMap<QString, QVariant> DBManager::setDefaultValues(QMap<QString, QVariant> data)
{
	data.insert("create_dt", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	return data;
}

QJsonObject DBManager::addAccount(QString uid, QJsonObject userObject, QString cookies)
{	
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = userObject.constBegin(); i != userObject.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}

	qDebug() << "[DBManager::addAccount uid]" << uid;
	QJsonObject account = queryAccount(uid);
	
	if (account.isEmpty()) {
		//default values
		data.insert("current", false);
		data.insert("priority", 0);
		data.insert("enable", true);	//account 
		data.insert("sync", false);		//account sync
		data.insert("status", 0);
		data.insert("active", true);
		data.insert("status", 0);
		data.insert("model", 0);		//auto or not
		data.insert("create_dt", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	}
	data.insert("uid", uid);
	//data.insert("islogin", true);
	data.insert("last_update", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	data.insert("cookie", cookies);
	//qDebug() << "=================cookie" << cookies;
	if (account.isEmpty()) {
		add("accounts", data);
	}
	else {
		update("accounts", data, QString("uid='%1'").arg(uid));
	}
	
	return QJsonObject();
}

bool DBManager::setSyncAccount(QString uid, bool state) 
{
	QMap<QString, QVariant> data;
	data.insert("sync", state);
	return update("accounts", data, QString("uid='%1'").arg(uid));
}

bool DBManager::updateOpenTime(QString uid)
{
	QDateTime timestamp = QDateTime::currentDateTime();
	return updateAccountField(uid, "last_open", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
}

void DBManager::setAccountLogin(QString uid, QVariant sts)
{
	updateAccountField(uid, "islogin", sts);
}

bool DBManager::updateAccountField(QString uid, QString field, QVariant val)
{	
	QMap<QString, QVariant> data;
	data.insert(field, val);
	return update("accounts", data, QString("uid='%1'").arg(uid));
}

bool DBManager::updateAccount(QString uid, QJsonObject obj)
{
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}

	return update("accounts", data, QString("uid='%1'").arg(uid));
}

bool DBManager::removeAccount(QString uid)
{
	return remove("accounts", QString("uid='%1'").arg(uid));
}

QJsonArray DBManager::queryAccounts(QString uid, QString filter, int page)
{	
	QString innerfilter = "1=1 ";
	if (!uid.isEmpty()) {
		innerfilter = QString("uid='%1' ").arg(uid);
	}
	if (!filter.isEmpty()) {
		innerfilter += "and " + filter;
	}
	
	QList<QJsonObject> items = query("accounts", innerfilter, page);

	QJsonArray objs;
	for (QJsonObject obj : items)
	{
		//obj["logo"] = Utils::imageToBase64(CApplication::resourcePath() + "/" + obj["sitenick"].toString() + ".png");
		objs << obj;
	}
	return objs;
}

QList<QJsonObject> DBManager::queryAccountsList()
{
	return query("accounts", 0);
}

QJsonObject DBManager::queryAccount(QString uid)
{	
	QJsonArray accounts = queryAccounts(uid);
	if (accounts.empty()) {
		return QJsonObject();
	}
	return accounts[0].toObject();
}

bool DBManager::hasScheduledItems()
{
	return queryScheduledTweets("").count();
}



void DBManager::add_or_updateSite(QString nick, QJsonObject obj)
{
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}
	QJsonObject site = querySite(nick);

	if (site.isEmpty()) {
		add("site", data);
	}
	else {
		update("site", data, QString("nickname='%1'").arg(nick));
	}
}

int DBManager::tableCount(QString table, QString uid)
{
	int rowCount = 0;
	if (!uid.isEmpty()) {
		QSqlQuery query(m_db);
		query.prepare(QString("SELECT COUNT( 1 ) FROM %1 WHERE ownerid='%2'").arg(table).arg(uid));
		if (query.exec() && query.seek(0)) {
			rowCount = query.value(0).toInt();
		}
		return rowCount;
	}
	//update data
	if (m_tableInfo.value(table).rowCount == 0) {
		updateTableCount(table);
	}

	return m_tableInfo[table].rowCount;
}


QJsonArray DBManager::queryTable(QString tablename, QString ownerid, QString filter, int page) 
{
	QString innerfilter = "1=1 ";
	if (!ownerid.isEmpty()) {
		innerfilter = QString("ownerid='%1' ").arg(ownerid);
	}

	if (!filter.isEmpty()) {
		innerfilter += "and " + filter;

	}

	QList<QJsonObject> items = query(tablename, innerfilter, page, PAGE_SIZE);
	QJsonArray objs;
	for each (QJsonObject obj in items)
	{	
		obj["object"] = QJsonDocument::fromJson(obj["object"].toString().toUtf8()).object();
		objs.append(obj);
	}
	return objs;
}

QJsonArray DBManager::queryTasks(QString ownerid, QString filter, int page)
{
	return queryTable("tasks", ownerid, filter, page);
}

bool DBManager::removeTask(QString id)
{
	qDebug() << id;
	return remove("tasks", QString("id=%1").arg(id));;
}

int DBManager::addTask(QString ownerid, QJsonObject obj)
{
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}

	if (!data.contains("status")) {
		data.insert("status", 0);
	}

	if (!data.contains("model")) {
		data.insert("model", 0);
	}

	data.insert("create_dt", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	data.insert("last_update", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

	return add("tasks", data);
}

bool DBManager::updateTask(QString id, QJsonObject obj)
{
	return updateTableItem("tasks", id, obj);
}

//status -- 0 normal, 1 on progress, 2 success , 3 failed
bool DBManager::setTaskStatus(QString id, int status)
{
	QJsonObject json;
	json["status"] = status;
	return updateTask(id, json);
}

int DBManager::addSchedule(QString ownerid, QJsonObject obj)
{	
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}


	if (!data.contains("status")) {
		data.insert("status", 1);
	}

	if (!data.contains("model")) {
		data.insert("model", 0);
	}

	if (!data.contains("sync")) {
		data.insert("sync", true);
	}
	//default value
	data.insert("ownerid", ownerid);
	data.insert("active", true);
	data.insert("success", false);

	QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(data["timestamp"].toLongLong());
	data.insert("time", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
	if (timestamp.isNull()) {
		return false;
	}
	qDebug() << "[DBManager::addSchedule data]" << data;
	return add("resource", data);
}

int DBManager::addComment(QJsonObject obj)
{
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}

	if (!data.contains("status")) {
		//default 0 wait, 1 progressing, 2 success, 3, stop, -1 failed
		data.insert("status", 0);
	}

	if (!data.contains("pageurl") && !data.contains("site")) {
		QUrl pageurl = obj["pageurl"].toString();
		data.insert("site", pageurl.host());
	}

	return add("comments", data);
}

bool DBManager::removeComments(QString parentId)
{
	return remove("comments", QString("parentid='%1'").arg(parentId));
}

//return item id
int DBManager::addResource(QJsonObject obj)
{
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}
	
	if (!data.contains("status")) {
		//default 0 wait, 1 progressing, 2 success, 3, stop, -1 failed
		data.insert("status", 0);
	}

	if (!data.contains("model")) {
		data.insert("model", 0);
	}

	if (!data.contains("sync")) {
		data.insert("sync", true);
	}

	if (!data.contains("pageurl") && !data.contains("site")) {
		QUrl pageurl = obj["pageurl"].toString();
		data.insert("site", pageurl.host());
	}
	data.insert("active", true);
	data.insert("success", false);
	
	return add("resource", data);
}

QJsonObject DBManager::getResource(QString id)
{
	QList<QJsonObject> items = query("resource", QString("id='%1'").arg(id));
	if (items.count()) {
		return items.first();
	}
	return QJsonObject();
}

bool DBManager::updateResource(qint64 id, QJsonObject obj)
{
	return updateTableItem("resource", QString::number(id), obj);
}

//clone resource ---> update new owner info and obj
int DBManager::cloneResource(QString targetid, QString ownerid, QJsonObject obj)
{	
	QJsonObject resource = getResource(targetid);
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		resource[i.key()] = i.value();
	}
	resource.remove("id");
	resource.remove("create_dt");

	if (!ownerid.isEmpty()) {
		resource.insert("ownerid", ownerid);
		resource.insert("uid", ownerid);
	}

	return addSchedule(ownerid, resource);
}

bool DBManager::removeResource(QString id)
{
	return remove("resource", QString("id=%1").arg(id));
}

bool DBManager::cancelSchedule(QString id, QString ownerid)
{
	QMap<QString, QVariant> data;
	data.insert("timestamp", "");
	data.insert("time", "");

	if (!id.isEmpty()) {
		update("schedule2", data, QString("id='%1'").arg(id));
	}
	else {
		update("schedule2", data, QString("ownerid='%1'").arg(ownerid));
	}
	return false;
}

QJsonObject DBManager::updateSchedule(QString id, QString ownerid, QJsonObject obj)
{
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}
	qDebug() << "[DBManager::updateSchedule data]" << data;
	//resource donot set timestamp
	if (!data.value("timestamp").isNull()) {
		QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(data["timestamp"].toLongLong());
		if (!timestamp.isNull()) {
			data.insert("time", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
		}
	}

	if (!ownerid.isEmpty()) {
		data.insert("ownerid", ownerid);
		data.insert("uid", ownerid);
	}

	if (update("resource", data, QString("id='%1'").arg(id))) {
		//return queryResources(ownerid, "id=" + id).first().toObject();
	}
	return QJsonObject();
}

bool DBManager::removeSchedulebyTimestamp(QString t)
{
	return remove("schedule2", QString("timestamp=%1").arg(t));
}

QJsonObject DBManager::querySchedules(QString ownerid, QString filter, int page)
{
	QString innerfilter = "1=1 ";
	if (!ownerid.isEmpty()) {
		innerfilter = QString("ownerid='%1' ").arg(ownerid);
	}
	if (!filter.isEmpty()) {
		innerfilter += "and " + filter;
	}

	QList<QJsonObject> items = query("schedule2", innerfilter, page);
	QJsonObject objs;
	for each (QJsonObject obj in items)
	{
		objs[obj["timestamp"].toString()] = obj;
	}
	return objs;
}

QJsonArray DBManager::querySites(QString filter, int page)
{
	QString innerfilter = "1=1 ";
	if (!filter.isEmpty()) {
		innerfilter += "and " + filter;
	}
	QList<QJsonObject> items = query("site", innerfilter, page);

	QJsonArray objs;
	for (QJsonObject obj : items)
	{
		//obj["logo"] = Utils::imageToBase64(GApplication::iconPath() + "/" + obj["nickname"].toString() + ".png");
		objs << obj;
	}
	return objs;
}

QJsonObject DBManager::querySite(QString nickname) {
	//nickname is pingyin, such as douyin, sitename can be Chinese name
	QString innerfilter = QString("nickname = '%1'").arg(nickname);
	QJsonArray items = querySites(innerfilter, 1);
	if (items.count() == 0) {
		return QJsonObject();
	}
	return items.at(0).toObject();
}

QJsonArray DBManager::queryGroups(QString ownerid, QString filter, int page)
{
	QString innerfilter = "1=1 ";
	if (!ownerid.isEmpty()) {
		innerfilter = QString("ownerid='%1' ").arg(ownerid);
	}
	if (!innerfilter.toUpper().contains("ORDER BY")) {
		innerfilter += " ORDER BY id DESC";
	}
	if (!filter.isEmpty()) {
		innerfilter += "and " + filter;
	}
	QList<QJsonObject> items = query("groups", innerfilter, page);

	QJsonArray objs;
	for (QJsonObject obj : items)
	{
		objs << obj;
	}
	return objs;
}

void DBManager::addGroup(QString ownerid, QJsonObject obj)
{
	QMap<QString, QVariant> data;
	QJsonObject::const_iterator i;
	for (i = obj.constBegin(); i != obj.constEnd(); ++i) {
		data.insert(i.key(), i.value().toVariant());
	}

	if (!data.contains("status")) {
		data.insert("status", 1);
	}

	if (!data.contains("model")) {
		data.insert("model", 0);
	}

	//default value
	data.insert("ownerid", ownerid);

	add("groups", data);
}

void DBManager::removeGroup(QString id)
{
	remove("groups", QString("id='%1'").arg(id));;
}

//status = 1 normal, 0 unexist, 2 error
QJsonArray DBManager::queryResources(QString ownerid, QString filter, int page)
{
	QString innerfilter = "1=1 ";
	if (!ownerid.isEmpty()) {
		innerfilter = QString("ownerid='%1' ").arg(ownerid);
	}

	if (!filter.isEmpty()) {
		innerfilter += "and " + filter;
		
	}

	if (!innerfilter.toUpper().contains("ORDER BY")) {
		innerfilter += " ORDER BY id DESC";
	}
	qDebug() << "[DBManager::queryResources innerfilter]" << innerfilter;
	QList<QJsonObject> items = query("resource", innerfilter, page);
	QJsonArray objs;
	for each (QJsonObject obj in items)
	{
		obj["key"] = obj["id"].toString();
		obj["icon"] = Utils::getNativeIcon(QFileInfo(obj["filepath"].toString()));
		objs.append(checkAndUpdateResource(obj));
	}
	return objs;
}

bool DBManager::addMessage(QString uid, QString cate, QVariant content)
{
	QMap<QString, QVariant> data;
	data.insert("uid", uid);
	data.insert("catelog", cate);
	data.insert("content", content);

	return add("message", data);
}

bool DBManager::removeMessage(QString id)
{
	return remove("message", QString("id='%1'").arg(id));;
}

QJsonArray DBManager::queryMessages(QString ownerid, QString filter, int page)
{
	QString innerfilter = "1=1 ";
	if (!ownerid.isEmpty()) {
		innerfilter = QString("ownerid='%1' ").arg(ownerid);
	}

	if (!filter.isEmpty()) {
		innerfilter += "and " + filter;

	}

	QList<QJsonObject> items = query("message", innerfilter, page, PAGE_SIZE);
	QJsonArray objs;
	for each (QJsonObject obj in items)
	{
		objs.append(obj);
	}
	return objs;
}

QJsonObject DBManager::checkAndUpdateResource(QJsonObject item)
{
	QString filepath = item["filepath"].toString();
	int status = item["status"].toInt();

	if (QFileInfo(filepath).exists() && status == 1) {
		//check exist
		item["exist"] = true;
	}
	else if(!QFileInfo(filepath).exists() && status == 0) {
		//check unexist
		item["exist"] = false;
	}
	else {
		item["exist"] = QFileInfo(filepath).exists();
		//update unexist status
		QMap<QString, QVariant> data;
		data.insert("status", item["exist"].toInt());
		update("resource", data, QString("id=%1").arg(item["id"].toInt()));
	}

	return item;
}

//used for trigger next schedule for all users
QJsonObject DBManager::queryValidSchedule(bool withTweet)
{
	QList<QJsonObject> items;
	if (withTweet) {
		items = query("schedule2", QString("time>'%1' and active=1 ORDER BY time LIMIT 1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
	}
	else {
		items = query("schedule2", QString("type not in ('tweet', 'video', 'article') and time>'%1' and active=1 ORDER BY time LIMIT 1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
	}
	
	if (items.empty()) {
		return QJsonObject();
	}
	return items.first();
}

QStringList DBManager::queryScheduledAccounts()
{
	QList<QJsonObject> items = query("schedule2", QString("time>'%1' and active=1 GROUP BY ownerid").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
	if (items.empty()) {
		return QStringList();
	}
	QStringList accounts;
	for each (QJsonObject obj in items) {
		accounts.append(obj["ownerid"].toString());
	}
	return accounts;
}

QJsonObject DBManager::queryScheduledTweets(QString ownerid)
{
	return querySchedules(ownerid, QString("type in ('tweet', 'video', 'article') and time>='%1' and active=1 ORDER BY time").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
}

QJsonObject DBManager::queryAllScheduledTweets(int hours, bool ascending, int limit)
{
	
	return QJsonObject();
}

void DBManager::setErrorCode(QString id, QString code) 
{	
	QJsonObject json;
	json["success"] = code;
	updateSchedule(id, NULL, json);
}

int DBManager::getRowCounts(const QString& tableName)
{
	QSqlQuery query;
	if (!query.exec("SELECT COUNT(*) FROM " + tableName)) {
		qDebug() << "Error executing the query:" << query.lastError().text();
		return -1;
	}
	if (query.next()) {
		int rowCount = query.value(0).toInt();
		return rowCount;
	}
	return 0;
}

SqlTableModel::SqlTableModel(QObject * parent, QSqlDatabase db)
	:QSqlTableModel(parent, db)
{
}

SqlTableModel::~SqlTableModel()
{
}

void SqlTableModel::setSqlQuery(const QSqlQuery &query)
{
	setQuery(query);
}

void SqlTableModel::setSqlQuery(const QString &sql)
{
	QSqlQuery query(sql);
	setQuery(query);
}
