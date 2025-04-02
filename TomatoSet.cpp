#include "TomatoSet.h"
#include <qmessagebox.h>

TomatoSet::TomatoSet(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::TomatoSetClass())
{
	ui->setupUi(this);
	connect(ui->YesButton, &QPushButton::clicked, this, &TomatoSet::judgeTask);
	ui->nameEdit->setText("番茄");
}

TomatoTask TomatoSet::getTask(TomatoTask m_task)
{	
	m_task.workDayofWeek.clear();
	task = m_task;
	task.name = ui->nameEdit->text();
	task.isForward = ui->isForwardCheck->isChecked();
	task.startTime = ui->timeEdit->time();
	task.workDuration=ui->tomatospinBox->value()*60;
	task.breakDuration=ui->restspinBox->value()*60;
	task.cycles=ui->performspinBox->value();
	if(ui->radioButton->isChecked()) task.workDayofWeek.append(1);
	if(ui->radioButton_2->isChecked()) task.workDayofWeek.append(2);
	if(ui->radioButton_3->isChecked()) task.workDayofWeek.append(3);
	if(ui->radioButton_4->isChecked()) task.workDayofWeek.append(4);
	if(ui->radioButton_5->isChecked()) task.workDayofWeek.append(5);
	if(ui->radioButton_6->isChecked()) task.workDayofWeek.append(6);
	if(ui->radioButton_7->isChecked()) task.workDayofWeek.append(7);
	task.endTime=task.startTime.addSecs((task.cycles)*(task.workDuration+task.breakDuration));
	return task;
}

TomatoTask TomatoSet::editTask(TomatoTask m_task)
{
	ui->nameEdit->setText(m_task.name);
	ui->isForwardCheck->setChecked(m_task.isForward);
	ui->timeEdit->setTime(m_task.startTime);
	ui->tomatospinBox->setValue(m_task.workDuration / 60);
	ui->restspinBox->setValue(m_task.breakDuration / 60);
	ui->performspinBox->setValue(m_task.cycles);

	ui->radioButton->setChecked(false);
	ui->radioButton_2->setChecked(false);
	ui->radioButton_3->setChecked(false);
	ui->radioButton_4->setChecked(false);
	ui->radioButton_5->setChecked(false);
	ui->radioButton_6->setChecked(false);
	ui->radioButton_7->setChecked(false);

	for (int day : m_task.workDayofWeek)
	{
		switch (day)
		{
		case 1: ui->radioButton->setChecked(true); break;
		case 2: ui->radioButton_2->setChecked(true); break;
		case 3: ui->radioButton_3->setChecked(true); break;
		case 4: ui->radioButton_4->setChecked(true); break;
		case 5: ui->radioButton_5->setChecked(true); break;
		case 6: ui->radioButton_6->setChecked(true); break;
		case 7: ui->radioButton_7->setChecked(true); break;
		}
	}

	return getTask(m_task);
}

void TomatoSet::judgeTask()
{
	if(ui->tomatospinBox->value() <= 0 || ui->restspinBox->value() <= 0 || ui->performspinBox->value() <= 0)
	{
		QMessageBox::warning(this, "错误", "有填写的数值不合法");
	}
	else
	{
		accept();
	}

}

TomatoSet::~TomatoSet()
{
	delete ui;
}
