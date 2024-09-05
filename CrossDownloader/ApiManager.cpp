#include "stdafx.h"
#include "ApiManager.h"


ApiManager::ApiManager(DBManager* db, QWidget* parent)
	: m_viewmanager(std::make_unique<ViewManager>())
	, m_accountmanager(std::make_unique<AccountManager>(db))
	, m_isClose(false)
{
	m_parent = parent;
	m_db = db;
	parepareWorkers();
}

QString ApiManager::siteName(const QString & url)
{
	if (url.indexOf("douyin") != -1) {
		return "douyin";
	}
	else if (url.indexOf("xiaohongshu") != -1) {
		return "xiaohongshu";
	}
	return QString();
}

// rowinfo - query info
Api ApiManager::generator(const QString & site, APIType type, const QJsonObject & rowinfo, QJsonObject params_)
{
	Api api;
	api.landing = m_landing.value(site);
	api.site = site;
	api.type = type;
	api.name = typeToName(type);

	QJsonObject params = params_;
	if (site == "douyin") {
		switch (type)
		{
		case APIType::NONE:
			break;
		case APIType::DETAIL:
			params["aweme_id"] = rowinfo["vid"].toString();
			break;
		case APIType::USER:
			params["sec_uid"] = rowinfo["sec_uid"].toString();
			break;
		case APIType::SEARCH:
			params["keyword"] = rowinfo["keyword"].toString();
			break;
		case APIType::COLLECTION:
			params["mix_id"] = rowinfo["mix_id"].toString();
			break;
		case APIType::COMMENT:
			params["aweme_id"] = rowinfo["vid"].toString();
			break;
		default:
			break;
		}
	}
	else if (site == "xiaohongshu") {
		switch (type)
		{
		case APIType::NONE:
			break;
		case APIType::DETAIL:
			params["source_note_id"] = rowinfo["vid"].toString();
			break;
		case APIType::USER:
			params["user_id"] = rowinfo["real_uid"].toString();
			params["curser"] = rowinfo["curser"].toString();
			break;
		case APIType::SEARCH:
			params["keyword"] = rowinfo["keyword"].toString();
			break;
		case APIType::COMMENT:
			params["note_id"] = rowinfo["vid"].toString();
			break;
		case APIType::INFO:
			break;
		case APIType::OTHER:
			break;
		default:
			break;
		}
	}
	api.params = params;
	return api;
}

Api ApiManager::apiParser(const QString & site, const QString & url, APIType type, QJsonObject param_)
{
	Api api;
	api.landing = m_landing.value(site);
	api.site = site;
	//type is None, which means you need to determine the apitype
	api.type = type;

	QJsonObject params = param_;
	if (site == "douyin") {
		// /video/7319652192758091045
		if (url.indexOf(".douyin.com/video/") != -1) {
			params["aweme_id"] = QUrl(url).fileName();
			//sometimes it will be COMMENT
			if (type == APIType::NONE) {
				api.type = APIType::DETAIL;
			}
			api.name = typeToName(api.type);
		}
		else if(url.indexOf(".douyin.com/user/")!=-1) {	
			QString sec_uid = QUrl(url).fileName();
			params["sec_uid"] = sec_uid;
			if (sec_uid.count() > 40) {
				api.type = APIType::USER;
				api.name = typeToName(api.type);
			}
		}
		else if (url.indexOf(".douyin.com/channel/") != -1) {
			params["channel_id"] = QUrl(url).fileName();
			api.type = APIType::CHENNAL;
			api.name = typeToName(api.type);

		}else if (url.indexOf(".douyin.com/hot") != -1) {

			api.name = "hostspot";
		}
		else if (url.indexOf(".douyin.com/search") != -1) {
			params["keyword"] = QUrl(url).fileName();
			api.type = APIType::SEARCH;
			api.name = typeToName(api.type);
		}
		else if (url.indexOf(".douyin.com/collection") != -1) {
			QStringList pathSegments = url.split('/', Qt::SkipEmptyParts);
			if (pathSegments.size() >= 3) {
				params["mix_id"] = pathSegments.at(3);
				api.type = APIType::COLLECTION;
				api.name = typeToName(api.type);
			}

		}
	}
	else if (site == "xiaohongshu") {
		if (url.indexOf("xiaohongshu.com/search_result") != -1) {
			QUrl url_(url);
			QUrlQuery query(url_);
			params["keyword"] = query.queryItemValue("keyword", QUrl::FullyDecoded);
			api.type = APIType::SEARCH;
			api.name = typeToName(api.type);
		}
		else if (url.indexOf("xiaohongshu.com/explore/") != -1 || url.indexOf("xiaohongshu.com/discovery/item/") != -1) {
			params["note_id"] = QUrl(url).fileName();
			//sometimes it will be COMMENT
			if (type == APIType::NONE) {
				api.type = APIType::DETAIL;
			}
			api.name = typeToName(api.type);
		}
		else if (url.indexOf("xiaohongshu.com/user/profile/") != -1) {
			params["user_id"] = QUrl(url).fileName();
			api.type = APIType::USER;
			api.name = typeToName(api.type);
		}
	}
	api.params = params;
	return api;
}

