#include "connection.h"
#include "joblistresponseparser.h"

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslSocket>
#include <QSslSocket>
#include <QSslCipher>

Q_LOGGING_CATEGORY(jConn, "jenkinsQViewer.connection", QtWarningMsg)

namespace jenkinsQViewer
{

QUrl Connection::getJenkinsRootUrl() const
{
    return jenkinsRootUrl;
}

void Connection::setJenkinsRootUrl(const QUrl &value)
{
    jenkinsRootUrl = value;
}

QString Connection::getUsername() const
{
    return username;
}

void Connection::setUsername(const QString &value)
{
    username = value;
}

QString Connection::getToken() const
{
    return token;
}

void Connection::setToken(const QString &value)
{
    token = value;
}

Connection::Connection(QNetworkAccessManager *_networkManager,
                       QString _username, QString _token,
                       QObject* _parent):
    QObject(_parent),
    networkManager(_networkManager)
{
    setToken(_token);
    setUsername(_username);
    createSslConfiguration();
}

void Connection::createSslConfiguration()
{
    sslConfiguration = QSslConfiguration();

    qCDebug(jConn) << "SSL enabled: " << QSslSocket::supportsSsl();

    QList<QSslCertificate> allCerts;
    for(const QString& certPath : caCerts)
    {
        QList<QSslCertificate> certsInFile = QSslCertificate::fromPath(certPath);
        if(certsInFile.empty())
            qCWarning(jConn) << "No certs found in path: " << certPath;

        allCerts.append(certsInFile);
    }

    sslConfiguration.setCaCertificates(allCerts);

    expectedSslErrors.clear();
    for(const QSslCertificate& cert : allCerts) {
        qCWarning(jConn) << "is cert null?" << cert.isNull()
                         << "\nis cert self signed?" << cert.isSelfSigned();

        expectedSslErrors <<
                             QSslError(QSslError::SelfSignedCertificate, cert) <<
                             QSslError(QSslError::CertificateUntrusted, cert);
    }
}

bool Connection::checkReply(QNetworkReply* reply)
{
    if(reply){
        if ( reply->error() != QNetworkReply::NoError ) {
            qCDebug(jConn) << "Network error!!!";
            qCWarning(jConn) <<"ErrorNo: "<< reply->error() << "for url: " << reply->url().toString();
            qCDebug(jConn) << "Request failed, " << reply->errorString()
                           << "\nHeaders:"<<  reply->rawHeaderList()
                           << "\ncontent:" << reply->readAll();
            return false;
        }
        else
        {
            return true;
        }
    }
}

void Connection::joblistReplyFinished()
{
    QNetworkReply* reply =
            qobject_cast<QNetworkReply*>(QObject::sender());

    if(checkReply(reply))
    {
        QByteArray responseContent = reply->readAll();
        qCDebug(jConn) << "Got response: " << responseContent;
        emit gotReply(responseContent);
    }
    reply->deleteLater();
}

void Connection::progressReplyFinished()
{
    QNetworkReply* reply =
            qobject_cast<QNetworkReply*>(QObject::sender());

    QString jobname = jobnamesByProgressReply.take(reply);
    if(!jobname.isEmpty()){
        emit progressResponse(jobname, reply->readAll());
    }
    else
    {
        qCWarning(jConn) << "No jobname for progress reply";
    }

    reply->deleteLater();
}

void Connection::sendJoblistRequest()
{
    QNetworkRequest request =
            createNetworkRequest(QStringLiteral("/api/xml"));

    reply = networkManager->get(request);
    reply->setSslConfiguration(sslConfiguration);
    reply->ignoreSslErrors(expectedSslErrors); // Not working?

    connect(reply, &QNetworkReply::encrypted, this, &Connection::ready);
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
    connect(reply, &QNetworkReply::finished,
            this, &Connection::joblistReplyFinished);
}

void Connection::sslErrors(QList<QSslError> _errors)
{
    for(const QSslError& err : _errors)
    {
        qCWarning(jConn) << "SslError: " << err.errorString();
    }
    reply->ignoreSslErrors(); //@todo fix
}

void Connection::ready()
{
    qCDebug(jConn) << " === Peer Certificate ===";
    QSslCertificate cert = reply->sslConfiguration().peerCertificate();
    dumpCertificate( cert );

    QSslCipher cipher = reply->sslConfiguration().sessionCipher();
    dumpCipher( cipher );

    qCDebug(jConn) << "Done";
}

void Connection::requestJobProgress(const QString &_jobName)
{
    QNetworkRequest request =
            createNetworkRequest(QStringLiteral("/job/%1/lastBuild/api/xml?depth=1&xpath=*/executor/progress/text()")
                                 .arg(_jobName));

    reply = networkManager->get(request);
    reply->setSslConfiguration(sslConfiguration);
    reply->ignoreSslErrors(expectedSslErrors); // Not working?

    connect(reply, &QNetworkReply::encrypted, this, &Connection::ready);
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
    connect(reply, &QNetworkReply::finished,
            this, &Connection::progressReplyFinished);

    jobnamesByProgressReply[reply] = _jobName;
}

QNetworkRequest Connection::createNetworkRequest(const QString& _page)
{
    if(jenkinsRootUrl.isEmpty())
    {
        qCritical() << "Trying to send request with empty root url";
    }

    QUrl url = getJenkinsRootUrl().toString() + _page;
    QNetworkRequest request(url);

    QString concatenated = QStringLiteral("%1:%2").arg(username, token);
    QString headerData = QStringLiteral("Basic ") +
            concatenated.toLocal8Bit().toBase64();
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    return request;
}

void Connection::dumpCipher(const QSslCipher& _cipher)
{
    qCDebug(jConn) << "\n== Cipher =="
                   << "\nAuthentication:\t\t" << _cipher.authenticationMethod()
                   << "\nEncryption:\t\t" << _cipher.encryptionMethod()
                   << "\nKey Exchange:\t\t" << _cipher.keyExchangeMethod()
                   << "\n_cipher Name:\t\t" << _cipher.name()
                   << "\nProtocol:\t\t" <<  _cipher.protocolString()
                   << "\nSupported Bits:\t\t" << _cipher.supportedBits()
                   << "\nUsed Bits:\t\t" << _cipher.usedBits();
}

void Connection::dumpCertificate( const QSslCertificate& _cert )
{
    qCDebug(jConn) << _cert.toPem()
                   << "\n== Subject Info ==\b"
                   << "\nCommonName:\t\t" << _cert.subjectInfo( QSslCertificate::CommonName )
                   << "\nOrganization:\t\t" << _cert.subjectInfo( QSslCertificate::Organization )
                   << "\nLocalityName:\t\t" << _cert.subjectInfo( QSslCertificate::LocalityName )
                   << "\nOrganizationalUnitName:\t" << _cert.subjectInfo( QSslCertificate::OrganizationalUnitName )
                   << "\nStateOrProvinceName:\t" << _cert.subjectInfo( QSslCertificate::StateOrProvinceName );

    QMultiMap<QSsl::AlternativeNameEntryType, QString> altNames = _cert.subjectAlternativeNames();
    if ( !altNames.isEmpty() ) {
        qCDebug(jConn) << "Alternate Subject Names (DNS):";
        foreach (const QString &altName, altNames.values(QSsl::DnsEntry)) {
            qCDebug(jConn) << altName;
        }

        qCDebug(jConn) << "Alternate Subject Names (Email):";
        foreach (const QString &altName, altNames.values(QSsl::EmailEntry)) {
            qCDebug(jConn) << altName;
        }
    }

    qCDebug(jConn) << "\n== Issuer Info =="
                   << "\nCommonName:\t\t" << _cert.issuerInfo( QSslCertificate::CommonName )
                   << "\nOrganization:\t\t" << _cert.issuerInfo( QSslCertificate::Organization )
                   << "\nLocalityName:\t\t" << _cert.issuerInfo( QSslCertificate::LocalityName )
                   << "\nOrganizationalUnitName:\t" << _cert.issuerInfo( QSslCertificate::OrganizationalUnitName )
                   << "\nStateOrProvinceName:\t" << _cert.issuerInfo( QSslCertificate::StateOrProvinceName )
                   << "\n== Certificate =="
                   << "\nEffective Date:\t\t" << _cert.effectiveDate().toString()
                   << "\nExpiry Date:\t\t" << _cert.expiryDate().toString();
}

}
