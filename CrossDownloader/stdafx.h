#pragma once
#include <QtWidgets>
#include <QObject>
#include <QJsonObject>
#include "Utils.h"
#include "WebObject.h"

#define CD_DEBUG false

#define VERSION "0.4.6"

#define AGENT "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36"

#define CHECKUPDATE_URL "http://appstatic.lnk5.com/cdr/update.json"

#define PROTAL "https://cdr.lnk5.com/?ref=app"

#define SUPPORT_SITES { "xiaohongshu.com", "douyin.com" }

#define TASK_TIMEOUT 30*60 //mins
#define WORKER_TIMEOUT 1000*TASK_TIMEOUT  //30 mins timeout
#define PORT 20009
#define PAGE_SIZE 100

#define COOKIE_SEPAROR "^|$"

#define CLIENTCLS L"Qt5QWindowIcon"

#pragma comment(lib, "zlib.lib")

