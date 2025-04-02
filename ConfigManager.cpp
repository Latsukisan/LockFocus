#include "ConfigManager.h"
#include <QSettings>
#include <QStringList>

ConfigManager::ConfigManager(QObject* parent, const QString& path)
	: QObject(parent),
	settings(new QSettings(path, QSettings::IniFormat))
{
	if (!settings->contains("Monthly/RemainingSkips")||!settings->contains("Whitelist/Apps")||!settings->contains("Whitelist/AppNames"))
	{
		setRemainingSkips(10);          // 设置默认跳过次数
		setWhitelistApps(QStringList(), QStringList()); // 初始白名单为空
		save();                        // 显式保存
	}
	m_count = settings->value("Monthly/RemainingSkips", 10).toInt();
	m_apps = settings->value("Whitelist/Apps").toStringList();
	m_appNames = settings->value("Whitelist/AppNames").toStringList();
}

ConfigManager::~ConfigManager()
{
}

int ConfigManager::getRemainingSkips() const
{
	return m_count;
}

void ConfigManager::setRemainingSkips(int count)
{
	m_count = count;
	save();
}

QStringList ConfigManager::getWhitelistApps()
{
	return m_apps;
}

QStringList ConfigManager::getWhitelistAppNames()
{
	return m_appNames;
}

void ConfigManager::setWhitelistApps(const QStringList& apps, const QStringList& appNames)
{

	m_apps = apps;
	m_appNames = appNames;
	save();
}

void ConfigManager::save()
{
	settings->setValue("Monthly/RemainingSkips", m_count);
	settings->setValue("Whitelist/Apps", m_apps);
	settings->setValue("Whitelist/AppNames", m_appNames);
	settings->sync();
}
