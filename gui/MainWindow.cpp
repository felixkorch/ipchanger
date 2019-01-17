#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include "ipchanger/Changer.h"
#include "ipchanger/Client.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window setup
    connect(ui->pushButton, SIGNAL (released()), this, SLOT (ChangeIP()));
    this->setFixedSize(QSize(354, 376));
    this->statusBar()->setSizeGripEnabled(false);


    // Initialize widgets

    for(auto c : ipchanger::clients)
        ui->comboBox->addItem(QString::number(c.version));

    ui->comboBox->setMaxVisibleItems(8);
}

void MainWindow::ChangeIP()
{
    const std::string ip = ui->lineEdit_3->text().toStdString();
    const unsigned int port = ui->lineEdit_2->text().toUInt();
    const unsigned int version = ui->comboBox->currentText().toUInt();

    const std::string success_message = "IP successfully changed!\nPort: " + std::to_string(port) + "\nVersion: " + std::to_string(version);
    auto failed_message = "";
    QMessageBox msgBox;
    msgBox.setWindowTitle("Tibia IP-Changer");

    if(ui->lineEdit_2->text().size() == 0 || ui->lineEdit_3->text().size() == 0) {
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Please fill in all fields");
        msgBox.exec();
        return;
    }

    namespace ch = ipchanger::changer;
    int result = ch::ChangeIP(ip, port, version);

    switch(result) {
        case 0: failed_message = "Please input a valid version"; break;
        case 1: failed_message = "Could not find Tibia process"; break;
        case 2: failed_message = "Try again with root"; break;
        case 3: failed_message = "Could not write RSA"; break;
        case 4: failed_message = "Could not write ip"; break;
        case 5: failed_message = "Could not write port"; break;
    }

    if(result == 6) {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(success_message.c_str());
    }
    else {
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(failed_message);
    }

    msgBox.exec();
}


MainWindow::~MainWindow()
{
    delete ui;
}
