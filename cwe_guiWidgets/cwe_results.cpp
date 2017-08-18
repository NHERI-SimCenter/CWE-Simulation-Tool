#include "cwe_results.h"
#include "ui_cwe_results.h"
#include "mytablemodel.h"

CWE_Results::CWE_Results(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Results)
{
    ui->setupUi(this);

    model = new MyTableModel(this);

    QStringList HeaderList;
    HeaderList << "result name" << "" << ""  << "type" << "description" << "file size";
    model->setHeaders(HeaderList);

    ui->resultsTableView->setModel(model);
    ui->resultsTableView->verticalHeader()->setVisible(false);

    ui->resultsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
    QString eye;  // this shoul dbecome an icon
    QString load; // this should become an icon

    if (showeye) {
        eye = "EyeIcon";
    }
    else {
        eye = "";
    }

    if (download) {
        load = "download";
    }
    else {
        load = "";
    }

    QStringList List;
    List << name << eye << load << type << description << filesize;
    model->addStringList(List);
}

void CWE_Results::addDummyResult(void)
{
    this->addResult( "your job name", true, true, "some type", "some description",  "huge");
}
