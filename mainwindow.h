#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QRegularExpression>
#include <QLabel>

#include "parser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void selectFile();
    void processFile();
    void selectParserByIndex(int index);
    void on_actionAbout_Logan_triggered();
    void on_actionAbout_Qt_triggered();

signals:
    void fileChanged(const QString &filePath);
    void entriesLoaded(const int &entriesNumber);

private:
    Ui::MainWindow *ui;
    QString selectedFile;
    QList<Parser*> parsers;
    Parser* currentParser;

    QRegularExpression regexp;

    QStringListModel modelParsers;
    QStandardItemModel modelEntries;
    QStringListModel modelHeaderLabels;

    //Status bar widgets
    QLabel *laFilePath;
    QLabel *laEntriesNumber;
};
#endif // MAINWINDOW_H
