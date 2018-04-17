#ifndef DEBUG_WINDOW_H
#define DEBUG_WINDOW_H

#include <QWidget>

class QGroupBox;

class DebugWindow : public QWidget
{
public:
    DebugWindow(QWidget* parent = nullptr);

private:
    QGroupBox* CreateAsmBox() const;
    QGroupBox* CreateCpuBox() const;
};

#endif // DEBUG_WINDOW_H