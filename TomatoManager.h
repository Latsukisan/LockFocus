#pragma once
#ifndef TOMATOMANAGER_H
#define TOMATOMANAGER_H
#include <QObject>
#include <QVector>
#include <QTime>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QList>

struct TomatoTask {
    QString name="番茄";          // 番茄钟名称
    QTime startTime=QTime(0, 0, 0);   // 开始时间（ISO8601格式）
    QTime endTime=QTime(0, 50, 0);     // 结束时间
    int workDuration = 2400; // 默认40分钟（秒）
    int breakDuration = 600; // 默认10分钟
    int cycles = 1;          // 执行次数
    bool isForward = false;   // 正向计时
    QList<int> workDayofWeek;     // 工作日（1-7对应周一到周日，空为非定时任务）

    
    bool operator==(const TomatoTask& other) const 
    {
        return name == other.name &&
            startTime == other.startTime &&
            endTime == other.endTime;
    }
    
    operator QStringList() const
    {
        QStringList days;
        for (int day : workDayofWeek) {
            days << QString::number(day);
        }
        return QStringList() << name 
                            << days.join(",")
                            << startTime.toString("HH:mm:ss")
                            << endTime.toString("HH:mm:ss")
                            << QString::number(workDuration/60)
                            << QString::number(breakDuration/60);
    }
};



class TomatoManager : public QObject {
    Q_OBJECT
public:
    explicit TomatoManager(QObject* parent = nullptr,
        const QString& configPath = "tomato_config.json");
    ~TomatoManager();
    // 核心操作方法
    bool loadManager();
    bool saveManager() const;
    void insertTask(const TomatoTask& task, int index = -1);
    void removeTask(int index = -1);
    void updateTask(int index, const TomatoTask& task);
    int getTaskCount() const;

    // 访问方法
    const QVector<TomatoTask>& tasks() const { return m_tasks; }
    TomatoTask getTask(int index = -1) const;

signals:
    void configReloaded();

private:
    QVector<TomatoTask> m_tasks;
    QString m_configPath="tomato_config.json";

    // JSON转换方法
    TomatoTask parseTask(const QJsonObject& obj) const;
    QJsonObject serializeTask(const TomatoTask& task) const;
    void createDefaultManager() const;
};
#endif