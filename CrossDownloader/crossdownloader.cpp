#include "stdafx.h"
#include "crossdownloader.h"
#include "Dbmanager.h"
#include "SiteManager.h"
#include "CApplication.h"
#include <QNetworkProxy>
#include "DownloadHandler.h"
#include "Setting.h"
#include "ApiManager.h"
#include "TabWidget.h"
#include "WebWidget.h"

CrossDownloader::CrossDownloader(QWidget *parent)
    : QMainWindow(parent)
	, m_websocketserv(nullptr)
	, m_downloadhandler(std::make_unique<DownloadHandler>())
	, m_sitemanager(std::make_unique<SiteManager>())
	, isplayCloseAnimation(0)
{
    ui.setupUi(this);

	m_dbmanager = std::make_unique<DBManager>(CApplication::dataPath() + "/db");
	m_apimanager = std::make_unique<ApiManager>(m_dbmanager.get(), this);

	qDebug() << "Path:" << CApplication::dataPath();

	QTimer::singleShot(3 * 1000, [=]() {
		m_websocketserv = new WebSocketServer(m_dbmanager.get(), 0);

		connect(m_websocketserv, &WebSocketServer::downloadFinish, [=](QJsonObject item) {
			addDownload(item, false);
			});
		});
	
	m_videoIcon = Utils::getLocalFileIcon("mp4");

	buildUI();
	initData();

	screenAdapter();
	setupWindowInfo();
	setupViewConnections();
}

CrossDownloader::~CrossDownloader()
{
	qDebug() << "CrossDownloader::~CrossDownloader";
	m_downloadhandler.get_deleter();
}

void CrossDownloader::setupWindowInfo()
{
	setWindowTitle(QStringLiteral("CrossDownloader") + QStringLiteral(" %1").arg(VERSION));


	//hide topleft Icon
	//QPixmap pixmap(32, 32);
	//pixmap.fill(Qt::transparent);
	setWindowIcon(QIcon(":/CrossDownloader/Resources/logo.ico"));
}

void CrossDownloader::screenAdapter()
{
	int width = 1000;
	int height = 700;
	double objectRate = CApplication::screenRate();
	resize(width*objectRate, height*objectRate);
	//setMinimumSize(1000, 600);
	//set screen center
	setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			size(),
			qApp->desktop()->availableGeometry()
		)
	);
}

