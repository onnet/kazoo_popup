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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QSystemTrayIcon;

class WebSocketManager;
class InformerDialog;
class ContactInfo;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *event);

private:
    void createTrayIcon();
    bool isCorrectSettings() const;

    Ui::MainWindow *ui;
    QSystemTrayIcon *m_trayIcon;

    QHash<ContactInfo*, InformerDialog*> m_informerDialogsHash;
    ContactInfo *m_contactInfoForTimer = nullptr;

    WebSocketManager *m_wsMan;

private slots:
    void onChannelCreated(ContactInfo *contactInfo);
    void onChannelAnswered(ContactInfo *contactInfo);
    void onChannelDestroyed(ContactInfo *contactInfo);

    void saveSettings();
    void loadSettings();

    void timeout();
};

#endif // MAINWINDOW_H
