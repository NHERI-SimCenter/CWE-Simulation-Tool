#include "dialogabout.h"
#include "ui_dialogabout.h"
#include <QApplication>
#include <QFile>

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);

    QFile file(":/help/About/CWEabout.html");
    if (file.open(QFile::ReadOnly))
    {
        ui->aboutTextBrowser->setHtml(file.readAll());
    }

}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_btn_backToTop_clicked()
{
    ui->aboutTextBrowser->setSource(QUrl("#top"));
}
