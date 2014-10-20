#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "defaults.h"

#include "caller.h"
#include "informerdialog.h"
#include "websocketmanager.h"

#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QSettings>
#include <QDesktopServices>
#include <QTimer>
#include <QDir>

#ifdef Q_OS_MAC
#   include <QTemporaryFile>
#   include <QProcess>
#endif

enum OpenUrl
{
    kOpenUrlOff,
    kOpenUrlCreateChannel,
    kOpenUrlAnswerChannel
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/res/kazoo_32.png"));

    createTrayIcon();

    loadSettings();

    m_wsMan = new WebSocketManager(this);
    connect(m_wsMan, &WebSocketManager::channelCreated,
            this, &MainWindow::onChannelCreated);
    connect(m_wsMan, &WebSocketManager::channelAnswered,
            this, &MainWindow::onChannelAnswered);
    connect(m_wsMan, &WebSocketManager::channelAnsweredAnother,
            this, &MainWindow::onChannelAnsweredAnother);
    connect(m_wsMan, &WebSocketManager::channelDestroyed,
            this, &MainWindow::onChannelDestroyed);
    connect(m_wsMan, &WebSocketManager::connected,
            this, &MainWindow::handleWsConnected);
    connect(m_wsMan, &WebSocketManager::connectionError,
            this, &MainWindow::handleWsConnectionError);

    connect(ui->cancelPushButton, &QPushButton::clicked,
            this, &MainWindow::close);
    connect(ui->okPushButton, &QPushButton::clicked,
            this, &MainWindow::saveSettings);

    m_wsMan->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(QIcon(":/res/kazoo_32_disabled.png"), this);
    m_trayIcon->setToolTip(tr("Kazoo Popup - Connecting"));

    QMenu *menu = new QMenu(this);
    QAction *stateAction = menu->addAction(tr("Connecting"));
    stateAction->setDisabled(true);
    menu->addSeparator();
    menu->addAction(tr("Settings"), this, SLOT(show()));
    menu->addAction(tr("Close all popups"), this, SLOT(closeAllPopups()));
    menu->addSeparator();
    menu->addAction(tr("Quit"), this, SLOT(quit()));
    m_trayIcon->setContextMenu(menu);

    m_trayIcon->show();
}

void MainWindow::onChannelCreated(const QString &callId, const Caller &caller)
{
    InformerDialog *informerDialog = new InformerDialog();

    connect(informerDialog, &InformerDialog::finished,
            this, &MainWindow::processDialogFinished);
    connect(informerDialog, &InformerDialog::dialogAttached,
            this, &MainWindow::processDialogAttached);

    informerDialog->setCaller(caller);
    informerDialog->adjustSize();
    QRect rect = qApp->desktop()->availableGeometry();
    informerDialog->setGeometry(rect.width() - informerDialog->width(),
                                rect.height() - informerDialog->height(),
                                informerDialog->width(),
                                informerDialog->height());
    informerDialog->show();

    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout,
            this, &MainWindow::timeout);
    timer->setSingleShot(true);
    m_timersHash.insert(callId, timer);
    timer->start(ui->popupTimeoutSpinBox->value() * 1000);

    m_informerDialogsHash.insert(callId, informerDialog);

    if (ui->openUrlComboBox->currentIndex() == kOpenUrlCreateChannel)
        QDesktopServices::openUrl(QUrl(caller.callerUrl()));
}

void MainWindow::onChannelAnswered(const QString &callId)
{
    if (!m_informerDialogsHash.contains(callId))
        return;

    InformerDialog *informerDialog = m_informerDialogsHash.value(callId);
    if (informerDialog->isVisible())
        informerDialog->setState(InformerDialog::kStateAnswered);

    if (m_timersHash.contains(callId))
    {
        QTimer *timer = m_timersHash.value(callId);
        timer->start();
    }

    if (ui->openUrlComboBox->currentIndex() == kOpenUrlAnswerChannel)
        informerDialog->openCallerUrl();
}

void MainWindow::onChannelAnsweredAnother(const QString &callId,
                                          const QString &calleeNumber,
                                          const QString &calleeName)
{
    if (!m_informerDialogsHash.contains(callId))
        return;

    InformerDialog *informerDialog = m_informerDialogsHash.value(callId);
    if (informerDialog->isVisible())
    {
        informerDialog->setCallee(calleeNumber, calleeName);
        informerDialog->setState(InformerDialog::kStateAnsweredAnother);
    }

    if (m_timersHash.contains(callId))
    {
        QTimer *timer = m_timersHash.value(callId);
        timer->start();
    }
}

void MainWindow::timeout()
{
    QTimer *timer = qobject_cast<QTimer*>(sender());

    if (timer == nullptr)
        return;

    QString callId = m_timersHash.key(timer);

    if (!m_informerDialogsHash.contains(callId))
        return;

    InformerDialog *informerDialog = m_informerDialogsHash.value(callId);
    if (informerDialog->isVisible())
        informerDialog->close();

    m_informerDialogsHash.remove(callId);
    informerDialog->deleteLater();
}

