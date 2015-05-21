#include "caller.h"

static const char * const kCallerInfoTemplate = "<html><body>"
                                                "Caller name: %1<br>"
                                                "Caller number: %2<br>"
                                                "Caller dialed: %3<br>"
                                                "Contact person: %4<br>"
                                                "Login name: <a href=\"https://onnet.su:8443/ru/zonnet/login?login_name=%5\">%5</a><br>"
                                                "Calling number: %6<br>"
                                                "Company name: %7<br>"
                                                "Balance: %8"
                                                "</body></html>";

Caller::Caller()
{
}

Caller::Caller(const QString &callerIdName,
               const QString &callerIdNumber,
               const QString &callerDialed,
               const QString &callerUrl) :
    m_callerIdName(callerIdName),
    m_callerIdNumber(callerIdNumber),
    m_callerDialed(callerDialed),
    m_callerUrl(callerUrl)
{
}

void Caller::setCallerIdName(const QString &callerIdName)
{
    m_callerIdName = callerIdName;
}

QString Caller::callerIdName() const
{
    return m_callerIdName;
}

void Caller::setCallerIdNumber(const QString &callerIdNumber)
{
    m_callerIdNumber = callerIdNumber;
}

QString Caller::callerIdNumber() const
{
    return m_callerIdNumber;
}

void Caller::setCallerDialed(const QString &callerInfo)
{
    m_callerDialed = callerInfo;
}

QString Caller::callerDialed() const
{
    return m_callerDialed;
}

void Caller::setCallerUrl(const QString &callerUrl)
{
    m_callerUrl = callerUrl;
}

QString Caller::callerUrl() const
{
    return m_callerUrl;
}

QString Caller::callerInfo() const
{
    QString info(kCallerInfoTemplate);
    return info.arg(m_callerIdName)
            .arg(m_callerIdNumber)
            .arg(m_callerDialed)
            .arg(m_contactPerson)
            .arg(m_login)
            .arg(m_callingNumber)
            .arg(m_companyName)
            .arg(m_balance);

//    return "Caller name: " + m_callerIdName
//            + "\nCaller number: " + m_callerIdNumber
//            + "\nCaller dialed: " + m_callerDialed;
}

void Caller::setContactPerson(const QString &contactPerson)
{
    m_contactPerson = contactPerson;
}

QString Caller::contactPerson() const
{
    return m_contactPerson;
}

void Caller::setLogin(const QString &login)
{
    m_login = login;
}

QString Caller::login() const
{
    return m_login;
}

void Caller::setCallingNumber(const QString &callingNumber)
{
    m_callingNumber = callingNumber;
}

QString Caller::callingNumber() const
{
    return m_callingNumber;
}

void Caller::setCompanyName(const QString &companyName)
{
    m_companyName = companyName;
}

QString Caller::companyName() const
{
    return m_companyName;
}

void Caller::setBalance(double balance)
{
    m_balance = balance;
}

double Caller::balance() const
{
    return m_balance;
}
