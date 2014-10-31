#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

private:
    Ui::DebugDialog *ui;

private slots:
    void appendLog(const QString &log);
    void ensureCursorVisible();
};

#endif // DEBUGDIALOG_H
