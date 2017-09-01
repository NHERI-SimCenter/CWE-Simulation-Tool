#include "cwe_results.h"
#include "ui_cwe_results.h"
//#include "mytablemodel.h"
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFileDialog>
#include "qdebug.h"
#include <QPixmap>

CWE_Results::CWE_Results(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Results)
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);

    QStringList HeaderList;
    HeaderList << "result name" << "" << ""  << "type" << "description" << "file size";
    model->setHorizontalHeaderLabels(HeaderList);

    ui->resultsTableView->setModel(model);
    ui->resultsTableView->verticalHeader()->setVisible(false);

    ui->resultsTableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->resultsTableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->resultsTableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->resultsTableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->resultsTableView->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Stretch);
    ui->resultsTableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Stretch);

    this->addDummyResult();
}

CWE_Results::~CWE_Results()
{
    delete ui;
}

void CWE_Results::setName(const QString &s)     {ui->label_theName->setText(s);}
void CWE_Results::setType(const QString &s)     {ui->label_theType->setText(s);}
void CWE_Results::setLocation(const QString &s) {ui->label_theLocation->setText(s);}


void CWE_Results::addResult(QString name, bool showeye, bool download, QString type, QString description, QString filesize)
{
    // Make data
    QList<QStandardItem *> List;

    QStandardItem *var1 = new QStandardItem(name);
    List.append(var1);

    QStandardItem *var2 = new QStandardItem;
    if (showeye) {
        var2->setIcon(QIcon(":/buttons/images/CWE_eye.png"));
    }
    else {
        var2->setIcon(QIcon(":/buttons/images/CWE_empty.png"));
    }
    List.append(var2);

    QStandardItem *var3 = new QStandardItem;if (download) {
        var3->setIcon(QIcon(":/buttons/images/CWE_download.png"));
    }
    else {
        var3->setIcon(QIcon(":/buttons/images/CWE_empty.png"));
    }
    List.append(var3);

    QStandardItem *var4 = new QStandardItem(type);
    List.append(var4);
    QStandardItem *var5 = new QStandardItem(description);
    List.append(var5);
    QStandardItem *var6 = new QStandardItem(filesize);
    List.append(var6);

    // Populate our model
    model->appendRow(List);
}

void CWE_Results::addDummyResult(void)
{
    this->addResult( "your job name", true, true, "some type", "some description",  "huge");
}

void CWE_Results::on_downloadEntireCaseButton_clicked()
{
    /* download the entire case TODO*/
    QFileDialog *saveDialog = new QFileDialog();

    QString foldername = "unknown";

    qDebug() << "don't know where to find stuff to download to " << foldername;
}
