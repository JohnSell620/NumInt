#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include "mydialog.h"
#include "integrationstack.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    IntegrationStack<Integration> *intStack = new IntegrationStack<Integration>();

private slots:
    void on_pushButton_clicked();
    void keyPressEvent(QKeyEvent*);
    void on_actionNew_Window_triggered();

private:
    Ui::MainWindow *ui;
    MyDialog *mDialog;
};

#endif // MAINWINDOW_H