void CrossDownloader::paintEvent(QPaintEvent *event) {
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void CrossDownloader::setupViewConnections()
{
	connect(ui.addUrl, &QAction::triggered, [=]() {
		openInput(APIType::NONE);
		});
	connect(ui.monitor, &QAction::triggered, [=]() {
		openInput(APIType::LIVE);
		});
	connect(ui.openDownloadFolder, &QAction::triggered, [=]() {
		QDesktopServices::openUrl(QUrl::fromLocalFile(CApplication::downloadPath()));
		});
	connect(ui.setting, &QAction::triggered, [=]() {
		Setting* settingView = new Setting(this);
		connect(settingView, &Setting::confirm, [=]() {
			loadProxy();
			customizeActions();
			});
		});

	connect(m_apimanager.get(), &ApiManager::error, [=](const QString& errmessage) {
		QMessageBox::warning(this, "CrossDownloader", errmessage);
		statusBar()->showMessage(QStringLiteral("错误：请5分钟后重启软件再试！"));
		});

	connect(m_apimanager.get(), &ApiManager::info, [=](const QString& message) {
		statusBar()->showMessage(message);
		});

	connect(ui.action_Worker, &QAction::triggered, [=]() {
		m_apimanager->showWorkerView("douyin");
		});

	connect(ui.tableWidget, &ResourceTableWidget::start, [=](int id) {
		addDownload(id);
		});

	connect(ui.tableWidget, &ResourceTableWidget::remove, [=](int id) {
		m_downloadhandler->stop(id);
		});

	connect(ui.tableWidget, &ResourceTableWidget::cellDoubleClicked, [=](int row, int column) {
		QJsonObject rowInfo = ui.tableWidget->getRowInfo(row);
		QDesktopServices::openUrl(QUrl::fromLocalFile(rowInfo["filepath"].toString()));
		});

	connect(ui.tableWidget, &ResourceTableWidget::openResource, [=](int id, APIType type, QString info) {
		processRequests(id, type, info);
		});

	connect(ui.tabWidget, &QTabWidget::tabCloseRequested, [=](int index) {
		QWidget* widgetToRemove = ui.tabWidget->widget(index);
		qDebug() << "QTabWidget::tabCloseRequested" << index << widgetToRemove;
		ui.tabWidget->removeTab(index);
		//ui.tabWidget->removeTab(index);
		widgetToRemove->deleteLater();
		});

	m_downloadhandler->events([&](int id) {
			// start
			qDebug() << "start" << id;
			int row = ui.tableWidget->getRow(id);
			QTableWidgetItem* item = ui.tableWidget->item(row, 0);
			if (item) {
				item->setIcon(QIcon(":/CrossDownloader/Resources/downloading.png"));
				ui.tableWidget->updateView();
			}

		}, [&](int id, qint64 bytesReceived, qint64 bytesTotal) {
			// progress
			int row = ui.tableWidget->getRow(id);
			//qDebug() << "Progress" << bytesReceived << bytesTotal << id << row;

			QTableWidgetItem* item = ui.tableWidget->item(row, 1);
			if (item) {
				item->setText(QString("%1%").arg(int((static_cast<double>(bytesReceived) / bytesTotal) * 100)));
			}

		}, [&](int id) {
			// finish
			qDebug() << "Task with is finished" << id;
			int row = ui.tableWidget->getRow(id);
			QTableWidgetItem* item = ui.tableWidget->item(row, 0);
			if (item) {
				item->setIcon(m_videoIcon);
				//update db status
				QJsonObject res;
				res["status"] = 2;
				m_dbmanager->updateResource(id, res);
				ui.tableWidget->updateView();
			}


		}, [&](int id, const QString& error) {
			// error
			qDebug() << "Task Error." << id << error;
			int row = ui.tableWidget->getRow(id);
			QTableWidgetItem* item = ui.tableWidget->item(row, 0);
			if (item) {
				item->setIcon(QIcon(":/CrossDownloader/Resources/fail.png"));
				//update db status
				QJsonObject res;
				res["status"] = -1;
				m_dbmanager->updateResource(id, res);
				ui.tableWidget->updateView();
			}
			ui.tableWidget->item(row, 1)->setText("Err");
		});
}

void CrossDownloader::initData()
{
	ui.tableWidget->setDB(m_dbmanager.get());
	ui.tableWidget->loadResourceData();
}

void CrossDownloader::loadProxy()
{
	QSettings setting(CApplication::settingFile(), QSettings::IniFormat);
	QString proxyip = setting.value("proxy").toString();
	if (proxyip.isEmpty()) {
		QNetworkProxyFactory::setUseSystemConfiguration(CD_DEBUG);
	}
	else {
		QStringList ipinfo = proxyip.split(":");

		QNetworkProxy proxy;
		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(ipinfo[0]);
		proxy.setPort(ipinfo[1].toInt());
		QNetworkProxy::setApplicationProxy(proxy);
	}
}

void CrossDownloader::buildUI()
{
	ui.tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);
	//ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui.tableWidget->setColumnWidth(0, 300);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->setMenuType(ResourceTableWidget::Menu::UserType);

	//ui.monitor->setVisible(false);

	QWidget* spacer = new QWidget;
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui.toolBar->addWidget(spacer);
	//ui.toolBar->addAction(QIcon(":/CrossDownloader/Resources/site.png"), QStringLiteral("添加网站"), [=]() {

	//	});
	ui.toolBar->addAction(QIcon(":/CrossDownloader/Resources/info.png"), "About", [=] (){
		QDesktopServices::openUrl(QUrl(PROTAL));
		});




	setStyleSheet("font-family:'Microsoft YaHei';");

	//set statusbar
	QLabel *versionlabel = new QLabel(this);
	versionlabel->setStyleSheet("color:#999;");
	versionlabel->setText(QString::fromLocal8Bit("主程序版本：%1.%2 ").arg(QCoreApplication::applicationVersion(), QDateTime::currentDateTime().toString("MMddyyhhmm")));
	
	statusBar()->addPermanentWidget(versionlabel);
	statusBar()->showMessage(QStringLiteral("准备中..."));
	
	QActionGroup *actionGroup = new QActionGroup(this);
	for (auto action : ui.siteBar->actions()) {
		actionGroup->addAction(action);
	}

	QObject::connect(actionGroup, &QActionGroup::triggered, [&](QAction *action) {
		qDebug() << "Action toggled:" << action->text();
		ui.actionBar->clear();
		setActionBarHeader(action->text());
		updateActionBar(action->objectName());
		});

	actionBarSetup();
}

