#ifndef INFORMERDIALOG_H
#define INFORMERDIALOG_H

#include <QDialog>

namespace Ui {
class InformerDialog;
}

class QMouseEvent;

class Caller;

class InformerDialog : public QDialog
{
    Q_OBJECT

public:
    enum State
    {
        kStateRinging,
        kStateAnswered,
        kStateAnsweredAnother
    };

    explicit InformerDialog(QWidget *parent = 0);
    ~InformerDialog();

    void setState(State state);

    bool isAttached() const;

    void setCaller(const Caller &caller);
    void setCallee(const QString &calleeNumber, const QString &calleeName);

signals:
    void dialogAttached(bool attached);

protected:
    // for moving frameless window
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::InformerDialog *ui;
    QString m_callerUrl;

    // for moving frameless window
    QPoint m_dragPosition;
    bool m_dragging;

private slots:
    void processAttach(bool checked);
    void openCallerUrl();
};

#endif // INFORMERDIALOG_H
