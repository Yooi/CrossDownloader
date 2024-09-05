#include "stdafx.h"
#include "Setting.h"
#include "CApplication.h"
#include <QHostAddress> 

Setting::Setting(QWidget *parent)
	:QDialog(parent)
{
	m_pSettings = new QSettings(CApplication::settingFile(), QSettings::IniFormat);

	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);

	connect(ui.dirButton, &QToolButton::clicked, [=]() {
		QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
		QString directory = QFileDialog::getExistingDirectory(0,
			QStringLiteral("选择下载文件夹"),
			"",
			options);
		if (!directory.isEmpty()) {
			ui.downloadPath->setText(directory);
		}
	});

	connect(ui.confirmButton, &QPushButton::clicked, [=]() {
		save();
		close();
		emit confirm();
	});
	setModal(true);
	load();
	show();
}

void Setting::paintEvent(QPaintEvent *event) {
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void Setting::save()
{
	//verify download path
	QString path = ui.downloadPath->text();
	QDir dir(path);
	if (dir.exists()) {
		m_pSettings->setValue("downloadPath", path);
	}

	m_pSettings->setValue("openAIkey", ui.openAPIkey->text());

	//verify ip
	QString ipaddr = ui.proxyEdit->text();
	if (isValidIPAddress(ipaddr)) {
		m_pSettings->setValue("proxy", ui.proxyEdit->text());
	}

	QString keywords = ui.actionEdit->text();
	m_pSettings->setValue("keywords", keywords);

	m_pSettings->sync();
}

void Setting::load()
{
	ui.downloadPath->setText(qobject_cast<CApplication*>(qApp)->downloadPath());

	for (QString key : m_pSettings->childKeys())
	{
		QString val = m_pSettings->value(key).toString();
		//read accounts into db
		if(key == "openAIkey")
		{
			ui.openAPIkey->setText(val);
		}
		else if (key == "proxy") {
			ui.proxyEdit->setText(val);
		}
		else if (key == "keywords") {
			ui.actionEdit->setText(val);
		}
	}
}

bool Setting::isValidIPAddress(const QString &ipAddress) {
	QHostAddress address(ipAddress);
	return !address.isNull();
}

