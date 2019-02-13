#include <Changer.h>
#include "ui_MainWindow.h"
#include "MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QIntValidator>
#include <QtConcurrent/QtConcurrent>

namespace sys = ipchanger::system;
namespace fs = std::filesystem;

static auto constexpr RAND = (current_os == OS::Windows) ? ".%%%%_%%%%_%%%%_%%%%.exe" : ".%%%%_%%%%_%%%%_%%%%";
static auto constexpr TIBIA = (current_os == OS::Windows) ? "Tibia.exe" : "Tibia";
static auto constexpr DEFAULT_IP = "127.0.0.1";
static auto constexpr DEFAULT_PORT = "7171";
static auto constexpr DEFAULT_PATH = "path/to/tibia";

struct Fields
{
    const std::string ip;
    unsigned int port;
    const fs::path in;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window setup
    connect(ui->button_launch, SIGNAL (released()), this, SLOT (Launch()));
    connect(ui->button_browse, SIGNAL (released()), this, SLOT (Browse()));
    connect(&this->launch_watcher, SIGNAL (finished()), this, SLOT (LaunchFinished()));
    connect(&this->save_watcher, SIGNAL (finished()), this, SLOT (SaveFinished()));

    this->setFixedSize(WINDOW_DIMENSIONS);
    this->statusBar()->setSizeGripEnabled(false);
    QWidget::setWindowTitle(TITLE);

    // Menu
    QAction* saveAct = new QAction{ tr("&Save"), this };
    connect(saveAct, &QAction::triggered, this, &MainWindow::Save);
    ui->menuOptions->addAction(saveAct);

    QAction* aboutAct = new QAction{ tr("&About"), this };
    connect(aboutAct, &QAction::triggered, this, &MainWindow::About);
    ui->menuAbout->addAction(aboutAct);

    //Widgets
    ui->edit_port->setValidator( new QIntValidator{ this } );
    ui->edit_ip->setPlaceholderText(DEFAULT_IP);
    ui->edit_port->setPlaceholderText(DEFAULT_PORT);
    ui->edit_path->setPlaceholderText(DEFAULT_PATH);
}

auto MainWindow::GetPort()
{
    QString port{ ui->edit_port->text() };
    if(port.isEmpty())
        return std::optional<unsigned int>{};
    return std::make_optional(port.toUInt());
}
auto MainWindow::GetIP()
{
    QString ip { ui->edit_ip->text() };
    if(ip.isEmpty())
        return std::optional<std::string>{};

	auto temp = ip.toLocal8Bit(); // Required on Windows (convert to UTF-8)
	return std::make_optional(std::string(temp));
}
auto MainWindow::GetPath()
{
    QString path{ ui->edit_path->text() };
    if(path.isEmpty())
        return std::optional<fs::path>{};

	auto temp = path.toLocal8Bit(); // Required on Windows (convert to UTF-8)
    return std::make_optional(fs::path{ std::string(temp) });
}

void MainWindow::Warning(const std::string& msg)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(TITLE);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(msg.c_str());
    msgBox.exec();
}

void MainWindow::About()
{
    QString ver = VERSION;
    QString msg = "An IP-Changer for OT\nVersion: " + ver;

    QMessageBox::about(this, TITLE, msg);
}

auto MainWindow::ReadFields()
{
    auto port = GetPort();
    auto ip_raw = GetIP();
    auto path_name = GetPath();

    if(!port.has_value() || !path_name.has_value() || !ip_raw.has_value()) {
        Warning("Please fill in all fields.");
        return std::optional<Fields>{};
    }

    auto in = path_name.value() / TIBIA;

    if(!fs::exists(in)) {
        Warning("Couldn't find Tibia(.exe)");
        return std::optional<Fields>{};
    }

    auto resolve_ip = sys::Resolve(ip_raw.value());
    auto ip = resolve_ip.has_value() ? resolve_ip.value() : ip_raw.value();
    ui->label_ip->setText("IP-Address (" +  QString(ip.c_str()) + ")");
    return std::make_optional(Fields{ ip, port.value(), in });
}

void MainWindow::LaunchFinished()
{
    QtConcurrent::run([out{ launch_watcher.future() }]{
        sys::ExecuteBinary(out);
        fs::remove(out);
    });
}

void MainWindow::SaveFinished()
{
}

void MainWindow::Save()
{
    auto fields = ReadFields();
    if(!fields.has_value())
        return;

    auto c = fields.value();

    QString file_name{ QFileDialog::getSaveFileName(this,
            tr("Save Tibia Launcher"), "",
            tr("Tibia (*.exe);;All Files (*)")) };

    if(file_name.isEmpty())
        return;

    auto temp = file_name.toLocal8Bit(); // Required on Windows (convert to UTF-8)
    auto future = QtConcurrent::run([ip{ c.ip }, port{ c.port }, in{ c.in }, out{ temp.toStdString() }] {
        ipchanger::Changer changer{ ip, port, in };
        const std::string& buff = changer.Data();
        sys::WriteBinary(fs::path(out), buff.data(), buff.size());
    });
    this->save_watcher.setFuture(future);
    LoadingDialog();
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

void MainWindow::LoadingDialog()
{
    QProgressDialog dialog(nullptr, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    dialog.setLabelText(QString("Patching..."));
    dialog.setCancelButton(nullptr);
    dialog.setRange(0, 0);
    dialog.setWindowModality(Qt::WindowModal);
    QObject::connect(&launch_watcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    QObject::connect(&save_watcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    dialog.exec();
}

void MainWindow::Launch()
{
    auto fields = ReadFields();
    if(!fields.has_value())
        return;

    auto c = fields.value();
	fs::path out = sys::TempName(RAND, c.in.parent_path());

    auto future = QtConcurrent::run([ip{ c.ip }, port{ c.port }, in{ c.in }, out] {
        ipchanger::Changer changer{ ip, port, in };
        const std::string& buff = changer.Data();
        sys::WriteBinary(out, buff.data(), buff.size(), sys::WINDOWS_HIDDEN_FILE);
        return out;
    });
    this->launch_watcher.setFuture(future);
    LoadingDialog();
}


MainWindow::~MainWindow()
{
    delete ui;
}
