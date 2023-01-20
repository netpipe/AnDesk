#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QFile>

#include <QCommandLineParser>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("voice-over-lan");
    a.setApplicationVersion("0.1");
    a.setQuitOnLastWindowClosed(false);
    QPixmap m( "./Resource/logo.png");

    QCommandLineParser parser;
    parser.setApplicationDescription("Voice over LAN");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("address", QApplication::translate("main", "Address of the counterpart"));

    parser.process(a);
    QString address("");
    if(!parser.positionalArguments().isEmpty())
    {
        address = parser.positionalArguments().at(0);
    }

    MainWindow w;

    QSplashScreen splash(m);
    splash.show();

    splash.finish(&w);

    w.show();
    return a.exec();
}
