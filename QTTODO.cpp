#include "QTTODO.h"


QTTODO::QTTODO(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QTTODOClass())
{
    ui->setupUi(this);
    ui->tmtTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    minTimer = new QTimer(this);
    secTimer = new QTimer(this);
    minTimer->setInterval(5 * 60 * 1000);
    secTimer->setSingleShot(true);
    secTimer->setInterval(1000);
    connect(minTimer, &QTimer::timeout, this, &QTTODO::minCheck);
    connect(secTimer, &QTimer::timeout, this, &QTTODO::secCheck);
    connect(ui->timetmtButton, &QPushButton::clicked, this, &QTTODO::timingTomato);
    connect(ui->nowtmtButton, &QPushButton::clicked, this, &QTTODO::instantTomato);
    connect(ui->whiteappButton, &QPushButton::clicked, this, &QTTODO::whiteListSet);
    connect(ui->delButton, &QPushButton::clicked, this, &QTTODO::delTask);
    connect(ui->editButton, &QPushButton::clicked, this, &QTTODO::editTask);
    currentTime = QTime::currentTime();
    currentDate = QDate::currentDate();
    currentDayOfWeek = currentDate.dayOfWeek();
    ui->tmtTableWidget->setColumnCount(6);
    ui->tmtTableWidget->setHorizontalHeaderLabels({"名字","工作日期","开始时间","结束时间","单次番茄时间","单次休息时间"});
    m_tasks=tomatoManager->tasks();
    recentTask.startTime = QTime(23, 59, 59);
    getTodayTasks();
    int taskCount=tomatoManager->getTaskCount();
    for (int row = 0; row < taskCount; row++)
    {
        tomatoList = QStringList(tomatoManager->getTask(row));
        ui->tmtTableWidget->insertRow(row);
        for(int col = 0; col < tomatoList.size(); col++)
        {
            ui->tmtTableWidget->setItem(row, col, new QTableWidgetItem(tomatoList.at(col)));
        }
    }

    autoTomato();
}


QTTODO::~QTTODO()
{
    delete ui;
}


void QTTODO::timingTomato()
{
    TomatoTask task;
    TomatoSet* set = new TomatoSet(this);
    if (set->exec() == QDialog::Accepted) 
    {
        task = set->getTask(task);
        tomatoManager->insertTask(task);
        //插入到表格里
        int row = ui->tmtTableWidget->rowCount();
        ui->tmtTableWidget->insertRow(row);
        tomatoList = QStringList(task);
        for (int col = 0; col < tomatoList.size(); col++) 
        {
            ui->tmtTableWidget->setItem(row, col, new QTableWidgetItem(tomatoList.at(col)));
        }
        m_tasks.append(task);
        if(task.workDayofWeek.contains(currentDayOfWeek)&&task.startTime>currentTime) 
        todayTasks.append(task);
        if (!secTimer->isActive()) 
        autoTomato();
    }
}

void QTTODO::instantTomato()
{
    TomatoTask task;
    TomatoSet* set = new TomatoSet(this);
    if (set->exec() == QDialog::Accepted)
    {
        task = set->getTask(task);
        task.isForward = false;
        beginTomato(task);
    }
    
}

void QTTODO::autoTomato()
{
    currentTime = QTime::currentTime();
    for (TomatoTask task : todayTasks)
    {
        int curDiff = currentTime.secsTo(task.startTime);
        int recentDiff = currentTime.secsTo(recentTask.startTime);
        if (curDiff<recentDiff&&curDiff>0)
        {
            recentTask = task;
        }
    }
    if(currentTime.secsTo(recentTask.startTime)>0 && currentTime.secsTo(recentTask.startTime) < 300)
    minCheck();
    else
    minTimer->start();
}

void QTTODO::whiteListSet()
{
    WhiteApp *appSet= new WhiteApp(this);
    appSet->show();
}

void QTTODO::beginTomato(TomatoTask task)
{
    LockWidget* lock = new LockWidget(task);
    lock->show();
}

void QTTODO::minCheck()
{
    currentTime = QTime::currentTime();
    if (currentTime.secsTo(recentTask.startTime) < 300)
    {
        minTimer->stop();
        secTimer->setInterval(1000*currentTime.secsTo(recentTask.startTime));
        secTimer->start();
    }
}

void QTTODO::secCheck()
{
    todayTasks.removeAll(recentTask);
    if(todayTasks.isEmpty())
    ifTodayTaskFinished = true;
    beginTomato(recentTask);
    recentTask.endTime = QTime(23, 59, 59);
    if(!ifTodayTaskFinished)
    autoTomato();
}

void QTTODO::getTodayTasks()
{
    currentTime=QTime::currentTime();
    currentDate = QDate::currentDate();
    currentDayOfWeek = currentDate.dayOfWeek();
     //保证符合所有今天的任务，并且开始时间比当前时间晚的任务都被选进去，早的就不选进去
    for(const TomatoTask &task : m_tasks)
    {
        if (task.workDayofWeek.contains(currentDayOfWeek) && task.startTime > currentTime)
        {
            todayTasks.append(task);
        }
    }
}

void QTTODO::delTask()
{
    int row = ui->tmtTableWidget->currentRow();
    if (row >= 0)
    {
        tomatoManager->removeTask(row);
        ui->tmtTableWidget->removeRow(row);
    }
}

void QTTODO::editTask()
{
    // 获取当前选中的行
    int currentRow = ui->tmtTableWidget->currentRow();
    if (currentRow == -1) // 如果没有选中行
    {
        QMessageBox::warning(this, "警告", "请先选中要编辑的任务！");
        return;
    }
        
    
    // 获取当前行的任务
    TomatoTask oldTask = m_tasks.at(currentRow);
    
    // 创建编辑对话框并传入当前任务
    TomatoSet* set = new TomatoSet(this);

    TomatoTask newTask = set->editTask(oldTask);
    
    if(set->exec() == QDialog::Accepted)
    {
        // 获取编辑后的任务
        TomatoTask newTask = set->getTask(oldTask);
        
        // 更新任务管理器
        tomatoManager->updateTask(currentRow, newTask);
        
        // 更新表格显示
        tomatoList = QStringList(newTask);
        for(int col = 0; col < tomatoList.size(); col++)
        {
            // 先删除原有项再创建新项
            QTableWidgetItem* item = ui->tmtTableWidget->takeItem(currentRow, col);
            delete item;
            ui->tmtTableWidget->setItem(currentRow, col, new QTableWidgetItem(tomatoList.at(col)));
        }
        
        // 更新任务列表
        m_tasks.replace(currentRow, newTask);
        
        // 更新今日任务列表
        if(newTask.workDayofWeek.contains(currentDayOfWeek) && newTask.startTime > currentTime)
        {
            int index = todayTasks.indexOf(oldTask);
            if(index != -1)
                todayTasks.replace(index, newTask);
            else
                todayTasks.append(newTask);
        }
        else
        {
            todayTasks.removeOne(oldTask);
        }        
        // 如果计时器未激活，自动开始番茄钟
        if(!secTimer->isActive())
            autoTomato();
    }
    
    delete set;
}

