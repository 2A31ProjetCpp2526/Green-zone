#ifndef CHARTDIALOG_H
#define CHARTDIALOG_H

#include <QDialog>
#include <QChartView> // Required for displaying the chart
#include <QLineSeries> // Required for line data
#include <QChart> // Required for the main chart object

// Use the QtCharts namespace to avoid prefixing every class with QtCharts::
//QT_CHARTS_USE_NAMESPACE

    class ChartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChartDialog(QWidget *parent = nullptr);
    ~ChartDialog();

private:
    void setupChart();

    // Qt Chart specific members
    QChart *chart;
    QChartView *chartView;
};

#endif // CHARTDIALOG_H
