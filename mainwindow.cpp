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

    QString regexString = this->ui->pteRegExp->toPlainText();
    QRegularExpression regex(regexString);

    QFile file(this->selectedFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open the file:" << file.errorString();
        return;
    }

    // Create a QTextStream to read the file
    QTextStream in(&file);

    QStringList headerLabels;
    QRegularExpressionMatch match = regex.match("");
    for (int i = 1; i <= match.lastCapturedIndex(); ++i) {
        headerLabels << match.captured(i);
    }
    this->model.setHorizontalHeaderLabels(headerLabels);

    // Process each line in the file
    QStringList extraLines;
    while (!in.atEnd()) {
        QString line = in.readLine();

        // Use the regular expression to check for matches
        QRegularExpressionMatch match = regex.match(line);

        // Process the match
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

    this->ui->tvOutput->setModel(&(this->model));
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

