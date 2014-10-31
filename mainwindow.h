#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QSystemTrayIcon;

class WebSocketManager;
class InformerDialog;
class Caller;
class DebugDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void createTrayIcon();
    bool isCorrectSettings() const;

    Ui::MainWindow *ui;
    QSystemTrayIcon *m_trayIcon;

    QHash<QString, InformerDialog*> m_informerDialogsHash;
    QHash<QString, InformerDialog*> m_attachedDialogsHash;
    QHash<QString, QTimer*> m_timersHash;

    WebSocketManager *m_wsMan;

    DebugDialog *m_debugDialog = nullptr;

private slots:
    void onChannelCreated(const QString &callId, const Caller &caller);
    void onChannelAnswered(const QString &callId);
    void onChannelAnsweredAnother(const QString &callId, const QString &calleeNumber, const QString &calleeName);
    void onChannelDestroyed(const QString &callId);
    void handleWsConnectionError();
    void handleWsConnected();

    void saveSettings();
    void loadSettings();

    void timeout();

    void processDialogFinished();
    void processDialogAttached(bool attached);

    void showDebugDialog();
    void closeAllPopups();
    void quit();
};

#endif // MAINWINDOW_H
