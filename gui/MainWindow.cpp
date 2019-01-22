#include "ipchanger/Changer.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window setup
    connect(ui->pushButton, SIGNAL (released()), this, SLOT (ChangeIP()));
    connect(ui->pushButton_2, SIGNAL (released()), this, SLOT (Browse()));
    this->setFixedSize(QSize(312, 297));
    this->statusBar()->setSizeGripEnabled(false);
}

void MainWindow::Browse()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    ui->lineEdit->setText(dialog.getExistingDirectory());
}

void MainWindow::ChangeIP()
{
    if(ui->lineEdit->text().isEmpty() || ui->lineEdit_2->text().isEmpty() || ui->lineEdit_3->text().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Tibia IP-Changer");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Please fill in all fields.");
        msgBox.exec();
        return;
    }

    const unsigned int port = ui->lineEdit_2->text().toUInt();
    const std::string ip = ui->lineEdit_3->text().toStdString() + '\0'; // The string isn't null terminated for some reason.
    const std::string path = ui->lineEdit->text().toStdString();

    ipchanger::changer::ChangeIP(ip, port, path);
}


MainWindow::~MainWindow()
{
    delete ui;
}
