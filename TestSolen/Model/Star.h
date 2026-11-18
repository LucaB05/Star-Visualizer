#ifndef STAR_H
#define STAR_H

#include <QString>
#include <QVector3D>
#include <QColor>

enum class StarState {
    Normal,
    Filtered,
    Selected,

};

class Star {
public:
    //default, behöver finnas för QMap
    Star() : m_id(-1), m_ra(0), m_dec(0), m_parallax(0) {}

    Star(int id, QString name, float ra, float dec, float parallax,
         float vMag, float bMag, QString spectralType);

    // Getters
    int get_id() const {return m_id;}
    QString get_name() const { return m_name; }
    float get_ra() const { return m_ra; }
    float get_dec() const { return m_dec; }
    float get_parallax() const { return m_parallax; }
    float get_vMag() const { return m_vMag; }
    float get_bMag() const { return m_bMag; }
    QString get_spectralType() const { return m_spectralType; }
    int get_temp() const {return temp; }
    QColor get_color() const;

    QVector3D get_plottedPosition() const { return m_plottedPosition; }
    QColor    get_baseColor()       const { return m_baseColor; }
    StarState get_state()           const { return m_state; }
    float get_visualRadius() const { return m_visualRadius; }

    // Setters
    void set_id     (int id)                      {this->m_id = id;}
    void set_name   (QString name)                { this->m_name = name; }
    void set_ra     (float m_ra)                  { this->m_ra = m_ra; }
    void set_dec    (float m_dec)                 { this->m_dec = m_dec; }
    void set_parallax(float m_parallax)           { this->m_parallax = m_parallax; }
    void set_vMag   (float m_vMag)                { this->m_vMag = m_vMag; }
    void set_bMag   (float m_bMag)                { this->m_bMag = m_bMag; }
    void set_spectralType(QString m_spectralType) { this->m_spectralType = m_spectralType; }
    void set_temp();


    void set_plottedPosition (QVector3D pos) {this->m_plottedPosition = pos;}
    void set_baseColor ();
    void set_state (StarState state) {this->m_state = state;}
    void set_visualRadius(float r) { m_visualRadius = r; }

private:
    int m_id;
    QString m_name;
    float m_ra;         //right ascension i grader
    float m_dec;        //declination i grader
    float m_parallax;   //parallax i milliarcseconds
    float m_vMag;       //visual magnitude
    float m_bMag;       //blue magnitude
    QString m_spectralType;
    int temp;           // Stars temperature in Kelvin


    //plottad data
    QVector3D m_plottedPosition;
    QColor m_baseColor;
    StarState m_state;
    float m_visualRadius;


};

#endif
