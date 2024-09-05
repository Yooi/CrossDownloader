#include "stdafx.h"
#include "ResourceTableWidget.h"
#include <QSqlRecord>
#include <QSqlField> 
#include <QtConcurrent>

ResourceTableWidget::ResourceTableWidget(QWidget *parent)
	:QTableWidget(parent)
	, m_menu(new QMenu(this))
	, m_page(1)
	, m_db(nullptr)
	, m_menuType(Menu::None)
	, m_infinityScroll(true)
{
	//CustomizeTableHeader* myHeader = new CustomizeTableHeader(Qt::Horizontal, this);
	//setHorizontalHeader(myHeader);
	//remove click border of the cell
	setItemDelegate(new NoFocusDelegate());
	//setColumnWidth(0, 30);
	horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	//TMP: get file icon for all, temp approach here
	m_videoIcon = Utils::getLocalFileIcon("mp4");

	//connect(myHeader, &CustomizeTableHeader::toggleSelectAll, this, &ResourceTableWidget::selectAllRows);
	connect(this, &QTableView::customContextMenuRequested, [=](QPoint pos) {
		if (m_menuType == Menu::None) {
			return;
		}
		QModelIndex index = indexAt(pos);
		m_menu->clear();
		
		if (index.isValid()) {
			QTableWidgetItem *item = this->itemAt(pos);
			buildMenu(item->row());
			m_menu->exec(QCursor::pos());
		}
		});

	connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &ResourceTableWidget::handleScroll);

}

ResourceTableWidget::~ResourceTableWidget()
{
}

void ResourceTableWidget::selectAllRows(bool checked)
{
	for (int row = 0; row < rowCount(); ++row)
	{
		QTableWidgetItem *checkBoxItem = item(row, 0);
		if (checkBoxItem)
			checkBoxItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
	}
}

void ResourceTableWidget::buildMenu(int row)
{
	int id = item(row, 0)->data(Qt::UserRole).toInt();
	qDebug() << "ResourceTableWidget::buildMenu" << row << id;

	QJsonObject rowInfo = getRowInfo(row);
	QStringList tags = Utils::extractHashtags(item(row, 0)->text());
	if (rowInfo["status"].toInt() == 2) {
		m_menu->addAction(QIcon(":/CrossDownloader/Resources/video.png"), QStringLiteral("播放"), [=]() {
			QDesktopServices::openUrl(QUrl::fromLocalFile(rowInfo["filepath"].toString()));
			});

		m_menu->addAction(QIcon(":/CrossDownloader/Resources/folder-open.png"), QStringLiteral("打开文件夹"), [=]() {
			QStringList args;
			args << "/select," << QDir::toNativeSeparators(rowInfo["filepath"].toString());

			QProcess::startDetached("explorer", args);
			});
	}
	else {
		m_menu->addAction(QIcon(":/CrossDownloader/Resources/download.png"), QStringLiteral("开始"), [=]() {
			emit start(id);
			});
	}
	//qDebug() << tags << item(row, 0)->text();

	m_menu->addAction(QIcon(":/CrossDownloader/Resources/delete.png"), QStringLiteral("删除"), [=]() {
		//because if item is downloading, it will crash the app if remove the row
		removeRow(row);
		m_db->removeResource(QString::number(id));
		emit remove(id);
		});
	m_menu->addSeparator();
	if (!rowInfo["nickname"].toString().isEmpty()) {
		m_menu->addAction(QIcon(":/CrossDownloader/Resources/download.png"), QStringLiteral("发布者 %1").arg(rowInfo["nickname"].toString()), [=]() {
			emit openResource(id, APIType::USER, rowInfo["nickname"].toString());
			});
		m_menu->addAction(QIcon(":/CrossDownloader/Resources/download.png"), QStringLiteral("下载评论"), [=]() {
			emit openResource(id, APIType::COMMENT);
			});
	}
	m_menu->addAction(QIcon(":/CrossDownloader/Resources/link.png"), QStringLiteral("访问原页面"), [=]() {
		QDesktopServices::openUrl(QUrl(rowInfo["pageurl"].toString()));
		});
	if (tags.length() > 0 && !rowInfo["vid"].toString().isEmpty()) {
		m_menu->addSeparator();
		foreach(auto tag, tags) {
			m_menu->addAction(QIcon(":/CrossDownloader/Resources/search.png"), QStringLiteral("搜索 #%1").arg(tag), [=]() {
				qDebug() << "Menu Action search" << tag;
				emit openResource(id, APIType::SEARCH, tag);
				});
		}
	}

}

