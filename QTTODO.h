#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_QTTODO.h"
#include "TomatoManager.h"
#include "ConfigManager.h"
#include "TomatoSet.h"
#include "LockWidget.h"
#include "qmessagebox.h"
#include "WhiteApp.h"
#include "qdebug.h"
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class QTTODOClass; };
QT_END_NAMESPACE

class QTTODO : public QMainWindow
{
    Q_OBJECT

public:
    QTTODO(QWidget *parent = nullptr);
    ~QTTODO();
    void timingTomato();
    void instantTomato();
    void autoTomato();
    void whiteListSet();
    void beginTomato(TomatoTask task);
    void minCheck();
    void secCheck();
    void getTodayTasks();
    void delTask();
    void editTask();

private:
    Ui::QTTODOClass *ui;
    QTimer* minTimer;
    QTimer* secTimer;
    QDate currentDate;
    int currentDayOfWeek;
    QTime currentTime;
    TomatoManager *tomatoManager=new TomatoManager(this);
    ConfigManager *configManager=new ConfigManager(this);
    QVector<TomatoTask> m_tasks;
    QVector<TomatoTask> todayTasks;
    TomatoTask recentTask;
    bool ifTodayTaskFinished=false;
    QStringList tomatoList;
};