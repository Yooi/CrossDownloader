#pragma once
#include "ui_setting.h"

class Setting : public QDialog
{
	Q_OBJECT

signals:
	void closed();
	void confirm();

public:
	Setting(QWidget *parent = nullptr);
	~Setting() { emit closed(); };

protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void save();

	void load();

	bool isValidIPAddress(const QString & ipAddress);

private:
	Ui::SettingWindow ui;
	QSettings* m_pSettings;
};

