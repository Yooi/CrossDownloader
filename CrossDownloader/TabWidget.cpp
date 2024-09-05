#include "stdafx.h"
#include "TabWidget.h"
#include <QRandomGenerator>

CommentWidget::CommentWidget(DBManager* db, QWidget *parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);

	m_db = db;
	
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
	ui.tableWidget->setColumnWidth(0, 200);
	ui.tableWidget->setColumnWidth(1, 300);
	//ui.tableWidget->setColumnWidth(4, 100);
	ui.tableWidget->setInfinityScroll(false);

	ui.analyzeButton->hide();
	//set default key
	setProperty("key", QRandomGenerator::global()->generate());
	setupConnections();
}

CommentWidget::~CommentWidget()
{
	qDebug() << "CommentWidget::~CommentWidget" << key();
	//m_db->removeComments(QString::number(key()));
}

void CommentWidget::setupConnections()
{
	connect(ui.downloadButton, &QRadioButton::clicked, [=]() {
		ui.tableWidget->clearContents();
		ui.tableWidget->setRowCount(0);
		QString inputString = ui.link->text();
		if (!Utils::siteVerfiy(inputString)) {
			QMessageBox::warning(0, QStringLiteral("CrossDownloader HINT"), QStringLiteral("输入链接有误，目前支持抖音，请重新输入"));
			return;
		}
		QString val = ui.comboBox->currentText();
		int total = val.toInt();
		if (val == QStringLiteral("全部")) {
			total = 0;
		}

		emit commentUrl(inputString, total);
		});

	connect(ui.exportButton, &QRadioButton::clicked, [=]() {
		QList<QStringList> data;
		foreach(auto comment, commentData["comments"].toArray()) {
			data << (QStringList() 
				<< comment["site"].toString()
				<< comment["nickname"].toString() 
				<< comment["signature"].toString().replace("\n", "")
				<< comment["avatar"].toString()
				<< comment["text"].toString()
				<< comment["create_time"].toString()
				<< QString::number(comment["like"].toInt())
				<< QString::number(comment["reply_total"].toInt())
				<< comment["ip_label"].toString()
				<< comment["sec_uid"].toString()
				<< comment["short_id"].toString()
				<< comment["pageurl"].toString());
		}
		Utils::exportToCsv(data);
		});
}

//key is parentid
void CommentWidget::setKey(int key)
{
	setProperty("key", key);
}

int CommentWidget::addComments(const QJsonObject & results)
{
	int parentid = 0;
	commentData = results;
	foreach(auto item, results["comments"].toArray()) {
		//int id = m_db->addComment(item.toObject());
		//qDebug() << "addResourceID" << item.toObject();
		QJsonObject itemObj = item.toObject();
		//download item ID
		itemObj["id"] = 0;
		table()->addComment(itemObj);

		parentid = itemObj["parent_id"].toInt();
	}

	setKey(parentid);
	return parentid;
}

void CommentWidget::setInfo(int parentid)
{
	if (parentid == 0) {
		ui.subject->setText(QStringLiteral("---"));
		return;
	}
	QJsonObject videoItem = m_db->getResource(QString::number(parentid));
	QString subject = QString("<a href=\"%1\">%2</a>").arg(videoItem["pageurl"].toString()).arg(videoItem["subject"].toString());
	ui.subject->setText(subject);
	ui.link->setText(videoItem["pageurl"].toString());
}
