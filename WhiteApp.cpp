#include "WhiteApp.h"
#include "qlistwidget.h"
#include <qmessagebox.h>
#include <qfiledialog.h>
WhiteApp::WhiteApp(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::WhiteAppClass())
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	connect(ui->addButton, &QPushButton::clicked, this, &WhiteApp::addApp);
	connect(ui->delButton, &QPushButton::clicked, this, &WhiteApp::removeApp);
	connect(ui->saveButton, &QPushButton::clicked, this, &WhiteApp::saveApp);

	m_appNames = m_config->getWhitelistAppNames();
	m_appPaths = m_config->getWhitelistApps();
	for (int i = 0; i < m_appNames.size(); ++i)
	{
		QString programName = m_appNames.at(i);
		QString programPath = m_appPaths.at(i);
		QListWidgetItem *item = new QListWidgetItem;
		QString displayText = QString("%1 \t %2").arg(programName).arg(programPath);
		item->setText(displayText);

		// 添加至列表
		ui->appList->addItem(item);
	}

}

WhiteApp::~WhiteApp()
{
	delete ui;
}

void WhiteApp::addApp()
{
    appPath = QFileDialog::getOpenFileName(this, "选择应用程序", "", "应用程序 (*.exe);;所有文件 (*.*)");
    
    // 检查路径是否为空
    if(appPath.isEmpty()) return;  

    appName = QFileInfo(appPath).fileName();
    if(appName.isEmpty()) return;

    int currentRow = ui->appList->currentRow();
    
    // 创建列表项
    QListWidgetItem *item = new QListWidgetItem;
    QString displayText = QString("%1 \t %2").arg(appName).arg(appPath);
    item->setText(displayText);

    if(currentRow != -1) {
        // 插入到当前选中行上方
        ui->appList->insertItem(currentRow, item);
        m_appNames.insert(currentRow, appName);
        m_appPaths.insert(currentRow, appPath);
    } else {
        // 添加到列表末尾
        ui->appList->addItem(item);
        m_appNames.append(appName);
        m_appPaths.append(appPath);
    }
}

void WhiteApp::removeApp()
{
	int selectedRow = -1;
	if (ui->appList->currentRow() != -1)
		selectedRow = ui->appList->currentRow();
	else return;
	m_appNames.removeAt(selectedRow);
	m_appPaths.removeAt(selectedRow);
	QListWidgetItem* item = ui->appList->takeItem(selectedRow);
	delete item;
}

void WhiteApp::saveApp()
{
	m_config->setWhitelistApps(m_appPaths, m_appNames);
	m_config->save();
	QMessageBox::information(this, "提示", "保存成功！");
}
