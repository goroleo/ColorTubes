#ifndef SOLVER_H
#define SOLVER_H

#include <QObject>

class BoardModel;
class MoveItems;

class Solver : public QObject
{
    Q_OBJECT
public:
    explicit Solver(BoardModel *startBoard = 0);
    ~Solver();

    void         clear();
    void         setStartBoard(BoardModel *startBoard = 0);
    void         start(BoardModel *startBoard = 0);
    void         stop();                          // point to safe interrupt the process

    MoveItems  * moves()  { return m_moves; }
    quint8       result() { return m_resultId; }  // see CtGlobal::CT_SOLVER_*

signals:
    void         finished(quint8 result);

private:
    bool         m_externalBreak = false;
    BoardModel * m_startBoard;
    MoveItems  * m_moves;
    QVector <quint32> * m_hashes;
    quint8       m_resultId;   // see CtGlobal::CT_SOLVER_*
};

/*
// Unused. It was planned for threads but solution process completes in milliseconds
class SolverThread : public QObject
{
    Q_OBJECT
public:
    void start(BoardModel *startBoard = 0);
};
*/

#endif // SOLVER_H
