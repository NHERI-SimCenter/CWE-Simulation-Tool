#include "dialogabout.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QString versionText, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);

    QFile file(":/About/CWEabout.html");
    if (file.open(QFile::ReadOnly))
    {
        ui->aboutTextBrowser->setHtml(file.readAll());
    }
    QString versionString = "About Version ";
    versionString = versionString.append(versionText);
    ui->label_about->setText(versionString);
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_btn_backToTop_clicked()
{
    ui->aboutTextBrowser->setSource(QUrl("#top"));
}
