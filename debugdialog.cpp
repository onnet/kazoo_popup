#include "debugdialog.h"
#include "ui_debugdialog.h"

#include "logger.h"

#include <QTimer>
#include <QScrollBar>

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

    Logger *logger = Logger::instance();
    connect(logger, &Logger::newLog,
            this, &DebugDialog::appendLog);

    ui->plainTextEdit->setPlainText(logger->logs());

    QTimer::singleShot(0, this, SLOT(ensureCursorVisible()));
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::appendLog(const QString &log)
{
    ui->plainTextEdit->appendPlainText(log);
}

void DebugDialog::ensureCursorVisible()
{
    QScrollBar *scrollBar = ui->plainTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