void CrossDownloader::updateActionBar(const QString site) {
	QMap<QString, std::function<void()>> douyinActionMap;
	QMap<QString, QStringList> siteActions = m_sitemanager->actions(site);
	for (QString tag : siteActions.keys()) {
		QStringList value = siteActions.value(tag);
		douyinActionMap.insert(tag, [=]() { processRequests({ value[0] }, value[1]); });
	}

	for (const QString &actionName : douyinActionMap.keys()) {
		QAction *action = new QAction(actionName, ui.actionBar);
		QObject::connect(action, &QAction::triggered, douyinActionMap[actionName]);
		ui.actionBar->addAction(action);
	}

	customizeActions();
}

void CrossDownloader::actionBarSetup()
{
	setActionBarHeader(QStringLiteral("抖音"));
	updateActionBar("douyin");
}

void CrossDownloader::setActionBarHeader(const QString & text)
{
	if(ui.actionBar->actions().count() == 0){
		QLabel* head = new QLabel(text);
		head->setObjectName("actionbar_head");
		ui.actionBar->addWidget(head);
	}
	else {
		QLabel* header = (QLabel*)ui.actionBar->widgetForAction(ui.actionBar->actions()[0]);
		header->setText(text);
	}
}

void CrossDownloader::removeCustomizedActions()
{
	foreach(QAction* action, ui.actionBar->actions()) {
		if (action->data().toString() == "customize") {
			ui.actionBar->removeAction(action);
		}
	}
}

void CrossDownloader::customizeActions()
{
	removeCustomizedActions();
	QSettings setting(CApplication::settingFile(), QSettings::IniFormat);
	QStringList keywords = setting.value("keywords").toString().split(QRegExp("[,，、]"), QString::SkipEmptyParts);

	QAction *add = new QAction(QIcon(":/CrossDownloader/Resources/addAction.png"), QString(), ui.actionBar);
	add->setData("customize");
	QObject::connect(add, &QAction::triggered, [=]() {
		ui.setting->trigger();
		});
	for (QString keyword : keywords) {
		QAction *action = new QAction(keyword, ui.actionBar);
		action->setData("customize");
		QObject::connect(action, &QAction::triggered, [=]() { 
			processRequests({ QString("https://www.douyin.com/search/%1").arg(keyword) }, keyword);
			});
		ui.actionBar->addAction(action);
	}
	ui.actionBar->addAction(add);
}

void CrossDownloader::openInput(APIType type)
{
	InputDialog* dialog = new InputDialog(type, this);
	connect(dialog, &InputDialog::downloadUrls, [=](QStringList urls, APIType type) {
		if (type == APIType::COMMENT) {
			addTab(QStringLiteral("评论"), TabType::COMMENT);
			processRequests(urls, 0, type);
		}
		else if (type == APIType::LIVE) {
			addTab(QStringLiteral("直播监控"), TabType::LIVE);
			processLiveView(urls.at(0));
		}
		else {
			processRequests(urls, 0, type);
		}
		
		});
}

