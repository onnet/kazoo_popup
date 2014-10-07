#ifndef CALLER_H
#define CALLER_H

#include <QString>
#include <QUrl>

class Caller
{
public:
    Caller();
    Caller(const QString &callerIdName,
           const QString &callerIdNumber,
           const QString &callerDialed,
           const QString &callerUrl);

    void setCallerIdName(const QString &callerIdName);
    QString callerIdName() const;

    void setCallerIdNumber(const QString &callerIdNumber);
    QString callerIdNumber() const;

    void setCallerDialed(const QString &callerDialed);
    QString callerDialed() const;

    void setCallerUrl(const QString &callerUrl);
    QString callerUrl() const;

    QString callerInfo() const;

private:
    QString m_callerIdName;
    QString m_callerIdNumber;
    QString m_callerDialed;
    QString m_callerUrl;
};

#endif // CALLER_H
