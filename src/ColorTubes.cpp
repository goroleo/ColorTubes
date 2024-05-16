#include <sailfishapp.h>

#include <QtQuick>
#include <QObject>

#include "ctglobal.h"
#include "gui/gameboard.h"
#include "gui/flowerlayer.h"

int main(int argc, char *argv[])
{
    qmlRegisterType <GameBoard> ("GameBoard", 1, 0, "GameBoard");
    qmlRegisterType <FlowerLayer> ("FlowerLayer", 1, 0, "FlowerLayer");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setApplicationName("ColorTubes");
    app->setOrganizationName("com.me.legoru");

    CtGlobal::create();

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setSource(SailfishApp::pathToMainQml());
    view->showFullScreen();
    int result = app->exec();

    CtGlobal::destroy();
    return result;
}
