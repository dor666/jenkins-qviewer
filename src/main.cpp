#include "connection.h"
#include "joblistresponseparser.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

namespace jenkinsQViewer
{
namespace consoleViewer
{
enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

struct Options
{
    QString host;
    QString username;
    QString token;
    QStringList caCertPaths;
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, Options *options, QString *errorMessage)
{
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();
    const QCommandLineOption caCertOption = QCommandLineOption("CAcert",
                QCoreApplication::translate("main", "Path to CA certificate"),
                "path");
    const QCommandLineOption usernameOption = QCommandLineOption("username",
                QCoreApplication::translate("main", "Jenkins username"),
                "string");
    const QCommandLineOption tokenOption = QCommandLineOption("token",
                QCoreApplication::translate("main", "User token, must be specified if 'username' is set."),
                "string");
    bool result = parser.addOptions(QList<QCommandLineOption>{
                                        caCertOption,
                                        usernameOption,
                                        tokenOption
                                    });
    Q_ASSERT(result);

    parser.addPositionalArgument("host", QCoreApplication::translate("main", "Jenkins Host URL with port, ex. http://localhost:8080"));

    if (!parser.parse(QCoreApplication::arguments())) {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }
    if (parser.isSet(versionOption))
        return CommandLineVersionRequested;

    if (parser.isSet(helpOption))
        return CommandLineHelpRequested;

    if(parser.isSet(usernameOption))
    {
        if(!parser.isSet(tokenOption))
        {
            *errorMessage = QObject::tr("'username' option provided, but 'token' option missing.");
            return CommandLineError;
        }
        auto usernames = parser.values(usernameOption);
        auto tokens = parser.values(tokenOption);
        if(usernames.size() != 1 || tokens.size() != 1)
        {
            *errorMessage = QObject::tr("Specify one username and token value.");
            return CommandLineError;
        }
        options->username = usernames.at(0);
        options->token = tokens.at(0);
    }

    if (parser.isSet(caCertOption))
    {
        options->caCertPaths = parser.values(caCertOption);
    }

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.isEmpty()) {
        *errorMessage = QObject::tr("Argument 'host' missing.");
        return CommandLineError;
    }
    else if (positionalArguments.size() > 1) {
        *errorMessage = QObject::tr("Several 'host' arguments specified.");
        return CommandLineError;
    }
    else
    {
        options->host = positionalArguments.first();
        return CommandLineOk;
    }
}

int startConsoleMonitor(QGuiApplication &app, const Options& _options)
{
    QList<Job*> jobs;
    JoblistResponseParser parser(jobs);

    QNetworkAccessManager manager;
    Connection connection(&manager, _options.username, _options.token);
    connection.setJenkinsRootUrl(_options.host);
    connection.addCaCerts(_options.caCertPaths);

    QObject::connect(&connection, &Connection::gotReply,
                     &parser, &JoblistResponseParser::parseJoblistResponse);

    QObject::connect(&parser, &JoblistResponseParser::requestProgress,
                     &connection, &Connection::requestJobProgress);

    QObject::connect(&connection, &Connection::progressResponse,
                     &parser, &JoblistResponseParser::parseProgressResponse);

    QTimer sendTimer;
    sendTimer.setSingleShot(false);
    sendTimer.start(1000);
    QObject::connect(&sendTimer, &QTimer::timeout,
                     &connection, &Connection::sendJoblistRequest);

    QQmlApplicationEngine engine;
    qmlRegisterType<Job>("com.jenkins-qwiever", 1, 0, "Job");
    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("parser", &parser);

    engine.load(QUrl(QStringLiteral("qrc:///Main.qml")));

    return app.exec();
}

}  // namespace consoleViewer
}  // namespace jenkinsQViewer

using namespace jenkinsQViewer;
using namespace jenkinsQViewer::consoleViewer;

int main(int _argc, char** _argv)
{
    qInfo() << "Starting\n";

    QGuiApplication app(_argc, _argv);
    QCoreApplication::setApplicationName("jenkins-qviewer");
    QCoreApplication::setApplicationVersion("0.1"); ///@todo

    QCommandLineParser cmdParser;
    cmdParser.setApplicationDescription("Jenkins qviewer");
    Options options;
    QString errorMessage;

    QTextStream terminal( stdout );

    CommandLineParseResult parseResult = parseCommandLine(cmdParser, &options, &errorMessage);
    switch(parseResult)
    {
    case CommandLineOk:
        return startConsoleMonitor(app, options);
    case CommandLineError:
        qCritical().noquote() << errorMessage;
        cmdParser.showHelp(1);
        Q_UNREACHABLE();
    case CommandLineVersionRequested:
        terminal << QCoreApplication::applicationName() << " " <<
                    QCoreApplication::applicationVersion() << "\n";
        return 0;
    case CommandLineHelpRequested:
        cmdParser.showHelp();
        Q_UNREACHABLE();
    }
    Q_UNREACHABLE();
}
