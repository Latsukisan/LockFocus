#include "LockWidget.h"
#include "qtimer.h"
#include <Windows.h>
#include "qdebug.h"
#include <QDir>
#include <QRegularExpression>
#include <QProcess>

LockWidget::LockWidget(TomatoTask task, QWidget* parent)
    : QWidget(parent), ui(new Ui::LockWidgetClass), m_task(task), m_isManualExit(false),
    m_cyclesRemaining(task.cycles)
{
    ui->setupUi(this);
    connect(ui->whiteexeButton, &QPushButton::clicked, this, [this]() { LockWidget::openProcess(m_whiteList[ui->appCbox->currentIndex()]); });
    connect(ui->pauseButton, &QPushButton::clicked, this, &LockWidget::pauseButtonClicked);
    connect(ui->skipButton, &QPushButton::clicked, this, &LockWidget::skipButtonClicked);
    connect(m_updateTimer, &QTimer::timeout, this, &LockWidget::updateTimeDisplay);
    connect(m_checkTimer, &QTimer::timeout, this, &LockWidget::checkActiveWindow);
    connect(m_phaseTimer, &QTimer::timeout, this, [this]() {
        if (m_isWorking) {
            startBreakPhase();
        }
        else {
            if (--m_cyclesRemaining <= 0) {
                close();
                return;
            }
            startWorkPhase();
        }
        });
    setWindowState(Qt::WindowFullScreen);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    m_remainingTime = m_config.getRemainingSkips();
    ui->appCbox->addItems(m_whiteListNames);
    m_updateTimer->setInterval(1000);
    startWorkPhase();

}


LockWidget::~LockWidget()
{
	delete ui;
}

void LockWidget::pauseButtonClicked()
{
    
    m_checkTimer->stop();
    hide();
    // 创建一个单次触发的定时器来暂停两分钟
    QTimer::singleShot(2 * 60 * 1000, this, [this]() 
    {
        show();
        m_checkTimer->setInterval(500);
        m_checkTimer->start();
    });
}


void LockWidget::skipButtonClicked()
{
    if(m_remainingSkipTime > 0)
    {
        m_remainingSkipTime--;
        m_config.setRemainingSkips(m_remainingSkipTime);
        exit(true);
    }
    else
    {
        qWarning() << "No remaining skips";
    }
    
}


void LockWidget::startWorkPhase()
{
    ui->label->setText("当前处于番茄状态，剩余时间:");//todo：后续闲可能会用图片或者更好看的标题呈现
    m_isWorking = true;
    m_checkTimer->start(500);
    show();

    // 初始化倒计时
    m_remainingTime = m_task.workDuration;
    m_updateTimer->start();
    updateTimeDisplay();

    // 启动工作阶段定时器
    m_phaseTimer->start(m_task.workDuration * 1000); 
}

void LockWidget::startBreakPhase()
{
    ui->label->setText("当前处于休息状态，剩余时间:");
    m_isWorking = false;
    m_checkTimer->stop();
    show();

    // 初始化倒计时
    m_remainingTime = m_task.breakDuration;
    m_updateTimer->start();
    updateTimeDisplay();

    // 启动休息阶段定时器
    m_phaseTimer->start(m_task.breakDuration * 1000);
}

void LockWidget::closeEvent(QCloseEvent* event) 
{
    if (m_isManualExit) 
    {
        m_isManualExit = false;  
        event->accept();
    }
    else 
    {
        event->ignore();
     }
}

void LockWidget::exit(bool force) 
{
    m_isManualExit = true;  
    this->close();          
}

//检查当前活动窗口是否在白名单中，若不在则置顶锁定窗口

void LockWidget::checkActiveWindow()
{
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow || foregroundWindow == (HWND)winId()) return;
    if (foregroundWindow == reinterpret_cast<HWND>(winId())) return;

    DWORD processId;
    GetWindowThreadProcessId(foregroundWindow, &processId);
    QString processPath = getProcessPath(processId);


    normalizePath(processPath);


    QString dirPath = QFileInfo(processPath).path();

 
    // 白名单校验（匹配整个目录）
    bool isAllowed = std::any_of(m_whiteList.begin(), m_whiteList.end(),
        [&](const QString& pattern) {
            // 获取白名单路径的目录部分
            QString whiteDir = QFileInfo(pattern).path();
            normalizePath(whiteDir);
            normalizePath(dirPath);

            //检查当前进程目录是否以白名单目录开头
            qDebug() << "Checking white list:" << pattern << "vs" << dirPath;
            qDebug() << "Dir path:" << dirPath << "starts with:" << whiteDir;
            qDebug() << "Dir path:" << dirPath.startsWith(whiteDir, Qt::CaseInsensitive);
            return dirPath.startsWith(whiteDir, Qt::CaseInsensitive);
        });



    if (!isAllowed) {
        /*SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow((HWND)winId());*/
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        show();
    }
    else 
    {
        hide();
    }
}

/**
 * 根据进程ID获取进程的可执行文件路径
 * @param processId 进程ID
 * @return 进程路径字符串
 */
QString LockWidget::getProcessPath(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!hProcess) return "";

    WCHAR buffer[MAX_PATH] = { 0 };
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(hProcess, 0, buffer, &size)) {
        CloseHandle(hProcess);
        return QString::fromWCharArray(buffer);
    }
    CloseHandle(hProcess);
    return "";
}

/**
 * 打开指定路径的进程
 * @param path 要打开的进程路径
 */
void LockWidget::openProcess(QString path)
{

    path = QDir::toNativeSeparators(path);
    
 
    if(!QFile::exists(path)) {
        qWarning() << "File does not exist:" << path;
        return;
    }
    

    QFileInfo fileInfo(path);
    QString workingDir = fileInfo.absolutePath();
    

    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(workingDir);
    process->start(path);
    
    
    
    if(!process->waitForStarted()) 
    {
        qWarning() << "Failed to start process:" << path 
                   << "Error:" << process->errorString();
        delete process;
    }
}


void LockWidget::updateTimeDisplay()
{

    if(m_remainingTime <= 0) {
        m_updateTimer->stop();
        return;
    }

    int minutes = m_remainingTime / 60;
    int seconds = m_remainingTime % 60;
    ui->timeLCD->setDigitCount(5);
    ui->timeLCD->display(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));

    
    m_remainingTime--;
}



void LockWidget::normalizePath(QString& path)
{
    path = QDir::toNativeSeparators(path).toLower();
    path.replace("\\systemroot\\", "\\windows\\");
    
    // 确保目录路径以斜杠结尾
    if (!path.endsWith('/') && !path.endsWith('\\')) {
        path += QDir::separator();
    }
}



