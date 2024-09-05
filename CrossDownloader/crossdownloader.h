#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_crossdownloader.h"
#include "ui_inputDialog.h"
#include "ui_resourceDialog.h"
#include "WebSocketServer.h"
#include "type.h"

QT_FORWARD_DECLARE_CLASS(DBManager)
QT_FORWARD_DECLARE_CLASS(ApiManager)
QT_FORWARD_DECLARE_CLASS(SqlTableModel)
QT_FORWARD_DECLARE_CLASS(DownloadHandler)
QT_FORWARD_DECLARE_CLASS(SiteManager)

/*
	parse and covert input string
*/
class InputDialog : public QDialog
{
	Q_OBJECT

public:
	InputDialog(QWidget *parent = nullptr);
	InputDialog(APIType type, QWidget * parent = nullptr);
	~InputDialog();

	void setupViewConnections();

	bool checkContent(const QString & text);

	QStringList extractUrls(const QString & inputString);

signals:
	// if type is None, it will determine the apitype
	void downloadUrls(const QStringList& urls, APIType type = APIType::NONE);

protected:
	APIType InputDialog::currentType();

	bool validator(const QString & url, APIType type);

private:
	Ui::InputDialogUI ui;
};

class ResourceDialog : public QDialog
{
	Q_OBJECT
public:
	ResourceDialog(QJsonObject res, QWidget *parent = nullptr);
	ResourceDialog(QWidget *parent = nullptr);
	~ResourceDialog();

	void switchStatus(bool isLoading);

	void setTitle(const QString & title);

	void centerWidget();

	void buildList(const QJsonObject & res);

	void setupViewConnections();

	void setInfo(const QString& info, int count);

signals:
	void download(const QJsonObject&);
	void userClose();

private:
	Ui::ResDialog ui;
	QTimer m_timer;
};

class CrossDownloader : public QMainWindow
{
    Q_OBJECT

public:

    CrossDownloader(QWidget *parent = nullptr);
    ~CrossDownloader();

	void initData();

	void loadProxy();


private:
	void buildUI();

	void updateActionBar(const QString site);

	void actionBarSetup();

	void setActionBarHeader(const QString & text);

	void removeCustomizedActions();

	void customizeActions();

	void initScript();

	void setupWindowInfo();

	void screenAdapter();

	void setupViewConnections();

	void addDownloadImpl(const QJsonObject item);

private slots:
	void openInput(APIType type = APIType::NONE);

	QString prepareTemplate(QString viewtype);

	int addTab(const QString subject, TabType type, int id = 0);

	void processRequests(int id, APIType type, QString info);

	void processCommentsTabView(int parentid, int index, const QJsonObject & results);

	void processLiveView(const QString & url);

	void processRequests(const QStringList & urls, const QString& meta = 0, APIType type = APIType::NONE, const QJsonObject & params = QJsonObject());

	void processRequestsImpl(const ResourceDialog* resDialog, const QJsonObject & results);

	void loginNotify(const QString & site);

	void processDownloads(const QJsonObject & results);

	void errorHandler(const QString & msg);

	void addDownload(const QJsonObject item, bool process = true);

	void addDownload(int id);

	QString formatFilename(const QString& folder, const QString& basename, const QString& suffix);

	void openWorker(const QString url);

	//void playCloseAnimation();

protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

private:
    Ui::CrossDownloaderClass ui;
	WebSocketServer* m_websocketserv;
	QMap<QString, QString> m_templates;
	std::unique_ptr<SiteManager> m_sitemanager;
	std::unique_ptr<DBManager> m_dbmanager;
	std::unique_ptr<SqlTableModel> m_model;
	std::unique_ptr<ApiManager> m_apimanager;
	std::unique_ptr<DownloadHandler> m_downloadhandler;
	QIcon m_videoIcon;
	int isplayCloseAnimation;
};
