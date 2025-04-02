#pragma once

#include <QDialog>
#include "ui_WhiteApp.h"
#include "ConfigManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WhiteAppClass; };
QT_END_NAMESPACE

class WhiteApp : public QDialog
{
	Q_OBJECT

public:
	WhiteApp(QWidget *parent = nullptr);
	~WhiteApp();

private:
	Ui::WhiteAppClass *ui;
	ConfigManager* m_config = new ConfigManager();
	QString appName;
	QString appPath;
	QStringList m_appNames;
	QStringList m_appPaths;
private slots:
	void addApp();
	void removeApp();
	void saveApp();
};