QString CrossDownloader::prepareTemplate(QString viewtype)
{
	QString tpl = m_templates.value(viewtype);
	if (tpl.isEmpty() || CD_DEBUG) {
		QString path = QString(":/CrossDownloader/Resources/%1.html").arg(viewtype);
		if (CD_DEBUG) {
			path = PROJECT_ROOT + QString("/CrossDownloader/Resources/%1.html").arg(viewtype);
		}

		QFile file(path);
		file.open(QFile::ReadOnly);
		tpl = file.readAll();
		file.close();
		m_templates.insert(viewtype, tpl);
	}
	return tpl;
}

//get or create new tab widget
//if id is 0, create new one
int CrossDownloader::addTab(const QString subject, TabType type, int id)
{
	QWidget* widget = nullptr;
	if (id > 0) {
		//query tab what if exist
		for (int index = 0; index < ui.tabWidget->count(); index++) {
			widget = ui.tabWidget->widget(index);
			if (widget->property("key").toInt() == id) {
				ui.tabWidget->setTabText(index, subject);
				return index;
			}
		}
	}
	
	QIcon icon;
	switch (type)
	{
	case TabType::COMMENT:
		widget = new CommentWidget(m_dbmanager.get(), this);
		icon = QIcon(":/CrossDownloader/Resources/comment.png");
		connect(qobject_cast<CommentWidget*>(widget), &CommentWidget::commentUrl, [=](const QString & url, int total) {
			qDebug() << ">>>addTab" << url << total;
			QJsonObject params;
			params["total"] = total;
			processRequests({ url }, 0, APIType::COMMENT, params);
			});
		break;
	case TabType::ANALYZE:
		break;
	case TabType::LIVE:
		// currently, only for douyin
		widget = new WebWidget(m_dbmanager.get(), "douyin", this);
		qobject_cast<WebWidget*>(widget)->setApiManager(m_apimanager.get());
		icon = QIcon(":/CrossDownloader/Resources/live.png");
		break;
	default:
		break;
	}

	int index = -1;
	if (widget) {
		if (id != 0) {
			widget->setProperty("key", id);
		}
		index = ui.tabWidget->addTab(widget, icon, subject);
		ui.tabWidget->setCurrentIndex(index);
	}
	return index;
}

void CrossDownloader::processRequests(int id, APIType type, QString info) 
{
	qDebug() << "openResource" << id << info;
	statusBar()->showMessage(QStringLiteral("读取数据中..."));
	//ResourceDialog* resDialog = new ResourceDialog(this);
	//qDebug() << "openInput" << urls << accessManager.resolveUrl(urls[0]);
	if (type == APIType::USER) {
		m_apimanager->call(id, type, [=](ResourceDialog* resDialog, const QJsonObject& results) {
			qDebug() << "Open ResourceDialog APIType::USER";
			resDialog->setInfo(info, results["videos"].toArray().count());

			processRequestsImpl(resDialog, results);
			});
	}
	else if (type == APIType::SEARCH) {
		m_apimanager->search(id, info, [=](ResourceDialog* resDialog, const QJsonObject& results) {
			qDebug() << "Open ResourceDialog APIType::SEARCH";
			resDialog->setInfo(info, results["videos"].toArray().count());

			processRequestsImpl(resDialog, results);
			});
	}
	else if (type == APIType::DETAIL) {
		m_apimanager->call(id, type, [=](ResourceDialog* resDialog, const QJsonObject& results) {
			qDebug() << "Open ResourceDialog APIType::DETAIL";
			//resDialog->setInfo(info, results["videos"].toArray().count());

			processRequestsImpl(resDialog, results);
			});
	}
	else if (type == APIType::COMMENT) {
		int index = addTab(QStringLiteral("评论"), TabType::COMMENT, id);
		m_apimanager->call(id, type, [=](ResourceDialog* resDialog, const QJsonObject& results) {
			qDebug() << "Open ResourceDialog APIType::COMMENT";
			//resDialog->setInfo(info, results["videos"].toArray().count());

			processCommentsTabView(id, index, results);
			});
	}
}

