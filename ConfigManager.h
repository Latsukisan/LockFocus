#pragma once

#include <QObject>
#include <QSettings>
#include <QStringList>

class ConfigManager  : public QObject
{
	Q_OBJECT

public:
    ConfigManager(QObject* parent = nullptr,
        const QString& path = "self_control.ini");
    
	~ConfigManager();

    int getRemainingSkips() const;
    void setRemainingSkips(int count);
    QStringList getWhitelistApps();
    QStringList getWhitelistAppNames();
    void setWhitelistApps(const QStringList& apps, const QStringList& appNames);
    void save();

private:
    QScopedPointer<QSettings> settings;
    QStringList m_apps;
    QStringList m_appNames;
    int m_count;
};
