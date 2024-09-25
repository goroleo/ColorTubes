#ifndef SOLVER_H
#define SOLVER_H

#include <QObject>

class BoardModel;
class MoveItems;

class SolveProcess : public QObject
{
    Q_OBJECT
public:
    explicit SolveProcess();
    ~SolveProcess();

    void         clear();
    void         setRootBoard(BoardModel *startBoard = 0);
    void         doSolve(BoardModel *startBoard = 0);
    void         stop();
    MoveItems  * moves() {return m_moves;}

signals:
    void solved();
    void notSolved();
    void interrupted();

private:
    bool         m_externalBreak = false;
    BoardModel * m_rootBoard;
    MoveItems  * m_moves;
    QVector <quint32> * m_hashes;
};

class Solver : public QObject
{
    Q_OBJECT
public:
    void start(BoardModel *startBoard = 0);
};

#endif // SOLVER_H
