#ifndef STARMODEL_H
#define STARMODEL_H

#include <QObject>
#include <QHash>
#include "Star.h"

class StarModel : public QObject {
    Q_OBJECT

public:
    explicit StarModel(QObject *parent = nullptr);

    void addStar(const Star &star);

    Star getStar(int id) const;

    QString getStarName(int id) const;
    int getStarIdByName(const QString &name) const;

    QList<int> getAllIds() const;

    void clear();

    const QHash<int, Star>& getAllStars() const;

    void setStarState(int id, StarState state);

private:
    QHash<int, Star> m_stars;
};

#endif // STARMODEL_H
