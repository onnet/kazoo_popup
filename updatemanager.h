#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>

class QTimer;
class QNetworkReply;

class UpdateManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(UpdateManager)

public:
    static UpdateManager *instance();

    void start();
    bool doUpdate() const;
    void quietUpdate();

signals:
    void updateAvailable();
    void noUpdate();

private:
    explicit UpdateManager(QObject *parent = 0);

    void checkUpdate(bool quiet = false);
    void stop();

    static UpdateManager *m_instance;
    QTimer *m_timer;
    QString m_updateUrl;

private slots:
    void processTimeout();

    void onReplyFinished(QNetworkReply *reply);

};

#endif // UPDATEMANAGER_H
