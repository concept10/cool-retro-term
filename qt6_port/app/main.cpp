
#include <QtQml/QQmlApplicationEngine>
#include <QtGui/QGuiApplication>
#include <QQmlContext>
#include <QStringList>
#include <QtWidgets/QApplication>
#include <QIcon>
#include <QQuickStyle>
#include <QDebug>
#include <stdlib.h>
#include <QFontDatabase>
#include <QLoggingCategory>

#include "fileio.h"
#include "monospacefontmanager.h"

QString getNamedArgument(QStringList args, QString name, QString defaultName)
{
    int index = args.indexOf(name);
    return (index != -1) ? args[index + 1] : QString(defaultName);
}

QString getNamedArgument(QStringList args, QString name)
{
    return getNamedArgument(args, name, "");
}

int main(int argc, char *argv[])
{
    setenv("QT_QPA_PLATFORMTHEME", "", 1);
    QLoggingCategory::setFilterRules("qt.qml.connections.warning=false");

#if defined (Q_OS_LINUX)
    setenv("QSG_RENDER_LOOP", "threaded", 0);
#endif

#if defined(Q_OS_MAC)
    setenv("LC_CTYPE", "UTF-8", 1);
#endif

    if (argc>1 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help"))) {
        QTextStream cout(stdout, QIODevice::WriteOnly);
        cout << "Usage: " << argv[0] << " [--default-settings] [--workdir <dir>] [--program <prog>] [-p|--profile <prof>] [--fullscreen] [-h|--help]" << endl;
        cout << "  --default-settings  Run cool-retro-term with the default settings" << endl;
        cout << "  --workdir <dir>     Change working directory to 'dir'" << endl;
        cout << "  -e <cmd>            Command to execute" << endl;
        cout << "  -T <title>          Set window title to 'title'" << endl;
        cout << "  --fullscreen        Run cool-retro-term in fullscreen" << endl;
        cout << "  -p|--profile <prof> Run cool-retro-term with the given profile" << endl;
        cout << "  -h|--help           Print this help" << endl;
        cout << "  --verbose           Print additional information" << endl;
        return 0;
    }

    QString appVersion("2.0.0");

    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.setAttribute(Qt::AA_EnableHighDpiScaling);

    QQmlApplicationEngine engine;
    FileIO fileIO;
    MonospaceFontManager monospaceFontManager;

    app.setWindowIcon(QIcon(":../icons/32x32/cool-retro-term.png"));
    app.setOrganizationName("cool-retro-term");
    app.setOrganizationDomain("cool-retro-term");

    QStringList args = app.arguments();
    QStringList cmdList;
    if (args.contains("-e")) {
        cmdList << args.mid(args.indexOf("-e") + 1);
    }
    QVariant command(cmdList.empty() ? QVariant() : cmdList[0]);
    QVariant commandArgs(cmdList.size() <= 1 ? QVariant() : QVariant(cmdList.mid(1)));

    engine.rootContext()->setContextProperty("appVersion", appVersion);
    engine.rootContext()->setContextProperty("defaultCmd", command);
    engine.rootContext()->setContextProperty("defaultCmdArgs", commandArgs);
    engine.rootContext()->setContextProperty("workdir", getNamedArgument(args, "--workdir", "$HOME"));
    engine.rootContext()->setContextProperty("fileIO", &fileIO);
    engine.rootContext()->setContextProperty("monospaceSystemFonts", monospaceFontManager.retrieveMonospaceFonts());
    engine.rootContext()->setContextProperty("devicePixelRatio", app.devicePixelRatio());

    QStringList importPathList = engine.importPathList();
    importPathList.prepend(QCoreApplication::applicationDirPath() + "/qmltermwidget");
    importPathList.prepend(QCoreApplication::applicationDirPath() + "/../PlugIns");
    importPathList.prepend(QCoreApplication::applicationDirPath() + "/../../../qmltermwidget");
    engine.setImportPathList(importPathList);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        qDebug() << "Cannot load QML interface";
        return EXIT_FAILURE;
    }

    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QApplication::quit);

    return app.exec();
}
