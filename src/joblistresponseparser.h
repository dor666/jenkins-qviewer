#include <QUrl>
#include <QXmlStreamReader>
#include <QDebug>
#include <QColor>
#include <QQmlListProperty>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jParse)

namespace jenkinsQViewer
{

class Job : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool running READ getRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(float buildProgress READ getBuildProgress WRITE setBuildProgress NOTIFY buildProgressChanged)

    QUrl url;
    QColor color = QColor(Qt::gray);
    bool running = false;
    float buildProgress = 0.f;

signals:

    void colorChanged(QColor _value);
    void runningChanged(bool _running);
    void buildProgressChanged(float _progress);

public:

    // For qmlRegisterType
    Job(QObject* _parent = nullptr):
        QObject(_parent)
    {
    }

    Job(QString _name, QObject* _parent = nullptr) :
        QObject(_parent)
    {
        setObjectName(_name);
    }

    void update(const Job& _other)
    {
        setUrl(_other.getUrl());
        setColor(_other.getColor());
        setRunning(_other.getRunning());
    }

    QUrl getUrl() const;
    void setUrl(const QUrl &value);
    QColor getColor() const;
    void setColor(const QColor &value);
    bool getRunning() const;
    void setRunning(bool value);
    float getBuildProgress() const;
    void setBuildProgress(float value);
};

class JoblistResponseParser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<jenkinsQViewer::Job> jobs READ getJobs)

    QXmlStreamReader reader;
    QList<Job*>& jobs;

public:

    static QString runningColorSuffix;

    static bool isRunning(const QString& _color);

    JoblistResponseParser(QList<Job*>& _jobs, QObject* _parent = nullptr);

    QQmlListProperty<Job> getJobs();

signals:

    void newJobAdded(QString _jobName);

    void requestProgress(const QString& _jobName);

public slots:

    void parseJoblistResponse(const QString& _response);

    void parseProgressResponse(const QString& _jobName, const QByteArray& reply);

private:

    ///@todo add objects list
    Job* findOrCreateJob(const QString &_jobName);

    static QColor convertColor(QString _color);



    void readJobElement();

    void skipUnknownElement();

    void readHudsonElement();

    QList<Job *>::iterator findJobByName(const QString &_jobName);
    bool parseFreeStyleBuildProgressResponse(int &progress);
    bool parseExecutorProgressResponse(int& progress);
};

}
