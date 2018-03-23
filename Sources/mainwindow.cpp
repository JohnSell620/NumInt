#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
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


// New window to display previous matrix commands
void MainWindow::on_actionNew_Window_triggered()
{
    mDialog = new MyDialog(this);
    mDialog->show();
}


// Create and run executable
void MainWindow::on_pushButton_2_clicked()
{
    QString code = ui->textEdit->toPlainText();
    QString inclds = "#include <iostream>\n#include \"Matrix.hpp\"\n\n";
    QString header = "int main(int argc, char *argv[])\n{ \n";
    QString trailer = "\n\nreturn 0;\n}\n";
    QString executable = inclds + header + code + trailer;

    remove("executable.cpp");
    QString filename = "executable.cpp";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << executable;
        file.close();
    }


    // create child process to compile executable
    pid_t pID = vfork();

    if (pID == 0)
    {
        std::system("c++ -std=c++11 -Ofast -march=native -DNDEBUG -c executable.cpp -o executable.o");
        _Exit(0);
    }
    else if (pID < 0)
    {
        std::cerr << "Fork failed" << std::endl;
        ui->plainTextEdit->setPlainText("Failed to compile executable.o; fork failed");
        std::exit(1);
    }
    else
    {
        std::system("c++ -std=c++11 -Ofast -march=native -DNDEBUG executable.o Matrix.o Vector.o -o executable");
        ui->plainTextEdit->setPlainText("Success compiling executable.o");
    }

    if (exists("executable"))
    {
        std::string str = exec("./executable");
        QString result = QString::fromUtf8(str.c_str());
        ui->plainTextEdit->setPlainText(result);
        QString trailer2 = "\n\n-----------------------------------------------------\n\n";
        setCommandHistory("commands:\n\n" + code + "\n\nresults:\n\n" + result + trailer2);
    }
    else
    {
        ui->plainTextEdit->setPlainText("Error compiling executable");
    }

}


void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Text File"), "./", tr("cpp Files (*.cpp)"));
    if (fileName != "")
    {
        QFile file(QFileInfo(fileName).absoluteFilePath());

        if (file.exists())
        {
            QMessageBox::StandardButton chosenButton
                = QMessageBox::warning(this, tr("File exists"), tr("The file already exists. Do you want to overwrite it?"),
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Cancel);
            if (chosenButton != QMessageBox::Ok)
            {
                return; // Save cancelled
            }
        }
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Failed to save file"));
            return; // Aborted
        }
        // Save data
        QFile inFile("executable.cpp");
        if(!inFile.open(QIODevice::ReadOnly))
            QMessageBox::information(0, "error", inFile.errorString());

        QTextStream in(&inFile);
        QString text = in.readAll();
        inFile.close();
        QTextStream out(&file);
        out << text;
        file.close();
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}


bool MainWindow::exists(const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

std::string MainWindow::exec(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}


