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

#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include <QString>

static const char * const kContactInfoTemplate = "<html><head/><body>Contact person: %1<br>login name: "
        "<a href=\"https://onnet.su:8443/ru/zonnet/login?login_name=%2\">%2</a><br>calling number: "
        "%3<br>company name: %4<br>balance: %5</body></html>";

class ContactInfo
{
public:
    ContactInfo() {}

    void setContactPerson(const QString &contactPerson) { m_contactPerson = contactPerson; }
    QString contactPerson() const { return m_contactPerson; }

    void setLogin(const QString &login) { m_login = login; }
    QString login() const { return m_login; }

    void setCallingNumber(const QString &callingNumber) { m_callingNumber = callingNumber; }
    QString callingNumber() const { return m_callingNumber; }

    void setCompanyName(const QString &companyName) { m_companyName = companyName; }
    QString companyName() const { return m_companyName; }

    void setBalance(double balance) { m_balance = balance; }
    double balance() const { return m_balance; }

    QString toHtml() const
    {
        return QString(kContactInfoTemplate).arg(m_contactPerson, m_login, m_callingNumber, m_companyName).arg(m_balance);
    }

    bool isEmpty() const
    {
        return m_login.isEmpty();
    }

private:
    QString m_contactPerson;
    QString m_login;
    QString m_callingNumber;
    QString m_companyName;
    double m_balance;
};

#endif // CONTACTINFO_H