void CrossDownloader::processCommentsTabView(int parentid, int index, const QJsonObject& results) 
{
	if (results.isEmpty()) {
		errorHandler(QStringLiteral("无法读取！请检查视频是否包含评论或稍后再试！"));
		return;
	}
	statusBar()->showMessage(QStringLiteral("成功"));

	CommentWidget* widget = qobject_cast<CommentWidget*>(ui.tabWidget->widget(index));
	ui.tabWidget->setTabText(index, QStringLiteral("评论(%1)").arg(results["comments"].toArray().count()));

	widget->setInfo(parentid);
	widget->addComments(results);
}

void CrossDownloader::processLiveView(const QString& url)
{
	int index = ui.tabWidget->currentIndex();
	WebWidget* view = qobject_cast<WebWidget*>(ui.tabWidget->widget(index));
	if (CD_DEBUG) {
		view->load(QUrl("http://localhost:5173/"));
	}
	else {
		view->setHtml(prepareTemplate("liveDashboard"), QUrl("https://live.douyin.com"));
	}
	connect(view, &WebView::loadFinished, [=]() {
		QJsonObject json;
		json["room"] = url;
		//wait for page loaded
		QTimer::singleShot(100, [=]() {
			view->sendParams("connect", json);
			});
		
		});
	
}

/*
core function to send request to target view and get ajax results
*/
void CrossDownloader::processRequests(const QStringList& urls, const QString& info, APIType type, const QJsonObject & params)
{
	statusBar()->showMessage(QStringLiteral("读取数据中..."));
	//ResourceDialog* resDialog = new ResourceDialog(this);
	//qDebug() << "openInput" << urls << accessManager.resolveUrl(urls[0]);
	int index;
	if (type == APIType::COMMENT) {
		
	}
	m_apimanager->call(urls[0], type, [=](ResourceDialog* resDialog, const QJsonObject& results) {
		qDebug() << "Open ResourceDialog" << results.value("resourceView") << results.value("resourceView").toBool();

		if (type == APIType::COMMENT) {
			processCommentsTabView(0, ui.tabWidget->currentIndex(), results);
		}
		else {
			resDialog->setInfo(info, results["videos"].toArray().count());
			processRequestsImpl(resDialog, results);
		}
		
		}, params);
}

void CrossDownloader::processRequestsImpl(const ResourceDialog* resDialog, const QJsonObject& results)
{
	if (results.value("resourceView").toBool()) {
		statusBar()->showMessage(QStringLiteral("成功"));
		connect(resDialog, &ResourceDialog::download, this, &CrossDownloader::processDownloads);
	}
	else if (results.value("loginView").toBool()) {
		statusBar()->showMessage(QStringLiteral("未登录"));
		//QMessageBox::warning(0, QStringLiteral("CrossDownloader HINT"), "login required " + results.value("site").toString());
		loginNotify(results.value("site").toString());
	}
	else if (results.isEmpty()) {
		errorHandler(QStringLiteral("读取失败！请检查链接并稍后再试！"));
	}
	//TODO: process comments
	else {
		statusBar()->showMessage(QStringLiteral("成功"));
		processDownloads(results);
	}
}

void CrossDownloader::loginNotify(const QString& site)
{
	QMessageBox msgBox;
	msgBox.setWindowTitle(QStringLiteral("登录提醒"));
	QString message;
	if (site == "xiaohongshu") {
		message = QStringLiteral("该功能需要登录【小红书】进行操作！");
	}
	msgBox.setText(message);
	msgBox.setIcon(QMessageBox::Information);

	// 添加自定义按钮
	QPushButton *loginButton = msgBox.addButton(QStringLiteral("登录"), QMessageBox::ActionRole);
	QPushButton *cancelButton = msgBox.addButton(QStringLiteral("取消"), QMessageBox::RejectRole);

	loginButton->setStyleSheet("background-color: #007bff; color: white;");
	// 设置默认按钮
	msgBox.setDefaultButton(cancelButton);

	// 显示消息框，并等待用户响应
	msgBox.exec();
	if (msgBox.clickedButton() == loginButton) {
		// 用户点击了登录按钮
		// 在这里添加处理登录的代码
		m_apimanager->login(site, [=]() {
			//login success
			//close worker, in order to renew cookie
			m_apimanager->workerClose(site);
			}, true, this);
			
	}
	else {
		// 用户点击了取消按钮或关闭了消息框
		// 在这里添加取消操作的代码
	}
}

