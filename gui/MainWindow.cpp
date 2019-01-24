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

std::optional<unsigned int> MainWindow::GetPort()
{
    QString port = ui->edit_port->text();
    if(port.isEmpty())
        return std::nullopt;
    return std::make_optional(port.toUInt());
}
std::optional<std::string> MainWindow::GetIP()
{
    QString ip = ui->edit_ip->text();
    if(ip.isEmpty())
        return std::nullopt;
    return std::make_optional(ip.toStdString());
}
std::optional<std::string> MainWindow::GetPath()
{
    QString path = ui->edit_path->text();
    if(path.isEmpty())
        return std::nullopt;
    return std::make_optional(path.toStdString());
}

namespace sys = ipchanger::system;
namespace ch = ipchanger::changer;
namespace fs = boost::filesystem;

void MainWindow::Warning(const std::string& msg)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Tibia IP-Changer");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(msg.c_str());
    msgBox.exec();
}

void MainWindow::Save()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Tibia Launcher"), "",
            tr("Tibia (*.exe);;All Files (*)"));

    if(fileName.isEmpty())
        return;

    if(!GetIP().has_value() || !GetPath().has_value() || !GetPort().has_value()) {
        Warning("Please fill in all fields.");
        return;
    }

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

    auto port = GetPort();
    auto ip_raw = GetIP();
    auto file_path = GetPath();

    if(!port.has_value() || !file_path.has_value() || !ip_raw.has_value()) {
        Warning("Please fill in all fields.");
        return;
    }

    auto resolve_ip = sys::Resolve(ip_raw.value());
    auto ip = resolve_ip.has_value() ? resolve_ip.value() : ip_raw;

    auto constexpr RAND = (sys::current_os == sys::OS::Windows) ? ".%%%%_%%%%_%%%%_%%%%.exe" : ".%%%%_%%%%_%%%%_%%%%";
    auto unique_name = fs::unique_path(RAND); // Generates a unqiue name

    auto out = fs::path(file_path.value()).parent_path() / unique_name;
    auto in = fs::path(file_path.value());

    if(!fs::exists(in)) {
        Warning("Couldn't find Tibia executable");
        return;
    }

    ch::ChangeIP(ip.value(), port.value(), in, out);
    ch::LaunchTemporary(out);
}


MainWindow::~MainWindow()
{
    delete ui;
}