void MainWindow::onChannelDestroyed(const QString &callId)
{
    if (!m_informerDialogsHash.contains(callId))
        return;

    InformerDialog *informerDialog = m_informerDialogsHash.value(callId);
    if (informerDialog->isVisible() && !informerDialog->isAttached())
        informerDialog->close();

    m_informerDialogsHash.remove(callId);
    informerDialog->deleteLater();

    if (m_timersHash.contains(callId))
    {
        QTimer *timer = m_timersHash.value(callId);
        m_timersHash.remove(callId);
        timer->stop();
        timer->deleteLater();
    }
}

bool MainWindow::isCorrectSettings() const
{
    bool ok = true;
    ok &= !ui->loginLineEdit->text().isEmpty();
    if (!ok)
        return false;

    ok &= !ui->passwordLineEdit->text().isEmpty();
    if (!ok)
        return false;

    ok &= !ui->realmLineEdit->text().isEmpty();
    if (!ok)
        return false;

    ok &= !ui->authUrlLineEdit->text().isEmpty();
    if (!ok)
        return false;

    ok &= !ui->eventUrlLineEdit->text().isEmpty();
    if (!ok)
        return false;

    ok &= !ui->infoUrlLineEdit->text().isEmpty();
    if (!ok)
        return false;

    return ok;
}

void setRunAtStartup()
{
#ifdef Q_OS_WIN
    QSettings settings(kRegistryKeyRun, QSettings::NativeFormat);
    if (settings.contains(qApp->applicationName()))
        return;

    QString appExePath = QString("%1/%2.exe").arg(qApp->applicationDirPath(), qApp->applicationName());
    QString appExeNativePath = QDir::toNativeSeparators(appExePath);
    settings.setValue(qApp->applicationName(), appExeNativePath);
#elif defined Q_OS_MAC
    if (QFile::exists("~/Library/LaunchAgents/KazooPopup.restart.plist"))
            return;

    QFile plistTemplateFile(":/res/mac/KazooPopup.restart.plist");
    bool ok = plistTemplateFile.open(QIODevice::ReadOnly);
    QByteArray data = plistTemplateFile.readAll();
    plistTemplateFile.close();
    data.replace("/path/to/app", qApp->applicationDirPath().toLocal8Bit());
    QTemporaryFile plistFile;
    ok = plistFile.open();
    plistFile.write(data);
    plistFile.close();
    QString fileName = plistFile.fileName();

    QProcess::startDetached("sh", QStringList() << "-c" << "cp " + fileName + " ~/Library/LaunchAgents/KazooPopup.restart.plist");
    QProcess::startDetached("sh", QStringList() << "-c" << "launchctl load -w ~/Library/LaunchAgents/KazooPopup.restart.plist");
#endif
}

void unsetRunAtStartup()
{
#ifdef Q_OS_WIN
    QSettings settings(kRegistryKeyRun, QSettings::NativeFormat);
    if (!settings.contains(qApp->applicationName()))
        return;

    settings.remove(qApp->applicationName());
#elif defined Q_OS_MAC
    if (!QFile::exists("~/Library/LaunchAgents/KazooPopup.restart.plist"))
            return;

    QProcess::startDetached("launchctl unload -w ~/Library/LaunchAgents/KazooPopup.restart.plist");
    QFile::remove("~/Library/LaunchAgents/KazooPopup.restart.plist");
#endif
}

void MainWindow::saveSettings()
{
    if (!isCorrectSettings())
    {
        QMessageBox::warning(this, qApp->applicationName(), tr("All fields must be filled!"));
        return;
    }

    QSettings settings(dataDirPath() + "/settings.ini", QSettings::IniFormat);
    settings.setValue("login", ui->loginLineEdit->text());
    settings.setValue("password", ui->passwordLineEdit->text());
    settings.setValue("realm", ui->realmLineEdit->text());
    settings.setValue("auth_url", ui->authUrlLineEdit->text());
    settings.setValue("crossbar_url", ui->crossbarUrlLineEdit->text());
    settings.setValue("event_url", ui->eventUrlLineEdit->text());
    settings.setValue("info_url", ui->infoUrlLineEdit->text());
    settings.setValue("popup_timeout", ui->popupTimeoutSpinBox->value());
    settings.setValue("call_direction", ui->callDirectionComboBox->currentIndex());
    settings.setValue("open_url", ui->openUrlComboBox->currentIndex());
    settings.setValue("run_at_startup", ui->runAtStartupCheckBox->isChecked());

    if (ui->runAtStartupCheckBox->isChecked())
        setRunAtStartup();
    else
        unsetRunAtStartup();

    m_wsMan->start();
    close();
}

