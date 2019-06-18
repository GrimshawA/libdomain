#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <src/game_view_model.hpp>

#include "qdom.hpp"

class X : public QObject {

};

class Y {

};

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // TESTS
    std::cout << dom::object_traits<X>::name << std::endl;
    std::cout << dom::object_traits<Y>::name << std::endl;

    QDomModel<X> mm;

    qmlRegisterType<GameViewModel>("Game", 1, 0, "GameViewModel");

    //qmlRegisterType<QDomModel<Building>>("Game", 1, 0, "")

    qRegisterMetaType<QDomModel<Building>*>("QDomModel<Building>*");
    qRegisterMetaType<QDomModel<Trophy>*>("QDomModel<Trophy>*");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
