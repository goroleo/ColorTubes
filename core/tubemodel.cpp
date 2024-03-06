#include "tubemodel.h"

TubeModel::TubeModel()
{
    clear();
}

TubeModel::~TubeModel()
{

}

void TubeModel::clear()
{
    m_items.stored = 0;
    m_state = 0;
    m_count = 0;
    m_needStateUpdate = false;
}

bool TubeModel::isEmpty()
{
    return (m_count == 0);
}

bool TubeModel::isClosed()
{
    if (m_needStateUpdate)
        return checkClosed();
    return (m_state == 3);
}

bool TubeModel::checkClosed()
{
    if (m_count != 4)
        return false;

    quint8 tempColor = m_items.colors[0];
    for (quint8 i = 1; i < 4; ++i) {
        if (m_items.colors[i] != tempColor) {
            return false;
        }
    }
    return true;
}

void TubeModel::updateState()
{
    m_needStateUpdate = false;
    switch (m_count) {
    case 0:
        m_state = 0;             // STATE_EMPTY
        return;
    case 4:
        if (checkClosed()) {
            m_state = 3;         // STATE_CLOSED
        } else {
            m_state = 2;         // STATE_FILLED
        }
        return;
    default:
        m_state = 1;             // STATE_REGULAR
    }
}

quint8 TubeModel::currentColor() const
{
    if (m_count == 0)
        return 0;
    return m_items.colors[m_count - 1];
}

quint8 TubeModel::color(quint8 index)
{
    if (index > 4)
        return 0;
    return m_items.colors[index];
}

bool TubeModel::hasColor(quint8 colorNumber)
{
    quint8 index = 0;
    while (index < m_count) {
        if (m_items.colors[index] == colorNumber)
            return true;
        index ++;
    }
    return false;
}

bool TubeModel::canPutColor(quint8 colorNumber)
{
    if (colorNumber == 0)
        return false;

    switch(m_count) {
    case 0:
        return true;
    case 4:
        return false;
    default:
        return (currentColor() == colorNumber);
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

    m_items.colors[m_count] = colorNumber;
    m_count ++;

    // update tube's state
    if (updateState) {
        m_needStateUpdate = false;
        if (m_count < 4) {
            m_state = 1;                 // STATE_REGULAR
        } else if (checkClosed()) {
            m_state = 3;                 // STATE_CLOSED
        } else {
            m_state = 2;                 // STATE_FILLED
        }
    } else {
        m_needStateUpdate = true;
    }

    return true;
}

bool TubeModel::canExtractColor()
{
    if (m_needStateUpdate)
        updateState();
    return (m_state == 1 || m_state == 2);
}

quint8 TubeModel::extractColor()
{
    if (m_count == 0)
        return 0;

    m_count --;
    quint8 result = m_items.colors[m_count];
    m_items.colors[m_count] = 0;

    // update tube's state
    if (m_count == 0) {
        m_state = 0;                     // STATE_EMPTY;
    } else {
        m_state = 1;                     // STATE_REGULAR;
    }
    m_needStateUpdate = false;
    return result;
}

quint8 TubeModel::sameColorsCount() const
{
    if (m_count == 0) {
        return 0;
    }

    quint8 current = m_items.colors[m_count - 1];
    quint8 result = 0;
    quint8 i = m_count;
    do {
        result++;
        i--;
    } while (i > 0 && current == m_items.colors[i - 1]);

    return result;
}

bool TubeModel::operator == (const TubeModel & other) const
{
    return m_items.stored == other.m_items.stored;
}

void TubeModel::assignColors(quint32 stored)
{
    clear();
    while (stored > 0)
    {
        putColor(stored & 0xff, false);
        stored >>= 8;
    }
    updateState();
}

void TubeModel::assignColors(TubeModel * tm)
{
    if (tm != nullptr) {
        m_items.stored = tm->storeColors();
        m_state = tm->state();
        m_count = tm->count();
        m_needStateUpdate = tm->m_needStateUpdate;
    }
}

quint32 TubeModel::storeColors() const
{
    return m_items.stored;
}


