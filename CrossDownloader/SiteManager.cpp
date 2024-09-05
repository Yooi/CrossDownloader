#include "stdafx.h"
#include "SiteManager.h"

SiteManager::SiteManager()
{

}

// actions
// actionTag, actionUrl, actionName
QMap<QString, QStringList> SiteManager::actions(const QString site)
{
	QMap<QString, QStringList> douyinActionMap;
	if (site == "douyin") {
		douyinActionMap = {
		{QStringLiteral("知识"), {"https://www.douyin.com/channel/300203", QStringLiteral("知识")}},
		{QStringLiteral("游戏"), {"https://www.douyin.com/channel/300205", QStringLiteral("游戏")}},
		{QStringLiteral("娱乐"), {"https://www.douyin.com/channel/300201", QStringLiteral("娱乐")}},
		{QStringLiteral("二次元"), {"https://www.douyin.com/channel/300206", QStringLiteral("二次元")}},
		{QStringLiteral("音乐"), {"https://www.douyin.com/channel/300209", QStringLiteral("音乐")}},
		{QStringLiteral("美食"), {"https://www.douyin.com/channel/300204", QStringLiteral("美食")}},
		{QStringLiteral("体育"), {"https://www.douyin.com/channel/300207", QStringLiteral("体育")}},
		{QStringLiteral("时尚"), {"https://www.douyin.com/channel/300208", QStringLiteral("时尚")}},
		{QStringLiteral("电影"), {"https://www.douyin.com/search/%E7%94%B5%E5%BD%B1", QStringLiteral("电影")}},
		{QStringLiteral("电视剧"), {"https://www.douyin.com/search/%E7%94%B5%E8%A7%86%E5%89%A7", QStringLiteral("电视剧")}},

		// 可以根据需要添加更多的动作
		};
	}
	else if (site == "xiaohongshu") {
		douyinActionMap = {
		{QStringLiteral("旅行"), {"https://www.xiaohongshu.com/search_result?keyword=%25E6%2597%2585%25E8%25A1%258C&source=web_explore_feed", QStringLiteral("旅行")}},
		{QStringLiteral("电影推荐"), {"https://www.xiaohongshu.com/search_result?keyword=%25E7%2594%25B5%25E5%25BD%25B1%25E6%258E%25A8%25E8%258D%2590&source=web_search_result_notes", QStringLiteral("电影推荐")}},
		{QStringLiteral("美妆教程"), {"https://www.xiaohongshu.com/search_result?keyword=%25E7%25BE%258E%25E5%25A6%2586%25E6%2595%2599%25E7%25A8%258B&source=web_explore_feed", QStringLiteral("美妆教程")}},
		{QStringLiteral("情感语录"), {"https://www.xiaohongshu.com/search_result?keyword=%25E6%2583%2585%25E6%2584%259F%25E8%25AF%25AD%25E5%25BD%2595&source=web_explore_feed", QStringLiteral("情感语录")}},
		{QStringLiteral("健身食谱"), {"https://www.xiaohongshu.com/search_result?keyword=%25E5%2581%25A5%25E8%25BA%25AB%25E9%25A3%259F%25E8%25B0%25B1&source=web_explore_feed", QStringLiteral("健身食谱")}},
		};
	}
	return douyinActionMap;
}