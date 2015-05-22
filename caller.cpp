#include "caller.h"

static const char * const kCallerInfoTemplate = "<html><body><table style='font-size: 2em; vertical-align: bottom;'><tbody>"
                                                "<tr><td><font size=\"-1\">Caller name: </font></td><td><font size=\"+1\">%1</font></td></tr>"
                                                "<tr><td><font size=\"-1\">Caller number: </font></td><td><font size=\"+1\">%2 (%6)</font></td></tr>"
                                          //      "<tr><td><font size=\"-1\">Caller dialed: </font></td><td><font size=\"+1\">%3</font></td></tr>"
                                                "<tr><td><font size=\"-1\">Contact person: </font></td><td><font size=\"+1\">%4</font></td></tr>"
                                                "<tr><td><font size=\"-1\">Login name: </font></td><td><a href=\"https://onnet.su/login?login_name=%5\"><font size=\"+1\">%5</font></a></td></tr>"
                                                "<tr><td><font size=\"-1\">Company: </font></td><td><font size=\"+1\">%7</font></td></tr>"
                                                "<tr><td><font size=\"-1\">Balance: </font></td><td><font size=\"+1\">%8</font></td></tr>"
                                                "<tr><td><font size=\"-1\">Email: </font></td><td><font size=\"+1\">%9</font></td></tr>"
                                                "</tbody></table></body></html>";

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
     //       .arg(m_callerDialed)
            .arg(m_contactPerson)
            .arg(m_login)
            .arg(m_callingNumber)
            .arg(m_companyName)
            .arg(m_balance)
            .arg(m_email);

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

void Caller::setEmail(const QString &email)
{
    m_email = email;
}

QString Caller::email() const
{
    return m_email;
}
