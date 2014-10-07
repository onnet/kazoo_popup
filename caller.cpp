#include "caller.h"

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
    return "Caller name: " + m_callerIdName
            + "\nCaller number: " + m_callerIdNumber
            + "\nCaller dialed: " + m_callerDialed;
}