void CrossDownloader::processDownloads(const QJsonObject& results)
{
	//jump to first index
	ui.tabWidget->setCurrentIndex(0);

	QString folder = results["folder"].toString();
	if (results.contains("downloads")) {
		QJsonArray downloadItems;
		//multi downloads
		foreach(auto item, results["downloads"].toArray()) {
			int id = m_dbmanager->addResource(item.toObject());
			//qDebug() << "addResourceID" << item.toObject();
			QJsonObject itemObj = item.toObject();
			//download item ID
			itemObj["id"] = id;
			itemObj["folder"] = folder;
			downloadItems.prepend(itemObj);
			ui.tableWidget->prepand(itemObj);
		}
		//in order to download item from head to toe
		foreach(auto itemObj, downloadItems) {
			addDownloadImpl(itemObj.toObject());
		}
	}
	else {
		//single item
		int id = m_dbmanager->addResource(results);
		QJsonObject itemObj = results;
		itemObj["id"] = id;
		itemObj["folder"] = folder;

		addDownload(itemObj);
	}
}

void CrossDownloader::errorHandler(const QString & msg)
{
	statusBar()->showMessage(QStringLiteral("失败"));
	QMessageBox::warning(0, QStringLiteral("CrossDownloader HINT"), msg);
}

// add new download item and start downloading
void CrossDownloader::addDownload(const QJsonObject item, bool process)
{
	//new added item, should set waitting status!
	item["status"] = 0;
	ui.tableWidget->prepand(item);
	if (process) {
		addDownloadImpl(item);
	}
}

// start download item which already in the table
void CrossDownloader::addDownload(int id)
{
	QJsonObject item = m_dbmanager->getResource(QString::number(id));
	item["status"] = 0;
	ui.tableWidget->updateItemStatus(id, 0);
	addDownloadImpl(item);
}

void CrossDownloader::addDownloadImpl(const QJsonObject item)
{
	qDebug() << "CrossDownloader::addDownloadImpl" << item;
	QString outputfilepath = item["filepath"].toString();
	if (outputfilepath.isEmpty()) {
		outputfilepath = formatFilename(item["folder"].toString(), item["subject"].toString(), item["type"].toString());
		QJsonObject res;
		res["filepath"] = outputfilepath;
		
		m_dbmanager->updateResource(item["id"].toInt(), res);
	}
	
	m_downloadhandler->addDownload(item["id"].toInt(), item["url"].toString(), outputfilepath);

}

QString CrossDownloader::formatFilename(const QString& folder, const QString& basename, const QString& suffix)
{	
	QString dir = qobject_cast<CApplication*>(qApp)->downloadPath();
	if (!folder.isEmpty()) {
		QString foldername = Utils::makeValidWindowsFilename(folder);
		dir = Utils::makeFolder(qobject_cast<CApplication*>(qApp)->downloadPath() + "/" + foldername);
	}

	QString name = Utils::makeValidWindowsFilename(basename);
	return Utils::getUniqueFileName(dir + "/" + QString("%1.%2").arg(name).arg(suffix));
}

void CrossDownloader::openWorker(const QString url)
{

}
/*
void CrossDownloader::playCloseAnimation()
{
	setMinimumSize(0, 0);
	QPropertyAnimation* closeAnimation = new QPropertyAnimation(this, "geometry");
	closeAnimation->setStartValue(geometry());
	closeAnimation->setEndValue(QRect(geometry().x(), geometry().y() + height() / 2, width(), 0));
	closeAnimation->setDuration(300);
	isplayCloseAnimation = 1;
	connect(closeAnimation, SIGNAL(finished()), this, SLOT(close()));
	closeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}
*/
void CrossDownloader::closeEvent(QCloseEvent* event)
{
	m_downloadhandler->stopAll();
	event->accept();
	
	qDebug() << "CrossDownloader::closeEvent";
	//page()->deleteLater();
}

