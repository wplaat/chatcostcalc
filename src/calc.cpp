/**
 *  @file
 *  @brief The file contain Calc methods
 *  @author wplaat
 *
 *  Copyright (C) 2008-2010 PlaatSoft
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "calc.h"
#include "ui_calc.h"
#include "general.h"

#include "QtGui"

// -----------------------------------------
// Constructor & Destructor
// -----------------------------------------

Calc::Calc(QWidget *parent) : QWidget(parent), ui(new Ui::Calc)
{
    ui->setupUi(this);

    // Set Windows Title
    char tmp[100];
    sprintf(tmp,"%s v%s", APPL_NAME, APPL_VERSION);
    setWindowTitle(tmp);

    // Set fix windows form size.
    setMinimumSize(HORZ_SIZE,VERT_SIZE);
    setMaximumSize(HORZ_SIZE,VERT_SIZE);

    // Read registry setting data
    readSettings();

    start=false;
    pause=false;
    counter=0;

    sprintf(tmp,"%0.2f",0.0);
    ui->lcdNumber->display(QString(tmp));

    // Add background screen
    scene = new QGraphicsScene();
    scene->addPixmap(QPixmap(":/images/logo.png"));
    ui->graphicsView->setScene(scene);

    // Init statemachine
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(statemachine()));
    timer->start(990);
}

Calc::~Calc()
{
    delete ui;
}

// -----------------------------------------
// Other
// -----------------------------------------

void Calc::statemachine() {
   if (start) {
     counter++;
     int total=0;
     bool ok;

     // Get all hour values
     for (int i=0;i<ui->spinBox->value();i++) {

        if (ui->tableWidget->item(i, 0)->checkState()) {
            total+=ui->tableWidget->item(i,2)->text().toInt(&ok,10);
            qDebug() << "total" << total;
        }
     }
     sum+= total * (1/3600.0);

     qDebug() << "total" << total << "counter" << counter << "sum" << sum;

     char tmp[10];
     sprintf(tmp,"%0.2f",sum);
     ui->lcdNumber->display(QString(tmp));
     ui->timeEdit->setTime(time->addSecs(counter));

     QSound::play("snd/cash1.wav");
   }
}

/**
 * Read settings out Windows registry
 */
void Calc::readSettings()
{
    // Fetch previous window position
    QSettings settings("PlaatSoft", APPL_NAME);

    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    move(pos);

    int value = settings.value("amount", "0").toInt();
    ui->spinBox->setValue(value);
    on_spinBox_valueChanged(value);

    qDebug() << "Load settings";
}

/**
 * Write settings to Windows registry
 */
void Calc::writeSettings()
{
    bool ok;

    // Store current window position
    QSettings settings("PlaatSoft", APPL_NAME);

    // Get all hour values
    for (int i=0; i<ui->spinBox->value(); i++) {

       QString key = QString("name%1").arg(i+1);
       settings.setValue(key, ui->tableWidget->item(i,1)->text());

       key = QString("cost%1").arg(i+1);
       settings.setValue(key, ui->tableWidget->item(i,2)->text().toInt(&ok,10));
    }

    settings.setValue("amount", ui->spinBox->value());
    settings.setValue("pos", pos());

    qDebug() << "Write settings";
}

// -----------------------------------------
// User Actions
// -----------------------------------------

/**
 * Start & Hold button
 */
void Calc::on_pushButton_clicked()
{
   if (start) {
      start=false;
      ui->pushButton->setText("Start");

   } else {
      start=true;
      counter=0;
      sum=0;
      time=new QTime(0,0,0,0);

      ui->pushButton->setText("Stop");

      char tmp[10];
      sprintf(tmp,"%0.2f",0.0);
      ui->timeEdit->setTime(time->addSecs(counter));

      ui->lcdNumber->display(QString(tmp));
   }
}

/**
 * Pause & continue button
 */
void Calc::on_holdButton_clicked()
{
   if (pause) {
       ui->holdButton->setText("Continue");
      pause=false;
   } else {
      ui->holdButton->setText("Pause");
      pause=true;
   }
}

void Calc::on_spinBox_valueChanged(int value)
{
   // Set table dimensions
   ui->tableWidget->setColumnCount(3);
   ui->tableWidget->setRowCount(value);

    QTableWidgetItem *item;

   // Build Header
   ui->tableWidget->setColumnWidth(0,20);
   item = new QTableWidgetItem("");
   ui->tableWidget->setHorizontalHeaderItem(0, item);
   ui->tableWidget->setColumnWidth(1,170);
   item = new QTableWidgetItem("Member Name");
   ui->tableWidget->setHorizontalHeaderItem(1, item);
   ui->tableWidget->setColumnWidth(2,90);
   item = new QTableWidgetItem("Member Cost");
   ui->tableWidget->setHorizontalHeaderItem(2, item);

   QSettings settings("PlaatSoft", APPL_NAME);

   // Build Content
   for (int i=0; i<value; i++)
   {
      item = new QTableWidgetItem();
      item->setCheckState(Qt::Checked);
      ui->tableWidget->setItem(i, 0, item);

      QString key = QString("name%1").arg(i+1);
      item = new QTableWidgetItem(settings.value(key, "").toString());
      ui->tableWidget->setItem(i, 1, item);

      key = QString("cost%1").arg(i+1);
      item = new QTableWidgetItem(settings.value(key, "").toString());
      ui->tableWidget->setItem(i, 2, item);
   }

   ui->dial->setValue(value);
}

void Calc::on_dial_valueChanged(int value)
{
    ui->spinBox->setValue(value);
    on_spinBox_valueChanged(value);
}

/**
 * Close Window
 */
void Calc::closeEvent(QCloseEvent *event)
{
   // Store current window position
   writeSettings();
}

// -----------------------------------------
// The End
// -----------------------------------------

