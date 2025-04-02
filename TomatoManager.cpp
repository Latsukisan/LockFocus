#include "TomatoManager.h"

TomatoManager::TomatoManager(QObject* parent, const QString& configPath)
    : QObject(parent), m_configPath(configPath)
{
    if (!QFile::exists(m_configPath)) {
        createDefaultManager();
    }
    loadManager();
}

TomatoManager::~TomatoManager()
{
}

bool TomatoManager::loadManager() {
    QFile file(m_configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file:" << file.errorString();
        return false;
    }

    QByteArray fileData = file.readAll();
    if(fileData.isEmpty()) {
        qWarning() << "Json file is empty, creating default Json";
        createDefaultManager();
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return false;
    }

    m_tasks.clear();
    QJsonArray array = doc["tomatoes"].toArray();
    if(array.isEmpty()) {
        qWarning() << "No tasks found in Json, creating default";
        createDefaultManager();
    }

    for (const auto& value : array) {
        m_tasks.append(parseTask(value.toObject()));
    }
    emit configReloaded();
    return true;
}


bool TomatoManager::saveManager() const {
    QJsonArray array;
    for (const auto& task : m_tasks) {
        array.append(serializeTask(task));
    }

    QJsonDocument doc;
    doc.setObject({ { "tomatoes", array } });

    QFile file(m_configPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save config:" << file.errorString();
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

TomatoTask TomatoManager::parseTask(const QJsonObject& obj) const {
    return {
        .name = obj["name"].toString(),
        .startTime = QTime::fromString(obj["startTime"].toString(), "HH:mm:ss"),
        .endTime = QTime::fromString(obj["endTime"].toString(), "HH:mm:ss"),
        .workDuration = obj["workDuration"].toInt(2400),
        .breakDuration = obj["breakDuration"].toInt(600),
        .cycles = obj["cycles"].toInt(1),
        .isForward = obj["isForward"].toBool(true),
        .workDayofWeek = [&]() 
        {
            QList<int> list;
            for (auto v : obj["workDayofWeek"].toArray())
                list.append(v.toString().toInt());
            return list;
        }()
    };
}

QJsonObject TomatoManager::serializeTask(const TomatoTask& task) const {
    // 创建一个QStringList来存储转换后的字符串
    QStringList workDayOfWeekStrings;
    for (int day : task.workDayofWeek) {
        workDayOfWeekStrings.append(QString::number(day));
    }
    
    return {
        { "name", task.name },
        { "startTime", task.startTime.toString("HH:mm:ss") },
        { "endTime", task.endTime.toString("HH:mm:ss") },
        { "workDuration", task.workDuration },
        { "breakDuration", task.breakDuration },
        { "cycles", task.cycles },
        { "isForward", task.isForward },
        // 使用转换后的QStringList
        { "workDayofWeek", QJsonArray::fromStringList(workDayOfWeekStrings) },
    };
}


void TomatoManager::createDefaultManager() const {
    QJsonArray array;
    array.append(QJsonObject{
        {"name", "示例任务"},
        {"startTime", QTime::currentTime().toString("HH:mm:ss")},
        {"endTime", QTime::currentTime().addSecs(2400).toString("HH:mm:ss")},
        {"workDuration", 2400},
        {"breakDuration", 600},
        {"cycles", 1},
        {"isForward", false},
        {"workDayofWeek", QJsonArray()},
        });

    QJsonObject rootObj;  // 显式创建根对象
    rootObj["tomatoes"] = array; 

    QFile file(m_configPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(rootObj);  // 明确调用QJsonObject构造函数
        file.write(doc.toJson());
    }
}
// 操作方法实现
void TomatoManager::insertTask(const TomatoTask& task, int index) {
    if (index == -1) {
        index = m_tasks.size();  // 插入到末尾
    }
    if (index < 0 || index > m_tasks.size()) {
        index = m_tasks.size();  // 越界修正
    }
    m_tasks.insert(index, task);
    saveManager();
}

void TomatoManager::removeTask(int index) {
    if (index == -1 && !m_tasks.isEmpty()) {
        index = m_tasks.size() - 1;  // 删除最后一个元素
    }
    if (index < 0 || index >= m_tasks.size()) {
        return;
    }
    m_tasks.removeAt(index);
    saveManager();
}

void TomatoManager::updateTask(int index, const TomatoTask& task) {
    if (index >= 0 && index < m_tasks.size()) {
        m_tasks[index] = task;
        saveManager();
    }
}

int TomatoManager::getTaskCount() const
{
    return m_tasks.size();
}

TomatoTask TomatoManager::getTask(int index) const {
    if (index == -1 && !m_tasks.isEmpty()) {
        index = m_tasks.size() - 1;  // 获取最后一个元素
    }
    TomatoTask task;
    task=m_tasks.at(index);
    return (index >= 0 && index < m_tasks.size()) ? m_tasks.at(index) : TomatoTask{};
}
