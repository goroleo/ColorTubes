#include "auroraapp.h"

#include "ctglobal.h"
#include "src/game.h"
#include "gui/gameboard.h"
#include "gui/flowerlayer.h"

using namespace Aurora;

int main(int argc, char *argv[])
{
    qmlRegisterSingletonType <Game> ("Game", 1, 0, "Game", gameInstance);
    qmlRegisterType <GameBoard> ("GameBoard", 1, 0, "GameBoard");
    qmlRegisterType <FlowerLayer> ("FlowerLayer", 1, 0, "FlowerLayer");

    QScopedPointer<QGuiApplication> app(Application::application(argc, argv));
    app->setApplicationName("ColorTubes");
    app->setOrganizationName("com.me.legoru");

    QScopedPointer<QQuickView> view(Application::createView());
    view->setSource(Application::pathToMainQml());
    view->showFullScreen();
    return app->exec();
}
