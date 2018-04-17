#include "debugwindow.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QVBoxLayout>

#include <memory>

DebugWindow::DebugWindow(QWidget* parent) 
    : QWidget(parent)
{
    auto grid = std::make_unique<QGridLayout>();
    grid->addWidget(CreateAsmBox(), 0, 0);
    grid->addWidget(CreateCpuBox(), 0, 1);

    setLayout(grid.release());
}

QGroupBox* DebugWindow::CreateAsmBox() const
{
    auto asmText = std::make_unique<QTextEdit>();
    asmText->setReadOnly(true);
    asmText->setPlaceholderText("Lorem ipsum");

    auto layout = std::make_unique<QVBoxLayout>();
    layout->addWidget(asmText.release());

    auto asmBox = std::make_unique<QGroupBox>(tr("Program Assembly"));
    asmBox->setLayout(layout.release());

    return asmBox.release();
}

QGroupBox* DebugWindow::CreateCpuBox() const
{
    auto layout = std::make_unique<QVBoxLayout>();

    auto cpuBox = std::make_unique<QGroupBox>(tr("CPU State"));
    cpuBox->setLayout(layout.release());

    return cpuBox.release();
}