void MainWindow::loadSettings()
{
    QSettings settings(dataDirPath() + "/settings.ini", QSettings::IniFormat);
    ui->loginLineEdit->setText(settings.value("login", kLogin).toString());
    ui->passwordLineEdit->setText(settings.value("password", kPassword).toString());
    ui->realmLineEdit->setText(settings.value("realm", kRealm).toString());
    ui->authUrlLineEdit->setText(settings.value("auth_url", kAuthUrl).toString());
    ui->crossbarUrlLineEdit->setText(settings.value("crossbar_url", kCrossbarUrl).toString());
    ui->eventUrlLineEdit->setText(settings.value("event_url", kEventUrl).toString());
    ui->infoUrlLineEdit->setText(settings.value("info_url", kInfoUrl).toString());
    ui->popupTimeoutSpinBox->setValue(settings.value("popup_timeout", kPopupTimeout).toInt());
    ui->callDirectionComboBox->setCurrentIndex(settings.value("call_direction", kCallDirection).toInt());
    ui->openUrlComboBox->setCurrentIndex(settings.value("open_url", kOpenUrl).toInt());
    ui->runAtStartupCheckBox->setChecked(settings.value("run_at_startup", kRunAtStartup).toBool());

    if (ui->runAtStartupCheckBox->isChecked())
        setRunAtStartup();
    else
        unsetRunAtStartup();
}

void MainWindow::processDialogFinished()
{
    InformerDialog *informerDialog = qobject_cast<InformerDialog*>(sender());

    if (m_informerDialogsHash.values().contains(informerDialog))
    {
        QString callId = m_informerDialogsHash.key(informerDialog);
        m_informerDialogsHash.remove(callId);
    }
    else if (m_attachedDialogsHash.values().contains(informerDialog))
    {
        QString callId = m_attachedDialogsHash.key(informerDialog);
        m_attachedDialogsHash.remove(callId);
    }
}

void MainWindow::processDialogAttached(bool attached)
{
    InformerDialog *informerDialog = qobject_cast<InformerDialog*>(sender());
    if (attached)
    {
        QString callId = m_informerDialogsHash.key(informerDialog);
        m_attachedDialogsHash.insert(callId, informerDialog);
        m_informerDialogsHash.remove(callId);
    }
    else
    {
        QString callId = m_attachedDialogsHash.key(informerDialog);
        m_attachedDialogsHash.remove(callId);
        m_informerDialogsHash.insert(callId, informerDialog);
    }
}

void MainWindow::closeAllPopups()
{
    foreach (InformerDialog* informerDialog, m_informerDialogsHash)
    {
        informerDialog->close();
        informerDialog->deleteLater();
    }
    m_informerDialogsHash.clear();

    foreach (InformerDialog* informerDialog, m_attachedDialogsHash)
    {
        informerDialog->close();
        informerDialog->deleteLater();
    }
    m_attachedDialogsHash.clear();
}

void MainWindow::quit()
{
    int result = QMessageBox::question(this,
                                       qApp->applicationName(),
                                       tr("Do you really want to quit?"),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
    if (result != QMessageBox::Yes)
        return;

    QTimer::singleShot(0, qApp, SLOT(quit()));
}

void MainWindow::handleWsConnected()
{
    m_trayIcon->setIcon(QIcon(":/res/kazoo_32.png"));
    m_trayIcon->setToolTip(tr("Kazoo Popup - Connected"));
    QMenu *menu = m_trayIcon->contextMenu();
    QList<QAction*> actions = menu->actions();
    QAction *stateAction = actions.first();
    stateAction->setText(tr("Connected"));
    QAction *action = actions.at(2);
    QAction *separator = actions.at(3);
    if (action->text() != tr("Try reconnect..."))
        return;

    menu->removeAction(action);
    menu->removeAction(separator);
    m_trayIcon->showMessage(qApp->applicationName(),
                            tr("Connection established"));
}

void MainWindow::handleWsConnectionError()
{
    m_trayIcon->setIcon(QIcon(":/res/kazoo_32_error.png"));
    m_trayIcon->setToolTip(tr("Kazoo Popup - Cannot establish connection"));
    m_trayIcon->showMessage(qApp->applicationName(),
                            tr("Cannot establish connection"),
                            QSystemTrayIcon::Warning);

    QMenu *menu = m_trayIcon->contextMenu();
    QAction *stateAction = menu->actions().first();
    stateAction->setText(tr("Connection error"));
    QAction *action = menu->actions().at(2);
    if (action->text() == tr("Try reconnect..."))
        return;

    QAction *reconnectAction = new QAction(tr("Try reconnect..."), this);
    connect(reconnectAction, &QAction::triggered,
            m_wsMan, &WebSocketManager::start);
    menu->insertAction(action, reconnectAction);
    menu->insertSeparator(action);
}