void ResourceTableWidget::addRow(const QString &name, const QString &value, const QJsonObject& data)
{
	int row = rowCount();
	insertRow(row);
	QString type = data["type"].toString();

	CustomTableWidgetItem *checkBoxItem = new CustomTableWidgetItem("", true);
	setItem(row, 0, checkBoxItem);
	checkBoxItem->setData(Qt::UserRole, data);

	QIcon icon = m_videoIcon;
	if (type != "mp4") {
		icon = Utils::getLocalFileIcon("jpg");
	}
	setItem(row, 1, new QTableWidgetItem(icon, name));
	setItem(row, 2, new QTableWidgetItem(value));

}

/*
the reason for return QTableWidgetItem*
because the pos of items will be changed, so you can use QTableWidgetItem*
pointer the determine the absoluate pos of the item in the table
*/
QTableWidgetItem* ResourceTableWidget::prepand(const QJsonObject& data)
{
	insertRow(0);
	QTableWidgetItem* item = new QTableWidgetItem(getStatusIcon(data["status"].toInt(), data["type"].toString()), data["subject"].toString());
	item->setData(Qt::UserRole, data["id"].toInt());

	setItem(0, 0, item);
	setItem(0, 1, new CustomTableWidgetItem(data["status"].toInt() == 2 ? "100%" : "0%"));
	setItem(0, 2, new QTableWidgetItem(Utils::humanReadableSize(data.value("size").toVariant().toLongLong())));
	setItem(0, 3, new QTableWidgetItem(data["resolution"].toString()));

	item = new QTableWidgetItem;
	item->setData(Qt::DisplayRole, data["comment_count"].toInt());
	setItem(0, 4, item);
	item = new QTableWidgetItem;
	item->setData(Qt::DisplayRole, data["like_count"].toInt());
	setItem(0, 5, item);
	item = new QTableWidgetItem;
	item->setData(Qt::DisplayRole, data["share_count"].toInt());
	setItem(0, 6, item);

	setItem(0, 7, new QTableWidgetItem(data["create_time"].toString()));

	m_idMap.insert(data["id"].toInt(), item);

	return item;
}


QTableWidgetItem* ResourceTableWidget::append(const QSqlRecord& record)
{
	if (record.isEmpty()) {
		return nullptr;
	}
	int row = rowCount();
	insertRow(row);
	int status = record.field("status").value().toInt();
	status = status == 0 ? 3 : status;
	QTableWidgetItem* item = new QTableWidgetItem(getStatusIcon(status, record.field("type").value().toString()), record.field("subject").value().toString());
	item->setData(Qt::UserRole, record.field("id").value().toInt());
	setItem(row, 0, item);
	setItem(row, 1, new CustomTableWidgetItem(record.field("status").value().toInt() == 2 ? "100%" : "0%"));

	setItem(row, 2, new QTableWidgetItem(Utils::humanReadableSize(record.field("size").value().toLongLong())));
	setItem(row, 3, new QTableWidgetItem(record.field("resolution").value().toString()));

	item = new QTableWidgetItem;
	item->setData(Qt::DisplayRole, record.field("comment_count").value().toInt());
	setItem(row, 4, item);
	item = new QTableWidgetItem;
	item->setData(Qt::DisplayRole, record.field("like_count").value().toInt());
	setItem(row, 5, item);
	item = new QTableWidgetItem;
	item->setData(Qt::DisplayRole, record.field("share_count").value().toInt());
	setItem(row, 6, item);

	setItem(row, 7, new QTableWidgetItem(record.field("create_time").value().toString()));

	//qDebug() << "insert map" << record.field("id").value().toInt() << row;
	m_idMap.insert(record.field("id").value().toInt(), item);

	return item;
}

