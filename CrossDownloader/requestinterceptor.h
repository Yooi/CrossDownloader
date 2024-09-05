#ifndef REQUESTINTERCEPTOR_H
#define REQUESTINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>

class RequestInterceptor : public QWebEngineUrlRequestInterceptor
{
	Q_OBJECT

public:
	RequestInterceptor(QObject *parent = nullptr);
	~RequestInterceptor();
	void interceptRequest(QWebEngineUrlRequestInfo &info);
	bool intercept;

signals:
    void imageInfo(const QJsonObject&);
	
};

#endif // REQUESTINTERCEPTOR_H
