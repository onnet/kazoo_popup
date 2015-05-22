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

    void setContactPerson(const QString &contactPerson);
    QString contactPerson() const;

    void setLogin(const QString &login);
    QString login() const;

    void setCallingNumber(const QString &callingNumber);
    QString callingNumber() const;

    void setCompanyName(const QString &companyName);
    QString companyName() const;

    void setBalance(double balance);
    double balance() const;

    void setEmail(const QString &email);
    QString email() const;

private:
    QString m_callerIdName;
    QString m_callerIdNumber;
    QString m_callerDialed;
    QString m_callerUrl;

    QString m_contactPerson;
    QString m_login;
    QString m_callingNumber;
    QString m_companyName;
    double m_balance;
    QString m_email;
};

#endif // CALLER_H