InputDialog::InputDialog(APIType type, QWidget *parent)
	:InputDialog(parent)
{
	switch (type)
	{
	case APIType::NONE:
	case APIType::DETAIL:
	case APIType::USER:
	case APIType::SEARCH:
	case APIType::COLLECTION:
		ui.videoTypeButton->click();
		break;
	case APIType::COMMENT:
		ui.commentTypeButton->click();
		break;
	case APIType::LIVE:
		ui.monitorButton->click();
		break;
	default:
		break;
	}
}

InputDialog::InputDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle(QStringLiteral("添加数据"));
	setAttribute(Qt::WA_DeleteOnClose, true);

	ui.downloadPath->setText(CApplication::downloadPath());

	ui.videoTypeButton->setProperty("type", 0);
	ui.commentTypeButton->setProperty("type", 1);
	ui.monitorButton->setProperty("type", 2);
	//ui.monitorButton->hide();
	//read clipboard
	QClipboard *clipboard = QApplication::clipboard();
	ui.textEdit->setText(clipboard->text());

	setupViewConnections();
	show();
}

InputDialog::~InputDialog()
{

}

APIType InputDialog::currentType()
{
	QList<QRadioButton*> radioButtons = { ui.videoTypeButton, ui.commentTypeButton, ui.monitorButton };

	for (QRadioButton* radioButton : radioButtons) {
		if (radioButton->isChecked()) {
			// This radio button is checked
			QString checkedButtonName = radioButton->text();
			qDebug() << "Checked radio button:" << checkedButtonName;

			int type = radioButton->property("type").toInt();
			switch (type)
			{
			case 0:
				return APIType::NONE;
			case 1:
				return APIType::COMMENT;
			case 2:
				return APIType::LIVE;
			default:
				return APIType::NONE;
			}
		}
	}
	return APIType::NONE;
}

bool InputDialog::validator(const QString& url, APIType type)
{
	bool status = true;
	switch (type)
	{
	case APIType::NONE:
		break;
	case APIType::DETAIL:
		break;
	case APIType::USER:
		break;
	case APIType::SEARCH:
		break;
	case APIType::COLLECTION:
		break;
	case APIType::CHENNAL:
		break;
	case APIType::COMMENT:
		break;
	case APIType::LIVE:
		if (url.indexOf("live.douyin.com") == -1) {
			status = false;
		}
		break;
	case APIType::OTHER:
		break;
	default:
		break;
	}
	return status;
}

void InputDialog::setupViewConnections()
{
	connect(ui.confirmButton, &QPushButton::clicked, [=]() {
		QString inputString = ui.textEdit->toPlainText();
		QStringList urls = Utils::extractUrls(inputString);
		if (urls.count() == 0) {
			return;
		}
		if (!Utils::siteVerfiy(inputString) || !validator(urls.at(0), currentType())) {
			QMessageBox::warning(0, QStringLiteral("CrossDownloader HINT"), QStringLiteral("输入链接有误，目前支持抖音,小红书，请重新输入"));
			return;
		}
		
		emit downloadUrls(urls, currentType());
		accept();
		});

	connect(ui.dirButton, &QToolButton::clicked, [=]() {
		QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
		QString directory = QFileDialog::getExistingDirectory(0,
			QStringLiteral("选择下载文件夹"),
			"",
			options);
		if (!directory.isEmpty()) {
			ui.downloadPath->setText(directory);
			QSettings setting(CApplication::settingFile(), QSettings::IniFormat);
			setting.setValue("downloadPath", directory);
		}
		});

	connect(ui.videoTypeButton, &QRadioButton::clicked, [=]() {
		ui.textEdit->setPlaceholderText(QStringLiteral("支持输入视频链接、口令、用户页面、搜索链接、合辑"));
		ui.textEdit->setProperty("type", "video");
		});

	connect(ui.commentTypeButton, &QRadioButton::clicked, [=]() {
		ui.textEdit->setPlaceholderText(QStringLiteral("视频链接、口令"));
		ui.textEdit->setProperty("type", "comment");
		});

	connect(ui.monitorButton, &QRadioButton::clicked, [=]() {
		ui.textEdit->setPlaceholderText(QStringLiteral("直播间地址"));
		ui.textEdit->setProperty("type", "monitor");
		});
}

