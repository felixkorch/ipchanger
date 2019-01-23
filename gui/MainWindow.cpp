#include "ipchanger/Changer.h"
#include "ipchanger/Resolver.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QIntValidator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window setup
    connect(ui->button_launch, SIGNAL (released()), this, SLOT (ChangeIP()));
    connect(ui->button_browse, SIGNAL (released()), this, SLOT (Browse()));
    this->setFixedSize(QSize(332, 312));
    this->statusBar()->setSizeGripEnabled(false);

    // Menu
    QAction* saveAct = new QAction(tr("&Save"), this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::Save);
    ui->menuOptions->addAction(saveAct);

    //Widgets
    ui->edit_port->setValidator( new QIntValidator(this) );
}

namespace sys = ipchanger::system;
namespace ch = ipchanger::changer;
namespace fs = boost::filesystem;

void MainWindow::Save()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Tibia Launcher"), "",
            tr("Tibia (*.exe);;All Files (*)"));

    if(fileName.isEmpty())
        return;

    const unsigned int port = ui->edit_port->text().toUInt();
    const std::string ip_raw = ui->edit_ip->text().toStdString();
    auto resolve_ip = sys::Resolve(ip_raw);
    const std::string ip = resolve_ip.has_value() ? resolve_ip.value() : ip_raw;
    const std::string file_path = ui->edit_path->text().toStdString();

    fs::path in(file_path);
    fs::path out(fileName.toStdString());
    ipchanger::changer::ChangeIP(ip, port, in, out);
}

void MainWindow::Browse()
{
   ui->edit_path->setText(QFileDialog::getOpenFileName());
}

void MainWindow::ChangeIP()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Tibia IP-Changer");
    msgBox.setIcon(QMessageBox::Warning);

    if(ui->edit_ip->text().isEmpty() || ui->edit_path->text().isEmpty() || ui->edit_port->text().isEmpty()) {
        msgBox.setText("Please fill in all fields.");
        msgBox.exec();
        return;
    }

    const unsigned int port = ui->edit_port->text().toUInt();
    const std::string ip_raw = ui->edit_ip->text().toStdString();
    auto resolve_ip = sys::Resolve(ip_raw);
    const std::string ip = resolve_ip.has_value() ? resolve_ip.value() : ip_raw;
    const std::string file_path = ui->edit_path->text().toStdString();

    auto constexpr RAND = (sys::current_os == sys::OS::Windows) ? ".%%%%_%%%%_%%%%_%%%%.exe" : ".%%%%_%%%%_%%%%_%%%%";
    auto unique_name = fs::unique_path(RAND); // Generates a unqiue name

    auto out = fs::path(file_path).parent_path() / unique_name;
    auto in = fs::path(file_path);

    if(!fs::exists(in)) {
        msgBox.setText("Couldn't find Tibia executable");
        msgBox.exec();
        return;
    }

    ch::ChangeIP(ip, port, in, out);
    ch::LaunchTemporary(out);
}


MainWindow::~MainWindow()
{
    delete ui;
}
