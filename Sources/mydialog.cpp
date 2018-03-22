#include "mydialog.h"
#include "ui_mydialog.h"
#include "mainwindow.h"
#include <QString>

MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyDialog)
{
    ui->setupUi(this);
}

MyDialog::~MyDialog()
{
    delete ui;
}

void MyDialog::on_pushButton_clicked()
{
    //print the input and results
    //QString line = "integrand: " + inte + "\n" + "value: " + eval + "\n";
    //ui->plainTextEdit->insertPlainText(line);
}
