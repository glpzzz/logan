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


    // START initialize with default parsers

    Parser* pNoParser = new Parser("Don't parse", "^(.*)$", QStringList("Message"));
    this->parsers.append(pNoParser);


    QStringList yii2Headers;
    yii2Headers << "Date Time" << "IP" << "User ID" << "Request ID" << "Level" << "Category" << "Message";
    Parser* pYiiParser = new Parser(
        "Yii 2 Log",
        "^(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}) \\[(.*?)\\]\\[(.*?)\\]\\[(.*?)\\]\\[(info|error|warn)\\]\\[(.*?)\\](.*)$",
        yii2Headers);
    this->parsers.append(pYiiParser);

    // END initialize with default parsers

    this->ui->dwEntryDetails->setVisible(false);

    QStringList parsersNames;
    std::transform(this->parsers.begin(), this->parsers.end(), std::back_inserter(parsersNames),
                   [](Parser* parser) { return parser->getName(); });

    this->modelParsers.setStringList(parsersNames);
    this->ui->cbAvailableParsers->setModel(&(this->modelParsers));
    this->ui->tvOutput->setModel(&(this->modelEntries));

    // Connect the dataChanged signal to a slot
    QObject::connect(&(this->modelHeaderLabels), &QAbstractItemModel::dataChanged, [&]() {
        this->modelEntries.setHorizontalHeaderLabels(this->modelHeaderLabels.stringList());
    });

    QItemSelectionModel *selectionModel = this->ui->tvOutput->selectionModel();

    // Connect selectionChanged signal to a slot (or lambda function)
    QObject::connect(selectionModel,
                     &QItemSelectionModel::selectionChanged,
                     [&](const QItemSelection &selected, const QItemSelection &deselected) {

        QStringList headers = this->modelHeaderLabels.stringList();

        QStringList output;
        output << "<html><body>";
        for (const auto &index : selected.indexes()) {
            QString columnName = headers.at(index.column());
            QVariant cellValue = this->modelEntries.data(index);

            output << "<h2>" << columnName << "</h2>";
            output << "<pre>" << cellValue.toString() << "</pre>";
        }
        output << "</body></html>";
        this->ui->tbEntry->setHtml(output.join(""));
    });

    // Checks the action connected to the visibility of the entry details dock
    QObject::connect(this->ui->tvOutput, &QTableView::doubleClicked, [this]() {
        if (!this->ui->actionEntry_Details->isChecked()) {
            this->ui->actionEntry_Details->setChecked(true);
        }
    });

    // Setup status bar widgets
    this->laFilePath = new QLabel(this);
    this->ui->statusbar->addPermanentWidget(this->laFilePath, 1);
    this->laEntriesNumber = new QLabel(this);
    this->ui->statusbar->addPermanentWidget(this->laEntriesNumber);

    connect(this, &MainWindow::fileChanged, [this](const QString &filePath) {
        this->laFilePath->setText(filePath);
    });

    connect(this, &MainWindow::entriesLoaded, [this](const int &entriesAmount) {
        this->laEntriesNumber->setText(QString::number(entriesAmount) + " entries loaded.");
    });

    this->selectFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectFile()
{
    this->selectedFile = QFileDialog::getOpenFileName(this, "Select File");
    emit this->fileChanged(this->selectedFile);
}

void MainWindow::processFile()
{
    this->modelEntries.clear();

    // update the attributes based on the selected parser
    this->regexp.setPattern(this->currentParser->getPattern());

    if(!this->regexp.isValid()) {
        QMessageBox::critical(
            this,
            "Invalid regular expression",
            QString("<p>The regular expression <br/> <tt>{regexp}</tt> <br/> is not valid</p>")
                .replace("{regexp}", this->currentParser->getPattern()));
    }

    this->modelHeaderLabels.setStringList(this->currentParser->getHeaderLabels());
    this->modelEntries.setHorizontalHeaderLabels(this->modelHeaderLabels.stringList());

    QFile file(this->selectedFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open the file:" << file.errorString();
        return;
    }

    QTextStream in(&file);

    QStringList extraLines;
    while (!in.atEnd()) {
        QString line = in.readLine();

        QRegularExpressionMatch match = this->regexp.match(line);
        if (match.hasMatch()) {
            QList<QStandardItem *> rowItems;
            for (int i = 1; i <= match.lastCapturedIndex(); ++i) {
                QStandardItem *item = new QStandardItem(match.captured(i));
                item->setEditable(false);
                if (i == match.lastCapturedIndex()) {
                    item->setText(item->text() + "\n" + extraLines.join("\n"));
                    extraLines.clear();
                }
                rowItems << item;
            }
            this->modelEntries.appendRow(rowItems);
        } else { //add to StringList of lines between records
            extraLines.append(line);
        }
    }

    file.close();

    this->ui->tvOutput->resizeColumnsToContents();
    emit this->entriesLoaded(this->modelEntries.rowCount());
}

void MainWindow::selectParserByIndex(int index)
{
    this->currentParser = this->parsers.at(index);
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

