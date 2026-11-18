#include "StarModel.h"

StarModel::StarModel(QObject *parent) : QObject(parent) {
}

void StarModel::addStar(const Star &star) {
    m_stars.insert(star.get_id(), star);
}

Star StarModel::getStar(int id) const {
    return m_stars.value(id);
}

QString StarModel::getStarName(int id) const {
    if (m_stars.contains(id)) {
        return m_stars.value(id).get_name();
    }
    return QString("Unknown star");
}

int StarModel::getStarIdByName(const QString &name) const
{
    for (auto it = m_stars.constBegin(); it != m_stars.constEnd(); ++it) {
        if (it.value().get_name().compare(name, Qt::CaseInsensitive) == 0) {
            return it.key();
        }
    }

    return -1;
}

QList<int> StarModel::getAllIds() const {
    return m_stars.keys();
}

void StarModel::clear() {
    m_stars.clear();
}

const QHash<int, Star>& StarModel::getAllStars() const {
    return m_stars;
}

void StarModel::setStarState(int id, StarState state) {
    if (m_stars.contains(id)) {
        m_stars[id].set_state(state);
    }
}
