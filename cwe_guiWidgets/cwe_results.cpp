#include "cwe_results.h"
#include "ui_cwe_results.h"

#include "vwtinterfacedriver.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

CWE_Results::CWE_Results(QWidget *parent) :
    CWE_Super(parent),
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
}

CWE_Results::~CWE_Results()
{
    delete ui;
}

void CWE_Results::linkDriver(VWTinterfaceDriver * newDriver)
{
    CWE_Super::linkDriver(newDriver);
    QObject::connect(myDriver, SIGNAL(haveNewCase()),
                     this, SLOT(newCaseGiven()));
}

void CWE_Results::resetViewInfo()
{
    model->clear();
    viewIsValid = false;
    CFDcaseInstance * currentCase = myDriver->getCurrentCase();
    if (currentCase == NULL) return;
    CFDanalysisType * theTemplate = currentCase->getMyType();
    if (theTemplate == NULL) return;
    QJsonObject configobj    = theTemplate->getRawConfig()->object();
    QJsonObject stagesobj = configobj.value("stages").toObject();

    QMap<QString, StageState> currentStates = currentCase->getStageStates();

    viewIsValid = true;

    ui->label_theName->setText(currentCase->getCaseName());
    ui->label_theType->setText(theTemplate->getName());
    ui->label_theLocation->setText(currentCase->getCaseFolder());

    for (auto itr = stagesobj.constBegin(); itr != stagesobj.constEnd(); itr++)
    {
        //QString stageName = itr.key();
        QJsonArray resultArray = (*itr).toObject().value("results").toArray();
        for (auto itr2 = resultArray.constBegin(); itr2 != resultArray.constEnd(); itr2++)
        {
            QJsonObject aResult = (*itr2).toObject();
            //addResult(aResult.value("name"), true, false, aResult.value("type"),"N/a", "N/a");
        }
    }
}

void CWE_Results::on_downloadEntireCaseButton_clicked()
{
    if (myDriver->getCurrentCase() == NULL)
    {
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this, "Select Destination Folder:");

    myDriver->getCurrentCase()->downloadCase(fileName);
}

void CWE_Results::newCaseGiven()
{
    CFDcaseInstance * newCase = myDriver->getCurrentCase();

    if (newCase != NULL)
    {
        QObject::connect(newCase, SIGNAL(haveNewState(CaseState)),
                         this, SLOT(newCaseState(CaseState)));
    }

    resetViewInfo();
}

void CWE_Results::newCaseState(CaseState newState)
{
    //TODO : PRS
}


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
