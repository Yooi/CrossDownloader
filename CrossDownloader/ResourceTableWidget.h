#pragma once
#include <qtablewidget.h>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPainter>
#include <QSqlRecord>
#include <QSqlDatabase>
#include "ApiManager.h"
#include "Dbmanager.h"
//extern enum class APIType;

class NoFocusDelegate : public QStyledItemDelegate
{
public:
	NoFocusDelegate(QObject *parent = nullptr):QStyledItemDelegate(parent){}
	void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex &index) const
	{
		QStyleOptionViewItem itemOption(option);
		if (itemOption.state & QStyle::State_HasFocus)
		{
			itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
		}
		QStyledItemDelegate::paint(painter, itemOption, index);
	}
};

class CustomTableWidgetItem : public QTableWidgetItem
{
public:
	CustomTableWidgetItem(const QString &text) : QTableWidgetItem(text)
	{
		setTextAlignment(Qt::AlignCenter);
	}

	CustomTableWidgetItem(const QIcon& icon, const QString &text) : QTableWidgetItem(icon, text)
	{
		setTextAlignment(Qt::AlignCenter);
	}

	CustomTableWidgetItem(const QString &text, bool checkbox) : QTableWidgetItem(text)
	{
		if (checkbox) {
			setFlags(flags() | Qt::ItemIsUserCheckable);
			setCheckState(Qt::Checked);
		}
	}

};

class CustomizeTableHeader : public QHeaderView
{
	Q_OBJECT
public:
	CustomizeTableHeader(Qt::Orientation orientation, QWidget * parent = 0) : QHeaderView(orientation, parent)
	{
		isOn = false;
		setSectionResizeMode(QHeaderView::ResizeToContents);
	}

	enum STATUS {
		STOP,
		WAIT,
		PROCESSING,
		SUCCESS,
		FAIL,
	};

signals:
	void toggleSelectAll(bool checked);

protected:
	void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
	{
		painter->save();
		QHeaderView::paintSection(painter, rect, logicalIndex);
		painter->restore();
		if (logicalIndex == 0)
		{
			QStyleOptionButton option;
			option.rect = QRect(18, 18, 18, 18);
			if (isOn)
				option.state = QStyle::State_On;
			else
				option.state = QStyle::State_Off;
			this->style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
		}
	}

	void mousePressEvent(QMouseEvent *event)
	{
		if (visualIndexAt(event->pos().x()) == 0)
		{
			isOn = !isOn;
			emit toggleSelectAll(isOn);
			this->update();
		}
		QHeaderView::mousePressEvent(event);
	}
private:
	bool isOn;
};

class ResourceTableWidget :	public QTableWidget
{
	Q_OBJECT
public:
	enum class Menu{
		None,
		UserType,
	};

	explicit ResourceTableWidget(QWidget *parent = nullptr);
	~ResourceTableWidget();

	void setMenuType(Menu type) { m_menuType = type; };
	void setInfinityScroll(bool status) { m_infinityScroll = status; };

	void setDB(DBManager* db) { m_db = db; }

	void buildMenu(int row);

	void addRow(const QString & name, const QString & value, const QJsonObject& data);

	QTableWidgetItem* prepand(const QJsonObject & data);

	QTableWidgetItem* append(const QSqlRecord& record);

	QTableWidgetItem* addComment(const QJsonObject & data);

	void updateItemStatus(int id, int status);

	QIcon getStatusIcon(int status, QString type = 0);

	void updateView();

	QJsonArray selectedRowData();

	void selectAll(bool selected);

	void loadResourceData();

	int getRow(int id);

	QJsonObject getRowInfo(int row);

signals:
	void start(int id);
	void remove(int id);
	void openResource(int id, APIType type, QString keyword=0);

public slots:
	void selectAllRows(bool checked);

protected:
	void mousePressEvent(QMouseEvent *event) override;

	
private slots:

	void handleScroll(int value);

private:
	QIcon m_videoIcon;
	QMenu* m_menu;
	DBManager* m_db;
	Menu m_menuType;
	int m_page;
	bool m_infinityScroll;
	//id -- row
	QMap<int, QTableWidgetItem*> m_idMap;
};

class MetaCard : public QWidget
{
	Q_OBJECT
public:
	explicit MetaCard(QWidget *parent = nullptr);
	~MetaCard();
};
