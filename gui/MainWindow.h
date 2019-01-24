#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <boost/filesystem.hpp>

constexpr QSize WINDOW_DIMENSIONS(332, 312);

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
    void ChangeIP();
    void Browse();
    void Save();
private:
    Ui::MainWindow *ui;
    void Warning(const std::string& msg);
    std::optional<unsigned int> GetPort();
    std::optional<std::string> GetIP();
    std::optional<boost::filesystem::path> GetPath();
};

#endif // MAINWINDOW_H
