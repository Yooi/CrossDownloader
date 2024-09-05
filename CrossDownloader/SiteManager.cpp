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
		{QStringLiteral("֪ʶ"), {"https://www.douyin.com/channel/300203", QStringLiteral("֪ʶ")}},
		{QStringLiteral("��Ϸ"), {"https://www.douyin.com/channel/300205", QStringLiteral("��Ϸ")}},
		{QStringLiteral("����"), {"https://www.douyin.com/channel/300201", QStringLiteral("����")}},
		{QStringLiteral("����Ԫ"), {"https://www.douyin.com/channel/300206", QStringLiteral("����Ԫ")}},
		{QStringLiteral("����"), {"https://www.douyin.com/channel/300209", QStringLiteral("����")}},
		{QStringLiteral("��ʳ"), {"https://www.douyin.com/channel/300204", QStringLiteral("��ʳ")}},
		{QStringLiteral("����"), {"https://www.douyin.com/channel/300207", QStringLiteral("����")}},
		{QStringLiteral("ʱ��"), {"https://www.douyin.com/channel/300208", QStringLiteral("ʱ��")}},
		{QStringLiteral("��Ӱ"), {"https://www.douyin.com/search/%E7%94%B5%E5%BD%B1", QStringLiteral("��Ӱ")}},
		{QStringLiteral("���Ӿ�"), {"https://www.douyin.com/search/%E7%94%B5%E8%A7%86%E5%89%A7", QStringLiteral("���Ӿ�")}},

		// ���Ը�����Ҫ��Ӹ���Ķ���
		};
	}
	else if (site == "xiaohongshu") {
		douyinActionMap = {
		{QStringLiteral("����"), {"https://www.xiaohongshu.com/search_result?keyword=%25E6%2597%2585%25E8%25A1%258C&source=web_explore_feed", QStringLiteral("����")}},
		{QStringLiteral("��Ӱ�Ƽ�"), {"https://www.xiaohongshu.com/search_result?keyword=%25E7%2594%25B5%25E5%25BD%25B1%25E6%258E%25A8%25E8%258D%2590&source=web_search_result_notes", QStringLiteral("��Ӱ�Ƽ�")}},
		{QStringLiteral("��ױ�̳�"), {"https://www.xiaohongshu.com/search_result?keyword=%25E7%25BE%258E%25E5%25A6%2586%25E6%2595%2599%25E7%25A8%258B&source=web_explore_feed", QStringLiteral("��ױ�̳�")}},
		{QStringLiteral("�����¼"), {"https://www.xiaohongshu.com/search_result?keyword=%25E6%2583%2585%25E6%2584%259F%25E8%25AF%25AD%25E5%25BD%2595&source=web_explore_feed", QStringLiteral("�����¼")}},
		{QStringLiteral("����ʳ��"), {"https://www.xiaohongshu.com/search_result?keyword=%25E5%2581%25A5%25E8%25BA%25AB%25E9%25A3%259F%25E8%25B0%25B1&source=web_explore_feed", QStringLiteral("����ʳ��")}},
		};
	}
	return douyinActionMap;
}