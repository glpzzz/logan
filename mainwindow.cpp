#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QRegularExpression>
#include <QStandardItemModel>
#include <QMessageBox>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect the dataChanged signal to a slot
    QObject::connect(&(this->headers), &QAbstractItemModel::dataChanged, [&]() {
        this->model.setHorizontalHeaderLabels(this->headers.stringList());
    });

    this->ui->lvColumnsNames->setModel(&(this->headers));
    this->ui->tvOutput->setModel(&(this->model));

    QMetaObject::connectSlotsByName(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectFile()
{
    this->selectedFile = QFileDialog::getOpenFileName(this, "Select File");
}

void MainWindow::processFile()
{
    this->model.clear();
    this->model.setHorizontalHeaderLabels(this->headers.stringList());

    QString regexString = this->ui->pteRegExp->toPlainText();
    QRegularExpression regex(regexString);

    QFile file(this->selectedFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open the file:" << file.errorString();
        return;
    }

    QTextStream in(&file);

    QStringList extraLines;
    while (!in.atEnd()) {
        QString line = in.readLine();

        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            QList<QStandardItem *> rowItems;
            for (int i = 1; i <= match.lastCapturedIndex(); ++i) {
                QStandardItem *item = new QStandardItem(match.captured(i));
                if (i == match.lastCapturedIndex()){
                    item->setText(item->text() + "\n" + extraLines.join("\n"));
                    extraLines.clear();
                }
                rowItems << item;
            }
            this->model.appendRow(rowItems);
        } else { //add to StringList of lines between records
            extraLines.append(line);
        }
    }

    file.close();
}

void MainWindow::updateHeadersCount()
{
    QString regexString = this->ui->pteRegExp->toPlainText();
    QRegularExpression regex(regexString);

    QStringList headerLabels;
    for (int i = 1; i <= regex.captureCount(); ++i) {
        headerLabels << QString("Column ") + QString::number(i);
    }

    this->headers.setStringList(headerLabels);
}

void MainWindow::on_actionAbout_Logan_triggered()
{
    QMessageBox::about(this, "About Logan",
                       "Version: 1.0\n"
                       "Author: Gabriel A. López López <glpz@daxslab.com>\n"
                       "Description: Simple log parser/viewer/analyzer.");
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

