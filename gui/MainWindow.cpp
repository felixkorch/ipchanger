#include "ipchanger/Changer.h"
#include "ipchanger/Resolver.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QIntValidator>

namespace sys = ipchanger::system;
namespace ch = ipchanger::changer;
namespace fs = boost::filesystem;

static auto constexpr RAND = (sys::current_os == sys::OS::Windows) ? ".%%%%_%%%%_%%%%_%%%%.exe" : ".%%%%_%%%%_%%%%_%%%%";
static auto constexpr TIBIA = (sys::current_os == sys::OS::Windows) ? "Tibia.exe" : "Tibia";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window setup
    connect(ui->button_launch, SIGNAL (released()), this, SLOT (ChangeIP()));
    connect(ui->button_browse, SIGNAL (released()), this, SLOT (Browse()));
    this->setFixedSize(WINDOW_DIMENSIONS);
    this->statusBar()->setSizeGripEnabled(false);

    // Menu
    QAction* saveAct = new QAction(tr("&Save"), this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::Save);
    ui->menuOptions->addAction(saveAct);

    //Widgets
    ui->edit_port->setValidator( new QIntValidator(this) );
    ui->edit_ip->setPlaceholderText("127.0.0.1");
    ui->edit_port->setPlaceholderText("7171");
    ui->edit_path->setPlaceholderText("path/to/tibia");
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
std::optional<fs::path> MainWindow::GetPath()
{
    QString path = ui->edit_path->text();
    if(path.isEmpty())
        return std::nullopt;
    return std::make_optional(fs::path(path.toStdString()));
}

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
    QString file_name = QFileDialog::getSaveFileName(this,
            tr("Save Tibia Launcher"), "",
            tr("Tibia (*.exe);;All Files (*)"));

    if(file_name.isEmpty())
        return;

    auto port = GetPort();
    auto ip_raw = GetIP();
    auto file_path = GetPath();

    if(!port.has_value() || !file_path.has_value() || !ip_raw.has_value()) {
        Warning("Please fill in all fields.");
        return;
    }

    auto resolve_ip = sys::Resolve(ip_raw.value());
    auto ip = resolve_ip.has_value() ? resolve_ip.value() : ip_raw;

    fs::path in(file_path.value() / TIBIA);
    fs::path out(file_name.toStdString());
    ch::ChangeIP(ip.value(), port.value(), in, out);
}

void MainWindow::Browse()
{
   QFileDialog browse;
   browse.setOption(QFileDialog::ShowDirsOnly);
   auto path_name = browse.getExistingDirectory();
   if(path_name.isNull() || path_name.isEmpty())
       return;
   ui->edit_path->setText(path_name);
}

void MainWindow::ChangeIP()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Tibia IP-Changer");
    msgBox.setIcon(QMessageBox::Warning);

    auto port = GetPort();
    auto ip_raw = GetIP();
    auto path_name = GetPath();

    if(!port.has_value() || !path_name.has_value() || !ip_raw.has_value()) {
        Warning("Please fill in all fields.");
        return;
    }

    auto resolve_ip = sys::Resolve(ip_raw.value());
    auto ip = resolve_ip.has_value() ? resolve_ip.value() : ip_raw;
    auto unique_name = fs::unique_path(RAND); // Generates a unqiue name

    auto out = path_name.value() / unique_name;
    auto in = path_name.value() / TIBIA;

    if(!fs::exists(in)) {
        Warning("Couldn't find Tibia(.exe)");
        return;
    }

    ch::ChangeIP(ip.value(), port.value(), in, out);
    ch::LaunchTemporary(out);
}


MainWindow::~MainWindow()
{
    delete ui;
}
