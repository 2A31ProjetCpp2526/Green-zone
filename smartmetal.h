#ifndef SMARTMETAL_H
#define SMARTMETAL_H

#include <QDialog>
#include <QTableWidget>
#include <QString>
#include <QVector>
#include <QPushButton>

struct Sample {
    double weight;
    double conductivity;
    int magnetic;
    double infraredPurity;
    double radiationLevel;
    QString label;
};

class SmartMetal
{
public:
    SmartMetal() {}
    // Static classification function, no object needed
    static QString classifyMetal(double weight, double conductivity, int magnetic,
                                 double infraredPurity, double radiationLevel);
};

class SmartMetalDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SmartMetalDialog(QWidget *parent = nullptr);

private:
    QTableWidget *table;
    QPushButton *addRowBtn;

    void addMetalRow(double weight, double conductivity, int magnetic,
                     double irPurity, double radiation);
};

#endif // SMARTMETAL_H
