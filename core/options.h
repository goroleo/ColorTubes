#ifndef OPTIONS_H
#define OPTIONS_H

#include <QtGlobal>


class Options
{
public:
    Options();
    ~Options();

    bool load();
    bool save();

    quint32 level = 0;

private:

};

#endif // OPTIONS_H
