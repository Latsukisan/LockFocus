#pragma once

#include <QWidget>
#include "ui_LockWidget.h"
#include <windows.h>
#include "TomatoManager.h"
#include "ConfigManager.h"
#include <QTimer>
#include <QMessageBox>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class LockWidgetClass; };
QT_END_NAMESPACE

class LockWidget : public QWidget
{
	Q_OBJECT

public:
    explicit LockWidget(TomatoTask task, QWidget* parent = nullptr);
    void exit(bool force = true);
    ~LockWidget();

private slots:
    void pauseButtonClicked();
    void skipButtonClicked();
    void checkActiveWindow();
    void startWorkPhase();
    void startBreakPhase();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::LockWidgetClass* ui;
    TomatoTask m_task;
    ConfigManager m_config;
    QStringList m_whiteList = m_config.getWhitelistApps();
    QStringList m_whiteListNames = m_config.getWhitelistAppNames();
    qint64 m_remainingSkipTime=m_config.getRemainingSkips();
    QTimer* m_checkTimer = new QTimer(this);
    QTimer* m_phaseTimer = new QTimer(this);
    QTimer* m_updateTimer = new QTimer(this);
    bool m_isPaused = false;
    int m_cyclesRemaining;
    bool m_isWorking = true;
    bool m_isManualExit=false;
    qint64 m_remainingTime = 0;
    

    QString getProcessPath(DWORD processId);
    void openProcess(QString path);
    void updateTimeDisplay();
    void normalizePath(QString& path);
};
