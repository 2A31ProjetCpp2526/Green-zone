
#include "chartdialog.h"
#include <QVBoxLayout>

//QT_CHARTS_USE_NAMESPACE

ChartDialog::ChartDialog(QWidget *parent) :
    QDialog(parent),
    chart(new QChart()),
    chartView(new QChartView(chart))
{
    // ============================================================
    // 1. Setup Data Series (Green Zone Air Quality Evaluation)
    // ============================================================

    // Oxygen (O2) series
    QLineSeries *o2Series = new QLineSeries();
    o2Series->setName("Oxygen (O₂) %");

    // CO2 series
    QLineSeries *co2Series = new QLineSeries();
    co2Series->setName("Carbon Dioxide (CO₂) %");

    // Sample values: relation between green surface and air quality
    // x-axis = green surface index (0 = small, 5 = very large)
    // y-axis = percentage of gas concentration

    // Green Surface Index → O₂ (%)
    o2Series->append(0, 18);  // Very low vegetation
    o2Series->append(1, 19);
    o2Series->append(2, 20);
    o2Series->append(3, 21);
    o2Series->append(4, 22);
    o2Series->append(5, 23);  // Very green zone

    // Green Surface Index → CO₂ (%)
    co2Series->append(0, 0.045); // High CO2 (bad area)
    co2Series->append(1, 0.040);
    co2Series->append(2, 0.037);
    co2Series->append(3, 0.034);
    co2Series->append(4, 0.031);
    co2Series->append(5, 0.029); // Green zone reduces CO2

    // ============================================================
    // 2. Add Series to Chart
    // ============================================================
    chart->addSeries(o2Series);
    chart->addSeries(co2Series);

    // ============================================================
    // 3. Chart Appearance
    // ============================================================
    chart->setTitle("Impact of Green Surfaces on O₂ and CO₂ Levels");

    chart->createDefaultAxes();

    if (chart->axes(Qt::Horizontal).count() > 0)
        chart->axes(Qt::Horizontal).first()->setTitleText("Green Surface Index");

    if (chart->axes(Qt::Vertical).count() > 0)
        chart->axes(Qt::Vertical).first()->setTitleText("Gas Concentration");

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // ============================================================
    // 4. Chart View Settings
    // ============================================================
    chartView->setRenderHint(QPainter::Antialiasing);

    // ============================================================
    // 5. Layout
    // ============================================================
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);

    setWindowTitle("Green Zone Air Quality Chart");
    resize(800, 600);
}

ChartDialog::~ChartDialog()
{
}
