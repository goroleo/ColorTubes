#include <QtQuick>

#ifdef SAILFISH_OS
    #include <sailfishapp.h>
#else
    #include "auroraapp.h"
#endif

#include "src/game.h"
#include "gui/gameboard.h"
#include "gui/flowerlayer.h"

#ifndef SAILFISH_OS
using namespace Aurora;
#endif

static QObject *gameInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return &Game::instance();
}

int main(int argc, char *argv[])
{
    qmlRegisterSingletonType <Game> ("Game", 1, 0, "Game", gameInstance);
    qmlRegisterType <GameBoard> ("GameBoard", 1, 0, "GameBoard");
    qmlRegisterType <FlowerLayer> ("FlowerLayer", 1, 0, "FlowerLayer");

    #ifdef SAILFISH_OS
        QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    #else
        QScopedPointer<QGuiApplication> app(Application::application(argc, argv));
    #endif

    app->setApplicationName("ColorTubes");
    app->setOrganizationName("com.me.legoru");

    #ifdef SAILFISH_OS
        QScopedPointer<QQuickView> view(SailfishApp::createView());
        view->setSource(SailfishApp::pathToMainQml());
    #else
        QScopedPointer<QQuickView> view(Application::createView());
        view->setSource(Application::pathToMainQml());
    #endif

    view->showFullScreen();
    return app->exec();
}
