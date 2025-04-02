#pragma once

#include <QDialog>
#include "ui_TomatoSet.h"
#include "TomatoManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TomatoSetClass; };
QT_END_NAMESPACE

class TomatoSet : public QDialog
{
	Q_OBJECT

public:
	TomatoSet(QWidget *parent = nullptr);
	TomatoTask getTask(TomatoTask task);
	TomatoTask editTask(TomatoTask task);
	void judgeTask();
	~TomatoSet();

private:
	Ui::TomatoSetClass *ui;
	TomatoTask *m_task = new TomatoTask();
	TomatoTask task;
};
