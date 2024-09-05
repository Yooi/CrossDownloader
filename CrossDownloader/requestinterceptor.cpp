#include "stdafx.h"
#include "requestinterceptor.h"

RequestInterceptor::RequestInterceptor(QObject *parent)
	: QWebEngineUrlRequestInterceptor(parent)
	, intercept(true)
{

}

RequestInterceptor::~RequestInterceptor()
{
}

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {

	QString requestUrl = info.requestUrl().toString(), referUrl = info.firstPartyUrl().toString();
    QString referrer, origin, ref;

	//处理图片文件的引用
	if (info.resourceType() != QWebEngineUrlRequestInfo::ResourceTypeMainFrame 
		&& (
			requestUrl.indexOf("qpic.cn") != -1 // wechat article
			|| requestUrl.indexOf("xiaohongshu.com") != -1
            || requestUrl.indexOf("__ref") != -1)
			|| info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeSubFrame) {

        if (requestUrl.indexOf("__ref") != -1) {
            QUrlQuery query(info.requestUrl().query());
            ref = query.queryItemValue("__ref", QUrl::FullyDecoded);
        }
		/*means request send from host*/
        if (ref.isEmpty()|| referUrl.indexOf("lnk5.com") !=-1) {
            referrer = origin = info.requestUrl().host().prepend(info.requestUrl().scheme().append("://"));
        }
        else {
            //e.g. http://www.google.com or www.google.com
            QUrl url(ref);
            if (url.isValid()) {
                referrer = ref;
				origin = url.host().prepend(info.requestUrl().scheme().append("://"));
            }
            else {
                //add schema
                referrer = origin = info.requestUrl().scheme().append("://") + ref;
            }
        }

		info.setHttpHeader(QByteArray("Referer"), referrer.toLatin1());
		info.setHttpHeader(QByteArray("Origin"), origin.toLatin1());
		info.setHttpHeader(QByteArray("Client"), QByteArray("1"));
		
		info.setHttpHeader(QByteArray("DNT"), QByteArray("1"));
		//info.setHttpHeader(QByteArray("sec-ch-ua"), QByteArray("\"Not.A/Brand\"; v = \"8\", \"Chromium\"; v = \"110\", \"Google Chrome\"; v = \"110\""));
		//info.setHttpHeader(QByteArray("sec-ch-ua-mobile"), QByteArray("?0"));
		//info.setHttpHeader(QByteArray("sec-ch-ua-platform"), QByteArray("Windows"));
		//info.setHttpHeader(QByteArray("Sec-Fetch-Dest"), QByteArray("empty"));
		//info.setHttpHeader(QByteArray("Sec-Fetch-Mode"), QByteArray("cors"));
		//info.setHttpHeader(QByteArray("Sec-Fetch-Site"), QByteArray("same-origin"));
		
	}

	
}