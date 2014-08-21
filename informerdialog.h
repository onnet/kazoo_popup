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

#ifndef INFORMERDIALOG_H
#define INFORMERDIALOG_H

#include <QDialog>

namespace Ui {
class InformerDialog;
}

class QMouseEvent;

class ContactInfo;

class InformerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InformerDialog(QWidget *parent = 0);
    ~InformerDialog();

    void setContactInfo(ContactInfo *contactInfo);
    ContactInfo *contactInfo() const;
    void setAnswered(bool answered);
    bool isAttached() const;

signals:
    void dialogAttached(bool attached);

protected:
    // for moving frameless window
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::InformerDialog *ui;
    ContactInfo *m_contactInfo = nullptr;

    // for moving frameless window
    QPoint m_dragPosition;
    bool m_dragging;

private slots:
    void processAttach(bool checked);
};

#endif // INFORMERDIALOG_H
