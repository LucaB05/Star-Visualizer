#include "Star.h"
#include <QMap>
#include <QDebug>

Star::Star(int id, QString name, float ra, float dec, float parallax,
                   float vMag, float bMag, QString spectralType)
    : m_id(id),
    m_name(name),
    m_ra(ra),
    m_dec(dec),
    m_parallax(parallax),
    m_vMag(vMag),
    m_bMag(bMag),
    m_spectralType(spectralType),
    m_plottedPosition(0, 0, 0),
    m_baseColor(Qt::white),
    m_state(StarState::Normal),
    m_visualRadius(1)
{

}

void Star::set_temp(){
    QMap<QString, float> heatindex;
    heatindex["O5"] = 54000;
    heatindex["O6"] = 45000;
    heatindex["O7"] = 43300;
    heatindex["O8"] = 40600;
    heatindex["O9"] = 37800;
    heatindex["B0"] = 29200;
    heatindex["B1"] = 23000;
    heatindex["B2"] = 21000;
    heatindex["B3"] = 17600;
    heatindex["B5"] = 15200;
    heatindex["B6"] = 14300;
    heatindex["B7"] = 13500;
    heatindex["B8"] = 12300;
    heatindex["B9"] = 11400;
    heatindex["A0"] = 9600;
    heatindex["A1"] = 9330;
    heatindex["A2"] = 9040;
    heatindex["A3"] = 8750;
    heatindex["A4"] = 8480;
    heatindex["A5"] = 8310;
    heatindex["A7"] = 7920;
    heatindex["F0"] = 7350;
    heatindex["F2"] = 7050;
    heatindex["F3"] = 6850;
    heatindex["F5"] = 6700;
    heatindex["F6"] = 6550;
    heatindex["F7"] = 6400;
    heatindex["F8"] = 6300;
    heatindex["G0"] = 6050;
    heatindex["G1"] = 5930;
    heatindex["G2"] = 5800;
    heatindex["G5"] = 5660;
    heatindex["G8"] = 5440;
    heatindex["K0"] = 5240;
    heatindex["K1"] = 5110;
    heatindex["K2"] = 4960;
    heatindex["K3"] = 4800;
    heatindex["K4"] = 4600;
    heatindex["K5"] = 4400;
    heatindex["K7"] = 4000;
    heatindex["M0"] = 3750;
    heatindex["M1"] = 3700;
    heatindex["M2"] = 3600;
    heatindex["M3"] = 3500;
    heatindex["M4"] = 3400;
    heatindex["M5"] = 3200;
    heatindex["M6"] = 3100;
    heatindex["M7"] = 2900;
    heatindex["M8"] = 2700;
    heatindex["L0"] = 2600;
    heatindex["L3"] = 2200;
    heatindex["L8"] = 1500;
    heatindex["T2"] = 1400;
    heatindex["T6"] = 1000;
    heatindex["T8"] = 800;

    QMap<QString, float> d_heatindex;
    d_heatindex["M1"] = 3600;
    d_heatindex["M2"] = 3480;
    d_heatindex["M3"] = 3300;
    d_heatindex["M4"] = 3150;
    d_heatindex["M5"] = 2900;
    d_heatindex["M6"] = 2700;

    QString s_class;
    QString Spec_class = this->get_spectralType();

    if (Spec_class.isEmpty()){
        float bMag = this->get_bMag();
        float vMag = this->get_vMag();
        temp = 4600 * ((1/(0.92*(bMag-vMag) + 1.7)) + (1/(0.92*(bMag-vMag) + 0.72)));

        QString bestKey;
        int bestDiff = std::numeric_limits<int>::max();
        for(auto spec = heatindex.begin(); spec != heatindex.end(); ++spec){
            int diff = std::abs(spec.value() - temp);

            if(diff < bestDiff){
                bestDiff = diff;
                bestKey = spec.key();
            }
        }
        this->set_spectralType(bestKey);
        this->temp = temp;

    }

    // Sets heatindex for Star
    else if (!Spec_class.startsWith("d")) {
        if(Spec_class.startsWith("k")){
            s_class = Spec_class.mid(1).left(2);
        }
        else
            s_class = Spec_class.left(2);

        // make sure we have a temp for s_class otherwise set to lowest temp
        if (heatindex.contains(s_class)){
            this->temp = heatindex[s_class];
        }
        else{
            this->temp = heatindex["T8"];
        }
    }

    // Sets heatindex for Star if Dwarf star
    else {
        s_class = Spec_class.mid(1).left(2);
        // make sure we have a temp for s_class otherwise set to lowest temp
        if (d_heatindex.contains(s_class)){
            this->temp = d_heatindex[s_class];
        }
        else if (Spec_class.mid(1).left(1).startsWith("A")){
            this->temp = heatindex["B0"];
        }
        else if (Spec_class.mid(1).left(1).startsWith("s")){ //subDwarf
            this->temp = d_heatindex["M1"];
        }
        else if (Spec_class.mid(1).left(1).startsWith("e")){ //Extrem subDwarf ????
            this->temp = heatindex["K7"];
        }
        else {
            this->temp = d_heatindex["M6"];
        }
    }
    return;
};

void Star::set_baseColor(){
    static const std::map<QString, QColor> colorMap = {
        {"O", QColorConstants::Svg::blue},
        {"B", QColorConstants::Svg::cyan},
        {"A", QColorConstants::Svg::white},
        {"F", QColorConstants::Svg::khaki},
        {"G", QColorConstants::Svg::yellow},
        {"K", QColorConstants::Svg::orange},
        {"M", QColorConstants::Svg::red},
        {"d", QColorConstants::Svg::saddlebrown},
        {"L", QColorConstants::Svg::maroon},
        {"T", QColorConstants::Svg::maroon},
        {"NaN", QColorConstants::Svg::chartreuse}
    };

    QString s_class;
    QString Spec_class = this->get_spectralType();

    if (Spec_class.isEmpty()){ // if no spectral class detected
        s_class = "NaN";
    }
    else if (Spec_class.toLower().startsWith("s") || Spec_class.startsWith("e")){ // subDwarf star / Extrem subDwarf ????
        s_class = "d";
    }
    else if (Spec_class.toLower().startsWith("d")){ // if dwarf star
        if (Spec_class.mid(1).left(1) == "A")
            s_class = "A";
        else
            s_class = "d";
    }
    else if (Spec_class.startsWith("k")){ // k start naming????
        s_class = Spec_class.mid(1).left(1);
    }

    else{
        s_class = Spec_class.left(1); // normal star
    }

    if(s_class == "O" || s_class == "B"){ // Jag tror inte detta behövs kollas över innan inlämning
        qDebug() << "Id: " << this->get_id();
    }

    this->m_baseColor = colorMap.at(s_class);
    return;
};
