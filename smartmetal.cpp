#include "smartmetal.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QBrush>
#include <QColor>

SmartMetalDialog::SmartMetalDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("SmartMetal Classifier");
    resize(600, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Table setup
    table = new QTableWidget(this);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"Weight", "Conductivity", "Magnetic (0/1)",
                                      "IR Purity", "Radiation", "Classify"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true); // alternate row colors
    mainLayout->addWidget(table);

    // Table stylesheet for header and buttons
    table->setStyleSheet(
        "QTableWidget {"
        "  background-color: #f0f0f0;"
        "  alternate-background-color: #d0f0ff;"
        "  gridline-color: #a0a0a0;"
        "}"
        "QHeaderView::section {"
        "  background-color: #87CEFA;"
        "  color: white;"
        "  font-weight: bold;"
        "}"
        "QPushButton {"
        "  background-color: #4CAF50;"
        "  color: white;"
        "  border-radius: 5px;"
        "  padding: 3px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #45a049;"
        "}"
        );

    // Add Row Button
    addRowBtn = new QPushButton("Add Metal Row", this);
    mainLayout->addWidget(addRowBtn);

    // Connect Add Row Button
    connect(addRowBtn, &QPushButton::clicked, [=]() {
        bool ok;
        double weight = QInputDialog::getDouble(this, "Weight", "Enter weight:", 100, 0, 1000, 2, &ok);
        if (!ok) return;
        double conduct = QInputDialog::getDouble(this, "Conductivity", "Enter conductivity:", 10, 0, 1000, 2, &ok);
        if (!ok) return;
        int magnetic = QInputDialog::getInt(this, "Magnetic", "Enter magnetic (0/1):", 0, 0, 1, 1, &ok);
        if (!ok) return;
        double irPurity = QInputDialog::getDouble(this, "IR Purity", "Enter IR Purity:", 50, 0, 100, 2, &ok);
        if (!ok) return;
        double radiation = QInputDialog::getDouble(this, "Radiation", "Enter Radiation:", 0, 0, 100, 2, &ok);
        if (!ok) return;

        addMetalRow(weight, conduct, magnetic, irPurity, radiation);
    });

    // Example metals
    addMetalRow(300, 10, 1, 50, 0);
    addMetalRow(80, 40, 0, 85, 0);
    addMetalRow(200, 15, 0, 30, 50);
}

// Add a row with data + classify button
void SmartMetalDialog::addMetalRow(double weight, double conduct, int magnetic,
                                   double irPurity, double radiation)
{
    int row = table->rowCount();
    table->insertRow(row);

    table->setItem(row, 0, new QTableWidgetItem(QString::number(weight)));
    table->setItem(row, 1, new QTableWidgetItem(QString::number(conduct)));
    table->setItem(row, 2, new QTableWidgetItem(QString::number(magnetic)));
    table->setItem(row, 3, new QTableWidgetItem(QString::number(irPurity)));
    table->setItem(row, 4, new QTableWidgetItem(QString::number(radiation)));

    QPushButton *btn = new QPushButton("Classify", this);
    table->setCellWidget(row, 5, btn);

    // Connect button
    connect(btn, &QPushButton::clicked, [=]() {
        QString result = SmartMetal::classifyMetal(weight, conduct, magnetic, irPurity, radiation);
        QMessageBox::information(this, "Classification Result", "Metal classified as: " + result);

        // Color row based on classification
        QColor color;
        if (result == "Ferrous") color = QColor("#FFCCCC");
        else if (result == "Non-Ferrous") color = QColor("#FFFF99");
        else if (result == "Recyclable") color = QColor("#CCFFCC");
        else if (result == "E-Waste") color = QColor("#FFCCFF");
        else color = QColor("#D3D3D3");

        for (int col = 0; col < table->columnCount(); col++)
            table->item(row, col)->setBackground(color);
    });
}

// -------------------------
// SmartMetal classification (KNN)
QString SmartMetal::classifyMetal(double weight, double conduct, int magnetic,
                                  double irPurity, double radiation)
{
    QVector<Sample> training = {
        {300, 10, 1, 50, 0, "Ferrous"},
        {450, 12, 1, 55, 0, "Ferrous"},
        {80,  40, 0, 85, 0, "Non-Ferrous"},
        {120, 38, 0, 90, 0, "Non-Ferrous"},
        {100, 55, 0, 95, 0, "Recyclable"},
        {150, 60, 0, 97, 0, "Recyclable"},
        {200, 15, 0, 30, 50, "E-Waste"},
        {250, 20, 0, 35, 70, "E-Waste"},
        {50,  5,  0, 20, 90, "Hazardous"},
        {60,  7,  0, 25, 85, "Hazardous"}
    };

    struct Dist { double value; QString label; };
    QVector<Dist> distances;

    for (const auto &s : training) {
        double d = std::sqrt(
            std::pow(s.weight - weight, 2) +
            std::pow(s.conductivity - conduct, 2) +
            std::pow(s.magnetic - magnetic, 2) +
            std::pow(s.infraredPurity - irPurity, 2) +
            std::pow(s.radiationLevel - radiation, 2)
            );
        distances.push_back({d, s.label});
    }

    std::sort(distances.begin(), distances.end(),
              [](const Dist &a, const Dist &b){ return a.value < b.value; });

    const int k = 3;
    int ferrous = 0, nonf = 0, ewaste = 0, haz = 0, rec = 0;

    for (int i = 0; i < k; i++) {
        QString c = distances[i].label;
        if (c == "Ferrous") ferrous++;
        else if (c == "Non-Ferrous") nonf++;
        else if (c == "E-Waste") ewaste++;
        else if (c == "Hazardous") haz++;
        else rec++;
    }

    if (ferrous >= nonf && ferrous >= rec && ferrous >= ewaste && ferrous >= haz) return "Ferrous";
    if (nonf >= ferrous && nonf >= rec && nonf >= ewaste && nonf >= haz) return "Non-Ferrous";
    if (rec >= ferrous && rec >= nonf && rec >= ewaste && rec >= haz) return "Recyclable";
    if (ewaste >= ferrous && ewaste >= nonf && ewaste >= rec && ewaste >= haz) return "E-Waste";
    return "Hazardous";
}



