#pragma once
#include "stdafx.h"

enum class TabType {
	NORMAL,
	COMMENT,
	ANALYZE,
	LIVE,
};

enum class APIType {
	NONE,
	DETAIL,
	USER, // USER DATA
	SEARCH,
	COLLECTION,
	CHENNAL,
	COMMENT,
	LIVE,
	LOGIN,
	INFO, // USER INFO
	OTHER
};

enum class WorkerAction {
	CLOSE,
	RELOAD,
	CREATE,
	SHOW,
	HIDE,
};

struct Api {
	QString site;
	QString landing;
	QString name;
	APIType type;
	QJsonObject params;
};