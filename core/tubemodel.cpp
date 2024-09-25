#include "tubemodel.h"

TubeModel::TubeModel()
{
    m_colors.stored = 0;
}

TubeModel::~TubeModel()
{
}

void TubeModel::clear()
{
    quint8 oldState = m_state;
    m_colors.stored = 0;
    m_state = CT_STATE_EMPTY;
    m_count = 0;
    if (oldState != m_state)
        emit stateChanged();
}

bool TubeModel::isEmpty() const
{
    return m_count == 0;
}

bool TubeModel::isDone() const
{
    return m_state == CT_STATE_DONE;
}

bool TubeModel::checkDone()
{
    if (m_count != 4)
        return false;

    for (quint8 i = 1; i < 4; ++i) {
        if (m_colors.items[i] != m_colors.items[0]) {
            return false;
        }
    }
    return true;
}

void TubeModel::updateState()
{
    quint8 oldState = m_state;
    switch (m_count) {
    case 0:
        m_state = CT_STATE_EMPTY;
        return;
    case 4:
        if (checkDone()) {
            m_state = CT_STATE_DONE;
        } else {
            m_state = CT_STATE_FILLED;
        }
        return;
    default:
        m_state = CT_STATE_REGULAR;
    }
    if (oldState != m_state)
        emit stateChanged();
}

quint8 TubeModel::currentColor() const
{
    if (m_count == 0 || m_state == CT_STATE_DONE)
        return 0;
    return m_colors.items[m_count - 1];
}

quint8 TubeModel::color(quint8 index)
{
    if (index >= m_count)
        return 0;
    return m_colors.items[index];
}

bool TubeModel::hasColor(quint8 colorNumber) const
{
    for (quint8 i = 0; i < m_count; ++i) {
        if (m_colors.items[i] == colorNumber)
            return true;
    }
    return false;
}

bool TubeModel::canPutColor(quint8 colorNumber) const
{
    if (colorNumber == 0)
        return false;

    switch(m_count) {
    case 0:
        return true;
    case 4:
        return false;
    default:
        return (m_colors.items[m_count - 1] == colorNumber);
    }
}

bool TubeModel::putColor(quint8 colorNumber)
{
    return putColor(colorNumber, true);
}

bool TubeModel::putColor(quint8 colorNumber, bool updateState)
{
    if ((m_count >= 4) || (colorNumber == 0))
        return false;

    m_colors.items[m_count] = colorNumber;
    m_count ++;

    // update tube's state
    if (updateState) {
        quint8 oldState = m_state;
        if (m_count < 4) {
            m_state = CT_STATE_REGULAR;
        } else if (checkDone()) {
            m_state = CT_STATE_DONE;
        } else {
            m_state = CT_STATE_FILLED;
        }
        if (oldState != m_state)
            emit stateChanged();
    }
    return true;
}

bool TubeModel::canExtractColor() const
{
    return (m_state == CT_STATE_FILLED || m_state == CT_STATE_REGULAR);
}

quint8 TubeModel::extractColor()
{
    if (m_count == 0)
        return 0;

    m_count --;
    quint8 result = m_colors.items[m_count];
    m_colors.items[m_count] = 0;

    // update tube's state
    quint8 oldState = m_state;
    if (m_count == 0) {
        m_state = CT_STATE_EMPTY;
    } else {
        m_state = CT_STATE_REGULAR;
    }
    if (oldState != m_state)
        emit stateChanged();
    return result;
}

quint8 TubeModel::currentColorCount() const
{
    if (m_count == 0)
        return 0;

    quint8 result = 1;
    quint8 i = m_count - 1;

    while (i > 0 && m_colors.items[i - 1] == m_colors.items[m_count - 1]) {
        result ++;
        i--;
    }

    return result;
}

bool TubeModel::operator == (const TubeModel & other)
{
    return m_colors.stored == other.m_colors.stored;
}

void TubeModel::assignColors(quint32 storedColors)
{
    clear();
    while (storedColors > 0) {
        putColor(storedColors & 0xff, false);
        storedColors >>= 8;
    }
    updateState(); // don't forget!
}

void TubeModel::assignColors(const TubeModel &other)
{
    m_colors.stored = other.store();
    m_count = other.count();
    m_state = other.state();
}

quint32 TubeModel::store() const
{
    return m_colors.stored;
}

TubeModels::~TubeModels()
{
    clear();
}

void TubeModels::removeLast()
{
    if (!isEmpty()) {
        delete last();
        remove(size() - 1);
    }
}

void TubeModels::clear()
{
    while (!isEmpty())
        removeLast();
}
