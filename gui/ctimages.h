#ifndef CTIMAGES_H
#define CTIMAGES_H
#include <QQuickImageProvider>
#include "gui/bottleimage.h"
#include "src/game.h"

class CtImages : public QQuickImageProvider
{
public:
    CtImages(Game *game)
        : QQuickImageProvider(QQuickImageProvider::Pixmap)
    {
        m_game = game;
    }


    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override
    {
       int width = 80;
       int height = 180;

       if (size)
          *size = QSize(width, height);

       if (requestedSize.width() > 0 && requestedSize.height() > 0)
       {
           m_game->bottleImage->setScale(qMin(requestedSize.width()/width, requestedSize.height()/height));
       }

       if (id.compare("shade"))
           return m_game->bottleImage->shadeYellow();
       else if (id.compare("bottle"))
           return m_game->bottleImage->bottle();
       else
           return QPixmap(width, height);

    }

private:
    Game * m_game;

};

#endif // CTIMAGES_H