QTableWidgetItem * ResourceTableWidget::addComment(const QJsonObject & data)
{
	int row = rowCount();
	insertRow(row);

	QTableWidgetItem* item = new QTableWidgetItem(QIcon(":/CrossDownloader/Resources/comment.png"), data["nickname"].toString());
	item->setData(Qt::UserRole, data["id"].toInt());
	setItem(row, 0, item);
	setItem(row, 1, new QTableWidgetItem(data["text"].toString()));

	item = new QTableWidgetItem;
	item->setData(Qt::DisplayRole, data["like"].toInt());
	setItem(row, 2, item);

	setItem(row, 3, new QTableWidgetItem(data["ip_label"].toString()));
	setItem(row, 4, new QTableWidgetItem(data["create_time"].toString()));
	return item;
}

void ResourceTableWidget::updateItemStatus(int id, int status)
{
	QTableWidgetItem* tableitem = item(getRow(id), 0);
	tableitem->setIcon(getStatusIcon(status, "mp4"));
}

QIcon ResourceTableWidget::getStatusIcon(int status, QString type)
{
	switch (status)
	{
	case 0:
		return QIcon(":/CrossDownloader/Resources/wait.png");
	case 1:
		return QIcon(":/CrossDownloader/Resources/downloading.png");
	case 2:
		return Utils::getLocalFileIcon(type);
	case 3:
		return QIcon(":/CrossDownloader/Resources/stop.png");
	case -1:
		return QIcon(":/CrossDownloader/Resources/fail.png");
	default:
		return QIcon(":/CrossDownloader/Resources/video.png");
	}
}

void ResourceTableWidget::updateView()
{
	viewport()->update();
}

QJsonArray ResourceTableWidget::selectedRowData()
{
	QJsonArray list;
	for (int row = 0; row < rowCount(); ++row)
	{
		QTableWidgetItem *checkBoxItem = item(row, 0);
		if (checkBoxItem && checkBoxItem->checkState() == Qt::Checked) {
			list.append(checkBoxItem->data(Qt::UserRole).toJsonObject());
		}
			
	}
	return list;
}

void ResourceTableWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::RightButton) {
		// Determine the item at the mouse position
		QTableWidgetItem *item = itemAt(event->pos());

		// If an item is clicked, select the entire row
		if (item) {
			setCurrentItem(item);
			selectRow(item->row());
		}
	}

	QTableWidget::mousePressEvent(event);
}

void ResourceTableWidget::loadResourceData()
{
	if (!m_db->isValid()) {
		return;
	}
	QFuture<void> future = QtConcurrent::run([=]() {
		SqlTableModel model(NULL, m_db->currentDatabase());

		model.setTable("resource");
		//ID is important, otherwise cannot do operations of this row
		//m_model->query("SELECT id, uid, username, islogin, last_open FROM accounts");
		//m_model->setSqlQuery("SELECT id, subject, description, url, cover, resolution, create_time, nickname, avatar, sec_uid, comment_count, like_count, share_count FROM resource ORDER BY id DESC ");
		QString filter = QString("1=1 ORDER BY id DESC LIMIT %1,%2").arg((m_page - 1)*PAGE_SIZE).arg(PAGE_SIZE);
		model.setFilter(filter);
		model.select();
		qDebug() << "ResourceTableWidget::loadResourceData" << model.rowCount();
		
		for (int row = 0; row < model.rowCount(); ++row) {
			append(model.record(row));
		}
		});

	future.waitForFinished();
}

void ResourceTableWidget::handleScroll(int value) {
	// Check if the user has scrolled to the bottom
	if (value == verticalScrollBar()->maximum() && m_infinityScroll) {
		// Load more data
		m_page++;
		loadResourceData();
	}
}

int ResourceTableWidget::getRow(int id)
{
	QTableWidgetItem* item = m_idMap.value(id);
	if (item) {
		return item->row();
	}
	return -1;
}

QJsonObject ResourceTableWidget::getRowInfo(int row)
{
	QTableWidgetItem* table_item = item(row, 0);
	int id = table_item->data(Qt::UserRole).toInt();
	
	return m_db->getResource(QString::number(id));
}
