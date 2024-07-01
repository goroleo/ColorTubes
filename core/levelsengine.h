#ifndef LEVELSENGINE_H
#define LEVELSENGINE_H

class BoardModel;

class LevelsEngine
{
public:
    LevelsEngine();

    int current() { return m_current; }
    BoardModel * create();
    BoardModel * load();


private:
    int m_current;
};

#endif // LEVELSENGINE_H
