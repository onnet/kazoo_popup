/* --------------------------------------------------------------------------------------------------------------------------
 * ** 
 * ** Ordered by Kirill Sysoev kirill.sysoev@gmail.com
 * ** (OnNet communications Inc. http://onnet.su)
 * ** 
 * ** Developed by Alexey Lysenko lysenkoalexmail@gmail.com
 * ** 
 * ** Please report bugs and provide any possible patches directly to this repository: https://github.com/onnet/kazoo_popup.git
 * ** 
 * ** If you would like to order additional development, contact Alexey Lysenko over email lysenkoalexmail@gmail.com directly.
 * ** 
 * ** 
 * ** This application:
 * **  - connects to Kazoo whapp blackhole;
 * **  - listens for incoming calls;
 * **  - queries third party server whether it knows anything about caller's number;
 * **  - Pop's Up window with provided info.
 * ** 
 * ** It is:
 * **  - written in Qt which promises to be crossplatform application (hopefully);
 * **  - is NOT production ready, but intended to be a simple example of using blachole whapp
 * **    (please note, that blackhole whapp doesn't support secure connectoin over SSL yet; check KAZOO-2632).
 * ** 
 * ** Good luck!
 * ** 
 * ** -------------------------------------------------------------------------------------------------------------------------*/

#include "informerdialog.h"
#include "ui_informerdialog.h"

#include "contactinfo.h"

#include <QMouseEvent>

InformerDialog::InformerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InformerDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);

    connect(ui->closeToolButton, &QToolButton::clicked,
            this, &InformerDialog::hide);
}

InformerDialog::~InformerDialog()
{
    delete ui;
}

void InformerDialog::setContactInfo(ContactInfo *contactInfo)
{
    ui->informationLabel->setText(contactInfo->toHtml());
}

void InformerDialog::setAnswered(bool answered)
{
    if (answered)
        setStyleSheet("QDialog {\nbackground-color: #63C248;\n}");
    else
        setStyleSheet("QDialog {\nbackground-color: #FFFFBF;\n}");
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
