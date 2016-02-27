#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QUrl>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jConn)

namespace jenkinsQViewer
{

class Connection : public QObject
{
    Q_OBJECT

    QNetworkAccessManager* const networkManager;
    QSslConfiguration sslConfiguration;
    QList<QSslError> expectedSslErrors;
    QString username;
    QString token;
    QUrl jenkinsRootUrl;
    QNetworkReply* reply = nullptr;
    QStringList caCerts;
    QMap<QNetworkReply*, QString> jobnamesByProgressReply;

public:

    Connection(QNetworkAccessManager* _networkManager,
               QString _username, QString _token,
               QObject *_parent = nullptr);

    QUrl getJenkinsRootUrl() const;
    void setJenkinsRootUrl(const QUrl &value);

    QString getUsername() const;
    void setUsername(const QString &value);

    QString getToken() const;
    void setToken(const QString &value);

    void addCaCerts(const QStringList& _caCerts)
    {
        caCerts.append(_caCerts);
    }

public slots:

    void joblistReplyFinished();

    void progressReplyFinished();

    void sendJoblistRequest();

    void sslErrors(QList<QSslError> _errors);

    void ready();

    void requestJobProgress(const QString& _jobName);

signals:

    void gotReply(const QByteArray& reply);

    void progressResponse(const QString& job, const QByteArray& reply);

private:

    QNetworkRequest createNetworkRequest(const QString& _page);
    void createSslConfiguration();
    bool checkReply(QNetworkReply* reply);

    void dumpCipher(const QSslCipher& _cipher);
    void dumpCertificate(const QSslCertificate& _cert);
};

}
