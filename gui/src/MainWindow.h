#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <filesystem>

constexpr QSize WINDOW_DIMENSIONS(360, 380);
constexpr auto VERSION = "0.1.3";
constexpr auto TITLE = "Tibia IP-Changer";

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void LaunchFinished();
    void SaveFinished();
    void Launch();
    void Browse();
    void Save();
    void About();
private:
    Ui::MainWindow* ui;
    QFutureWatcher<void> save_watcher;
    QFutureWatcher<std::filesystem::path> launch_watcher;
    void LoadingDialog();
    auto ReadFields();
    void Warning(const std::string& msg);
    auto GetPort();
    auto GetIP();
    auto GetPath();
};

#endif // MAINWINDOW_H
