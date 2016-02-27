#include "joblistresponseparser.h"

#include <QtQml>

Q_LOGGING_CATEGORY(jRParser, "jenkinsQViewer.responseParser", QtWarningMsg)

void initResources()
{
    Q_INIT_RESOURCE(qml);
    ///@todo
    qmlRegisterType<jenkinsQViewer::Job>();//"jenkinsQViewer", 1,0, "Job");
}

namespace jenkinsQViewer {

QString JoblistResponseParser::runningColorSuffix =
        QStringLiteral("_anime");

bool Job::getRunning() const
{
    return running;
}

void Job::setRunning(bool value)
{
    if(running != value){
        running = value;
        emit runningChanged(running);

        if(!running)
            setBuildProgress(0.f);
    }
}

float Job::getBuildProgress() const
{
    return buildProgress;
}

void Job::setBuildProgress(float value)
{
    if(buildProgress != value){
        buildProgress = value;
        emit buildProgressChanged(buildProgress);
    }
}

QUrl Job::getUrl() const
{
    return url;
}

void Job::setUrl(const QUrl &value)
{
    url = value;
}

QColor Job::getColor() const
{
    return color;
}

void Job::setColor(const QColor &value)
{
    if(color != value){
        color = value;
        emit colorChanged(color);
    }
}

JoblistResponseParser::JoblistResponseParser(QList<Job *> &_jobs, QObject *_parent):
    QObject(_parent),
    jobs(_jobs)
{
    initResources();
}

QQmlListProperty<Job> JoblistResponseParser::getJobs()
{
    ///@todo This constructor should not be used in production code
    return QQmlListProperty<Job>(this, jobs);
}

void JoblistResponseParser::parseJoblistResponse(const QString &_response)
{
    reader.clear();
    reader.addData(_response);

    while (!reader.atEnd()) {
        qCInfo(jRParser) << reader.name();
        if(reader.isStartElement()){
            if(reader.name() == QStringLiteral("hudson")){
                readHudsonElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}

void JoblistResponseParser::parseProgressResponse(
        const QString& _jobName, const QByteArray& reply)
{
    QList<Job*>::iterator job = findJobByName(_jobName);
    bool responseOk;
    int progress = reply.toInt(&responseOk);
    if(responseOk && job != jobs.end()){
        (*job)->setBuildProgress((float)progress/100.f);
    }
}

QList<Job*>::iterator JoblistResponseParser::findJobByName(const QString &_jobName)
{
    return std::find_if(jobs.begin(), jobs.end(),
                         [&_jobName](const Job* _j)
    {
        return _jobName == _j->objectName();
    });
}

Job* JoblistResponseParser::findOrCreateJob(const QString &_jobName)
{
    QList<Job*>::iterator job = findJobByName(_jobName);

    if(job == jobs.end())
    {
        qCDebug(jRParser) << "Creating new job" << _jobName;
        jobs.push_back(new Job(_jobName));
        job = std::next(jobs.end(), -1);
        emit newJobAdded(_jobName);
    }

    return *job;
}

QColor JoblistResponseParser::convertColor(QString _color)
{
    ///@todo Could be optimized with strinref?
    if(_color.endsWith(runningColorSuffix)){
        _color.remove(runningColorSuffix);
    }

    if(_color == QStringLiteral("blue"))
        return Qt::darkGreen;
    else if(_color == QStringLiteral("red"))
        return Qt::darkRed;
    else if(_color == QStringLiteral("disabled"))
        return Qt::lightGray;
    else {
        qCInfo(jRParser) << "Unrecognized color parsed: " << _color;
        return Qt::darkGray;
    }
}

bool JoblistResponseParser::isRunning(const QString &_color){
    return _color.contains(runningColorSuffix);
}

void JoblistResponseParser::readJobElement()
{
    Job* job = nullptr;
    reader.readNext();
    while (!reader.atEnd()) {
        if(reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if(reader.isStartElement())
        {
            QStringRef elementName = reader.name();
            QString text = reader.readElementText(QXmlStreamReader::SkipChildElements);
            if(elementName == "name")
            {
                qCInfo(jRParser) << "Job name" << text;
                job = findOrCreateJob(text);
            }
            else if(job && elementName == "url")
            {
                qCInfo(jRParser) << "Job url" << text;
                job->setUrl(text);
            }
            else if(job && elementName == "color")
            {
                qCInfo(jRParser) << "Job color" << text;
                job->setColor(convertColor(text));
                bool running = isRunning(text);
                job->setRunning(running);
                if(running)
                    emit requestProgress(job->objectName());
            }

            if (reader.isEndElement())
                reader.readNext();
        }
        else
        {
            reader.readNext();
        }
    }
}

void JoblistResponseParser::skipUnknownElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            skipUnknownElement();
        } else {
            reader.readNext();
        }
    }
}

void JoblistResponseParser::readHudsonElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if(reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if(reader.isStartElement())
        {
            if(reader.name() == "job")
            {
                readJobElement();
            }
            else
            {
                skipUnknownElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}

} // namespace jenkinsQViewer