QString ApiManager::typeToName(APIType type)
{
	QString apiName = "";
	switch (type)
	{
	case APIType::NONE:
		break;
	case APIType::DETAIL:
		apiName = "videoDetail";
		break;
	case APIType::USER:
		apiName = "userVideos";
		break;
	case APIType::SEARCH:
		apiName = "searchVideos";
		break;
	case APIType::COLLECTION:
		apiName = "collection";
		break;
	case APIType::CHENNAL:
		apiName = "channel";
		break;
	case APIType::COMMENT:
		apiName = "videoComment";
		break;
	case APIType::LOGIN:
		apiName = "login";
		break;
	case APIType::INFO:
		apiName = "userInfo";
	case APIType::OTHER:
		break;
	default:
		break;
	}
	return apiName;
}

void ApiManager::parepareWorkers()
{	
	//foreach (auto site, m_landing.keys())
	//{
	const QString site = "douyin";
		m_viewmanager->permanentWorker(m_accountmanager->loadCookieJar(site), [=](WebView* view, QString scriptid) {
			qDebug() << "ApiManager::parepareWorkers" << site;
			view->setApiManager(this);
			QJsonObject ops;
			ops["onCreated"] = false;
			//add site script here
			view->addScript(site, CApplication::siteScript(site), ops);
			view->setUrl(m_landing.value(site));
			//for debug
			//view->show();

			connect(view, &WebView::loadFinished, [=]() {
				// run script here
				if (errorHandler(view, site)) {
					return;
				}
				view->setValid(1);
				emit info("Ready");
				//add Cookies
				m_db->addAccount(site, QJsonObject(), view->getCookieString());
				qDebug() << "Site:" << site << "Load Finished!!";
				});

		}, site, m_parent);
	//}

}

// site is uid
void ApiManager::login(const QString & uid, bool show, QWidget* parent)
{
	login(uid, []() {}, show, parent);
}

void ApiManager::workerOperation(WorkerAction act, QString workerId)
{
	auto view = m_viewmanager->getWorker(workerId);
	if (!view) {
		return;
	}
	switch (act)
	{
	case WorkerAction::SHOW:
		view->show();
		break;
	case WorkerAction::CLOSE:
		view->close();
		break;
	case WorkerAction::RELOAD:
		view->reload();
		break;
	case WorkerAction::CREATE:
		//Unused
		break;
	default:
		break;
	}
}

bool ApiManager::errorHandler(WebView * view, const QString site)
{
	QString title = view->title();
	if (site.startsWith("douyin") && title.contains(QStringLiteral("验证码"))) {
		view->setValid(-1);
		emit error(QStringLiteral("初始化失败！由于抖音接口验证码，导致初始化中断，请点击【Worker】检查并完成验证后重试！"));
		return true;
	}
	return false;
}

void ApiManager::workerClose(const QString & uid)
{
	//INFO: because worker cannot remove from main thread
	//so have to set the approach in sub thread
	QTimer::singleShot(100, [=]() {
		workerOperation(WorkerAction::CLOSE, uid);
		});
	
}

void ApiManager::showWorkerView(const QString workerId)
{
	workerOperation(WorkerAction::SHOW, workerId);
}

QString ApiManager::covertWidToUrl(const QString workerId)
{
	// sometimes workerid will be site, sometimes it will be account id
	// query sitename by accountId
	return m_landing.value(workerId);
}
