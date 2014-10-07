#include "informerdialog.h"
#include "ui_informerdialog.h"

#include "caller.h"

#include <QMouseEvent>

#include <QDesktopServices>

InformerDialog::InformerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InformerDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);

    connect(ui->closeToolButton, &QToolButton::clicked,
            this, &InformerDialog::hide);
    connect(ui->attachToolButton, &QToolButton::clicked,
            this, &InformerDialog::processAttach);
    connect(ui->openUrlToolButton, &QToolButton::clicked,
            this, &InformerDialog::openCallerUrl);
}

InformerDialog::~InformerDialog()
{
    delete ui;
}

void InformerDialog::setCaller(const Caller &caller)
{
    ui->informationLabel->setText(caller.callerInfo());
    m_callerUrl = caller.callerUrl();
}

void InformerDialog::setCallee(const QString &calleeNumber, const QString &calleeName)
{
    QString text = ui->informationLabel->text();
    text.append("\nCallee number: " + calleeNumber + "\nCallee name: " + calleeName);
    ui->informationLabel->setText(text);
    adjustSize();
}

void InformerDialog::setState(State state)
{
    if (state == kStateAnswered)
    {
        setStyleSheet("QDialog {\nbackground-color: #63C248;\n}");
        ui->stateLabel->setText(tr("State: Answered"));
    }
    else if (state == kStateAnsweredAnother)
    {
        setStyleSheet("QDialog {\nbackground-color: #FFBA66;\n}");
        ui->stateLabel->setText(tr("State: Answered by another user"));
    }
    else if (state == kStateRinging)
    {
        setStyleSheet("QDialog {\nbackground-color: #FFFFBF;\n}");
        ui->stateLabel->setText(tr("State: Ringing"));
    }
}

void InformerDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (rect().contains(event->pos()))
        {
            m_dragging = true;
            setCursor(Qt::SizeAllCursor);
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }
}

void InformerDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_dragging = false;
    setCursor(Qt::ArrowCursor);
}

void InformerDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (m_dragging)
        {
            move(event->globalPos() - m_dragPosition);
            event->accept();
        }
    }
}

bool InformerDialog::isAttached() const
{
    return ui->attachToolButton->isChecked();
}

void InformerDialog::processAttach(bool checked)
{
    emit dialogAttached(checked);
    if (checked)
    {
        setStyleSheet("QDialog {\nbackground-color: #BFDFFF;\n}");
        ui->stateLabel->setText(tr("State: Attached"));
    }
    else
    {
        ui->stateLabel->setText(tr("State: Detached"));
    }
}

void InformerDialog::openCallerUrl()
{
    QDesktopServices::openUrl(QUrl(m_callerUrl));
}
