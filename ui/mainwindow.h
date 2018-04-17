#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include <memory>

class DebugWindow;
class RenderWidget;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

private slots:
    void About();
    void Open();
    void OpenDebugWindow();
    void Play();

private:
    void CreateMenus();

private:
    std::unique_ptr<DebugWindow> m_debugWindow;
    std::unique_ptr<RenderWidget> m_renderWidget;
};

#endif // MAIN_WINDOW_H