ResourceDialog::ResourceDialog(QWidget *parent)
	:QDialog(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);
	
	ui.progressBar->setStyle(QStyleFactory::create("windowsvista"));
	ui.downloadPath->setText(CApplication::downloadPath());
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	ui.tableWidget->setColumnWidth(0, 20);

	ui.loadmore->hide();

	switchStatus(true);
	setupViewConnections();
	show();

	m_timer.setSingleShot(true);
	m_timer.start(2*60*1000); //20 sec

	//worker recycle
	m_timer.callOnTimeout([=]() {
		close();
		});
}

ResourceDialog::ResourceDialog(QJsonObject res, QWidget *parent)
	: ResourceDialog(parent)
{
	buildList(res);
}

ResourceDialog::~ResourceDialog()
{
	emit userClose();
}

void ResourceDialog::switchStatus(bool isLoading)
{
	if (isLoading) {
		setWindowTitle(QStringLiteral("AI解析中，请耐心等待..."));
		resize(700, 80);
		ui.loadingWidget->show();
		ui.mainWidget->hide();
	}
	else {
		setWindowTitle(QStringLiteral("资源选择器"));
		hide();
		resize(700, 580);
		ui.loadingWidget->hide();
		ui.mainWidget->show();
		m_timer.stop();
	}
	show();
	centerWidget();
}

void ResourceDialog::setTitle(const QString& title)
{
	setWindowTitle(title);
}

void ResourceDialog::centerWidget()
{
	QRect mainRect = qobject_cast<QWidget*>(parent())->geometry();
	QRect subRect = geometry();

	int x = mainRect.x() + (mainRect.width() - subRect.width()) / 2;
	int y = mainRect.y() + (mainRect.height() - subRect.height()) / 2;
	setGeometry(x, y, subRect.width(), subRect.height());
}

void ResourceDialog::buildList(const QJsonObject& res)
{
	m_timer.stop();
	QJsonArray items = res.value("videos").toArray();
	foreach (auto item, items)
	{
		ui.tableWidget->addRow(item["description"].toString(), item["resolution"].toString(), item.toObject());
	}
	
	bool hasmore = res.value("page").toObject()["hasmore"].toBool();
	if (!hasmore) {
		
	}
	
	ui.tableWidget->setInfinityScroll(false);
	switchStatus(false);
}

void ResourceDialog::setupViewConnections()
{
	connect(ui.confirmButton, &QPushButton::clicked, [=]() {
		QJsonObject res;
		res["downloads"] = ui.tableWidget->selectedRowData();
		QString folderName = ui.folderName->text();
		if (ui.folderName->isEnabled() && !folderName.isEmpty()) {
			res["folder"] = folderName;
		}
		emit download(res);
		accept();

		});

	connect(ui.dirButton, &QToolButton::clicked, [=]() {
		QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
		QString directory = QFileDialog::getExistingDirectory(0,
			QStringLiteral("选择下载文件夹"),
			"",
			options);
		if (!directory.isEmpty()) {
			ui.downloadPath->setText(directory);
			QSettings setting(CApplication::settingFile(), QSettings::IniFormat);
			setting.setValue("downloadPath", directory);
		}
		});

	connect(ui.selectAll, &QCheckBox::stateChanged, [=](int state) {
		if (state == 0) {
			ui.tableWidget->selectAllRows(false);
		}
		else if (state == 2) {
			ui.tableWidget->selectAllRows(true);
		}
		});

	connect(ui.loadmore, &QToolButton::clicked, [=]() {
		
		});
}

void ResourceDialog::setInfo(const QString & info, int count)
{
	ui.meta->setText(QStringLiteral("读取%1 共（%2）条").arg(info).arg(QString::number(count)));
	ui.folderName->setText(info);
}
