#include <iomanip>
#include <iostream>
#include <string>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydialog.h"
#include "gaussquadrature.cpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->lineEdit_6->clear();

    QString ol = ui->lineEdit_8->text();
    QString ou = ui->lineEdit_7->text();
    QString ml = ui->lineEdit_5->text();
    QString mu = ui->lineEdit_4->text();
    QString il = ui->lineEdit_3->text();
    QString iu = ui->lineEdit_2->text();
    QString expression = ui->lineEdit->text();

    std::string a = ol.toStdString();
    std::string b = ou.toStdString();
    std::string g1 = ml.toStdString();
    std::string g2 = mu.toStdString();
    std::string h1 = il.toStdString();
    std::string h2 = iu.toStdString();
    std::string integrand = expression.toStdString();

    QString diffOrder = ui->comboBox->currentText() + ui->comboBox_2->currentText() + ui->comboBox_3->currentText();
    std::string integrationOrder = diffOrder.toStdString();

    // Create Integration object and store for recall
    Integration integC (expression, diffOrder, ol, ou, ml, mu, il, iu);
    intStack->prepend(integC);

    double value = gaussquadrature(integrand, integrationOrder, a, b, g1, g2, h1, h2);
    QString valueAsString = QString::number(value, 'g', 16);
    ui->lineEdit_6->insert(valueAsString);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
    {
        if (intStack->size() >= 1)
        {
            Integration integUK = intStack->takeFirst();
            ui->lineEdit->clear();
            ui->lineEdit->insert(integUK.expression());
            intStack->append(integUK);
        }
    }
    if (event->key() == Qt::Key_Down)
    {
        if (intStack->size() >= 1)
        {
            Integration integDK = intStack->takeLast();
            ui->lineEdit->clear();
            ui->lineEdit->insert(integDK.expression());
            intStack->prepend(integDK);
        }
    }
}

// New window to display previous integration information
void MainWindow::on_actionNew_Window_triggered()
{
    mDialog = new MyDialog(this);
    mDialog->show();
}
