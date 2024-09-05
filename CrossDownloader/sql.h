#pragma once

const QString accounts_sql_ = 
"CREATE TABLE IF NOT EXISTS accounts (\
id INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT, \
uid VARCHAR(30) NOT NULL UNIQUE,\
sitenick VARCHAR(30), \
priority int(11), \
username VARCHAR(30), \
avatar TEXT,\
remark TEXT, \
signature TEXT, \
current BOOL, \
active BOOL, \
islogin BOOL, \
enable BOOL, \
lock   BOOL, \
follower_count VARCHAR(30), \
groupname VARCHAR(50), \
catelog VARCHAR(30), \
tags VARCHAR(200), \
type VARCHAR(30), \
source TEXT, \
cookie TEXT, \
sync BOOL, \
last_update datetime NOT NULL, \
create_dt datetime NOT NULL, \
status int(11) NOT NULL,\
model int(11) NOT NULL, \
params TEXT, \
last_schedule_dt datetime, \
schedule_dt datetime, \
account VARCHAR(30), \
password VARCHAR(30) \
)";

const QString resource_sql_ = 
"CREATE TABLE IF NOT EXISTS resource (\
id INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT, \
ownerid VARCHAR(30),\
uid VARCHAR(30) ,\
site VARCHAR(30) ,\
priority int(11), \
filepath TEXT, \
name VARCHAR(200), \
size VARCHAR(50), \
url TEXT, \
vid VARCHAR(20), \
\
pageurl TEXT, \
pagetitle VARCHAR(200), \
pagecookie TEXT, \
file_created datetime, \
lastModified datetime, \
description TEXT, \
\
collect_count int, \
comment_count int, \
like_count int, \
share_count int, \
\
subject TEXT, \
cover TEXT, \
content TEXT, \
duration VARCHAR(20), \
resolution VARCHAR(20), \
ratio VARCHAR(20), \
create_time datetime, \
\
nickname VARCHAR(200), \
avatar TEXT, \
signature TEXT, \
sec_uid VARCHAR(200), \
real_uid VARCHAR(100), \
short_id VARCHAR(100), \
\
type VARCHAR(20), \
tags VARCHAR(200), \
catelog VARCHAR(60), \
timestamp VARCHAR(30), \
schedule datetime, \
active BOOL, \
object TEXT, \
sync BOOL, \
cookie TEXT, \
model int(11) NOT NULL, \
success BOOL, \
status int(11) NOT NULL,\
create_dt datetime NOT NULL \
)";

const QString user_sql_ = 
"CREATE TABLE IF NOT EXISTS user (\
id INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT, \
uid VARCHAR(30) ,\
phone VARCHAR(30) ,\
type VARCHAR(30), \
token VARCHAR(30), \
last_login datetime, \
success BOOL, \
object TEXT, \
status int(11) NOT NULL,\
create_dt datetime NOT NULL \
)";

const QString site_sql_ = 
"CREATE TABLE IF NOT EXISTS site (\
id INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT, \
sitename VARCHAR(30), \
nickname VARCHAR(30), \
sitelogo VARCHAR(100), \
description TEXT, \
loginUrl TEXT ,\
verifyScript TEXT ,\
statsScript TEXT ,\
injectOptions TEXT, \
object TEXT ,\
type VARCHAR(30), \
success BOOL, \
status int(11) NOT NULL,\
create_dt datetime NOT NULL \
)";

const QString comment_sql_ =
"CREATE TABLE IF NOT EXISTS comments (\
id INTEGER UNIQUE PRIMARY KEY AUTOINCREMENT, \
site VARCHAR(30) ,\
pageurl TEXT, \
\
nickname VARCHAR(200), \
signature TEXT, \
avatar TEXT, \
sec_uid VARCHAR(200), \
real_uid VARCHAR(100), \
short_id VARCHAR(100), \
\
text TEXT, \
create_time datetime, \
cid VARCHAR(50), \
parent_id VARCHAR(50), \
ip_label VARCHAR(20), \
reply_total int, \
like int, \
\
object TEXT, \
status int(11) NOT NULL,\
create_dt datetime NOT NULL \
)";