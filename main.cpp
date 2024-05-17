#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTimer>

#include "bringapi.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;

    parser.addPositionalArgument("email", "User email to login");
    parser.addPositionalArgument("password", "User password to login");

    QNetworkAccessManager nm;
    QBringApi bring(&nm, nullptr);

    parser.process(a);
    const auto args = parser.positionalArguments();

    QTimer::singleShot(0, [args, &bring] {
        bring.login(args.at(0), args.at(1));
    });

    QObject::connect(&bring, &QBringApi::loggedIn, [&bring]{
        bring.loadLists();
    });

    QObject::connect(&bring, &QBringApi::listsLoaded, [&bring]{
        bring.safeItem(bring.lists().values().at(0), "Milk", "1000ml");
    });

    return a.exec();
}
