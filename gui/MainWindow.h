#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFutureWatcher>
#include <boost/filesystem.hpp>

constexpr QSize WINDOW_DIMENSIONS(332, 312);
constexpr auto VERSION = "0.1.0";
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
    void ChangeFinished();
    void Launch();
    void Browse();
    void Save();
    void About();
private:
    Ui::MainWindow* ui;
    QFutureWatcher<void> change_watcher;
    auto ReadFields();
    void Warning(const std::string& msg);
    auto GetPort();
    auto GetIP();
    auto GetPath();
};

#endif // MAINWINDOW_H
