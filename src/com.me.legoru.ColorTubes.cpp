#include <QtQuick>
#include "auroraapp.h"

#include "src/game.h"
#include "gui/gameboard.h"
#include "gui/flowerlayer.h"

using namespace Aurora;

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

    QScopedPointer<QGuiApplication> app(Application::application(argc, argv));
    app->setApplicationName("ColorTubes");
    app->setOrganizationName("com.me.legoru");

    QScopedPointer<QQuickView> view(Application::createView());
    view->rootContext()->setContextProperty("version", APP_VERSION);
    view->setSource(Application::pathToMainQml());
    view->showFullScreen();
    return app->exec();
}
