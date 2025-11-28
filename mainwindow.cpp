#include "mainwindow.h"
#include "connection.h"
#include "zone.h"
#include "poubelle.h"
#include "ChartDialog.h"
#include "sortdialog.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries> // ou tout autre type de série utilisé
#include <QSortFilterProxyModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QCalendarWidget>
#include <QListWidget>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QTextCharFormat>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QCalendar>
#include <QPalette>
#include <QCalendarWidget>
//QT_CHARTS_USE_NAMESPACE

// --------------------------------------------------------------
// CONSTRUCTOR
// --------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    proxyModel(nullptr),
    isCalendarVisible(false)
{
    setWindowTitle("Dashboard - ZONES VERTES");
    resize(1400, 700);

    // ---- DB Connection ----
    Connection *c = Connection::getInstance();
    if (!c->openConnection()) {
        QMessageBox::critical(this, "Erreur", "Échec de connexion à la base de données !");
    } else {
        // Auto-create table if not exists
        QSqlQuery q;
        QString ddl =
            "BEGIN "
            "EXECUTE IMMEDIATE 'CREATE TABLE ZONES_VERTES ("
            "ID_ZONE VARCHAR2(20 BYTE) PRIMARY KEY, "
            "NOM VARCHAR2(50 BYTE), "
            "TYPE VARCHAR2(50 BYTE), "
            "SUPERFICIE NUMBER, "
            "LOCALISATION VARCHAR2(100 BYTE), "
            "RESPONSABLE VARCHAR2(50 BYTE), "
            "ETAT VARCHAR2(50 BYTE))'; "
            "EXCEPTION WHEN OTHERS THEN "
            "IF SQLCODE = -955 THEN NULL; ELSE RAISE; END IF; "
            "END;";
        q.exec(ddl);
    }

    setupUI();
    afficherZones();
    setupConnections();

    // Notifications timer
    notificationTimer = new QTimer(this);
    connect(notificationTimer, &QTimer::timeout, this, &MainWindow::checkEventNotifications);
    notificationTimer->start(60000);
}
QSqlQuery q;




MainWindow::~MainWindow()
{
    if (notificationTimer && notificationTimer->isActive())
        notificationTimer->stop();
}

// --------------------------------------------------------------
// UI SETUP
// --------------------------------------------------------------
void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // Sidebar
    setupSidebar();
    mainLayout->addWidget(sidebarWidget);

    // LEFT panel
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(12);

    setupForm();
    leftLayout->addWidget(totalZonesLabel);
    leftLayout->addWidget(totalAreaLabel);
    leftLayout->addLayout(formLayout);

    setupButtons();
    leftLayout->addLayout(btnLayout);

    // Search Row
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Rechercher...");
    searchClearBtn = new QPushButton("Clear");
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchClearBtn);
    leftLayout->addLayout(searchLayout);

    setupTable();
    leftLayout->addWidget(tableView);

    exportPdfBtn = new QPushButton("Exporter PDF");
    exportPdfBtn->setStyleSheet("background-color:#007F8F;color:white;padding:8px;border-radius:8px;");
    leftLayout->addWidget(exportPdfBtn);

    mainLayout->addLayout(leftLayout);

    // Calendar panel
    QVBoxLayout *calLayout = new QVBoxLayout();

    calendar = new QCalendarWidget();
    calendar->setGridVisible(true);
    calendar->setVisible(false);
    calLayout->addWidget(calendar);

    eventList = new QListWidget();
    eventList->setVisible(false);
    calLayout->addWidget(eventList);

    addEventBtn = new QPushButton("Ajouter Événement");
    removeEventBtn = new QPushButton("Supprimer Événement");
    addEventBtn->setVisible(false);
    removeEventBtn->setVisible(false);

    QHBoxLayout *eventBtns = new QHBoxLayout();
    eventBtns->addWidget(addEventBtn);
    eventBtns->addWidget(removeEventBtn);

    calLayout->addLayout(eventBtns);
    mainLayout->addLayout(calLayout);

    updateCalendarHighlights();
    // Chart widget
    //chartView = new QtCharts::QChartView(this);
    //chartView->setRenderHint(QPainter::Antialiasing);
    //chartView->setMinimumHeight(300);   // Adjust height as needed
    //chartView->setMinimumWidth(400);    // Adjust width
   // leftLayout->addWidget(chartView);   // Add it below tableView

}

// --------------------------------------------------------------
// SIDEBAR
// --------------------------------------------------------------
void MainWindow::setupSidebar()
{
    sidebarWidget = new QWidget();
    sidebarWidget->setFixedWidth(220);
    sidebarWidget->setStyleSheet("background-color:#005B99;color:white;border-radius:12px;");

    sidebarLayout = new QVBoxLayout(sidebarWidget);
    sidebarLayout->setContentsMargins(15, 15, 15, 15);
    sidebarLayout->setSpacing(12);

    QString btnStyle =
        "QPushButton { background-color:#007F8F;color:white;padding:10px;border-radius:8px;font-weight:bold; }"
        "QPushButton:hover { background-color:#005B99; }";

    calcTotalZonesBtn = new QPushButton("Total Zones");
    calcTotalAreaBtn = new QPushButton("Superficie Totale");
    classifierPoubelleBtn = new QPushButton("Classifier Poubelles");
    sortBtn = new QPushButton("Trier");
    toggleCalendarBtn = new QPushButton("Afficher Calendrier");

    calcTotalZonesBtn->setStyleSheet(btnStyle);
    calcTotalAreaBtn->setStyleSheet(btnStyle);
    classifierPoubelleBtn->setStyleSheet(btnStyle);
    sortBtn->setStyleSheet(btnStyle);
    toggleCalendarBtn->setStyleSheet(btnStyle);

    sidebarLayout->addWidget(calcTotalZonesBtn);
    sidebarLayout->addWidget(calcTotalAreaBtn);
    sidebarLayout->addWidget(classifierPoubelleBtn);
    sidebarLayout->addWidget(sortBtn);
    sidebarLayout->addWidget(toggleCalendarBtn);

    sidebarLayout->addStretch();
    ecoAnalysisBtn = new QPushButton("Analyse Écologique");
    ecoAnalysisBtn->setStyleSheet(btnStyle);
    sidebarLayout->addWidget(ecoAnalysisBtn);
    refreshChartBtn = new QPushButton("Actualiser Chartes");
    refreshChartBtn->setStyleSheet(btnStyle);
    sidebarLayout->addWidget(refreshChartBtn);


}

// --------------------------------------------------------------
// FORM
// --------------------------------------------------------------
void MainWindow::setupForm()
{
    idEdit = new QLineEdit();
    nomEdit = new QLineEdit();
    typeEdit = new QLineEdit();
    supEdit = new QLineEdit();
    locEdit = new QLineEdit();
    respEdit = new QLineEdit();
    etatEdit = new QLineEdit();

    formLayout = new QGridLayout();
    formLayout->setSpacing(8);

    formLayout->addWidget(new QLabel("ID Zone:"), 0, 0);
    formLayout->addWidget(idEdit, 0, 1);
    formLayout->addWidget(new QLabel("Nom:"), 1, 0);
    formLayout->addWidget(nomEdit, 1, 1);
    formLayout->addWidget(new QLabel("Type:"), 2, 0);
    formLayout->addWidget(typeEdit, 2, 1);
    formLayout->addWidget(new QLabel("Superficie:"), 3, 0);
    formLayout->addWidget(supEdit, 3, 1);
    formLayout->addWidget(new QLabel("Localisation:"), 4, 0);
    formLayout->addWidget(locEdit, 4, 1);
    formLayout->addWidget(new QLabel("Responsable:"), 5, 0);
    formLayout->addWidget(respEdit, 5, 1);
    formLayout->addWidget(new QLabel("État:"), 6, 0);
    formLayout->addWidget(etatEdit, 6, 1);

    totalZonesLabel = new QLabel("Total Zones: 0");
    totalAreaLabel = new QLabel("Superficie Totale: 0 ha");
}

// --------------------------------------------------------------
// BUTTONS
// --------------------------------------------------------------
void MainWindow::setupButtons()
{
    addBtn = new QPushButton("Ajouter");
    updateBtn = new QPushButton("Modifier");
    deleteBtn = new QPushButton("Supprimer");
    refreshBtn = new QPushButton("Actualiser");

    QString style =
        "QPushButton { background-color:#007F8F;color:white;padding:8px;border-radius:8px;font-weight:bold; }"
        "QPushButton:hover { background-color:#005B99; }";

    addBtn->setStyleSheet(style);
    updateBtn->setStyleSheet(style);
    deleteBtn->setStyleSheet(style);
    refreshBtn->setStyleSheet(style);

    btnLayout = new QHBoxLayout();
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(updateBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(refreshBtn);
}

// --------------------------------------------------------------
// TABLE
// --------------------------------------------------------------
void MainWindow::setupTable()
{
    tableView = new QTableView();
    tableView->setAlternatingRowColors(true);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setVisible(false);
    tableView->setStyleSheet(
        "QTableView {"
        "    background-color: #f0f8ff;"          /* couleur de fond */
        "    alternate-background-color: #dbefff;" /* lignes alternées */
        "    gridline-color: #007F8F;"             /* couleur des bordures */
        "    color: #000000;"                      /* couleur du texte */
        "    selection-background-color: #007F8F;" /* fond ligne sélectionnée */
        "    selection-color: white;"              /* texte ligne sélectionnée */
        "}"
        "QHeaderView::section {"
        "    background-color: #007F8F;"          /* en-tête fond */
        "    color: white;"                        /* texte en-tête */
        "    padding: 4px;"
        "    border: 1px solid #005B99;"
        "}"
        );
    tableView->setAlternatingRowColors(true);


}

// --------------------------------------------------------------
// CONNECTIONS
// --------------------------------------------------------------
void MainWindow::setupConnections()
{
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::ajouterZone);
    connect(updateBtn, &QPushButton::clicked, this, &MainWindow::modifierZone);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::supprimerZone);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::afficherZones);

    connect(calcTotalZonesBtn, &QPushButton::clicked, this, &MainWindow::calculerTotalZones);
    connect(calcTotalAreaBtn, &QPushButton::clicked, this, &MainWindow::calculerTotalSuperficie);
    connect(classifierPoubelleBtn, &QPushButton::clicked, this, &MainWindow::classifierZonePoubelle);

    connect(sortBtn, &QPushButton::clicked, this, &MainWindow::ouvrirTri);
    connect(toggleCalendarBtn, &QPushButton::clicked, this, &MainWindow::toggleCalendar);

    connect(searchClearBtn, &QPushButton::clicked, searchEdit, &QLineEdit::clear);

    connect(exportPdfBtn, &QPushButton::clicked, this, &MainWindow::exportToPdf);

    connect(calendar, &QCalendarWidget::selectionChanged, this, [=]() {
        eventList->clear();
        QDate d = calendar->selectedDate();
        if (events.contains(d))
            eventList->addItems(events[d]);
    });

    connect(addEventBtn, &QPushButton::clicked, this, [=]() {
        QDate d = calendar->selectedDate();
        QString evt = QInputDialog::getText(this, "Ajouter événement",
                                            "Événement pour " + d.toString("dd/MM/yyyy") + " :");
        if (!evt.isEmpty()) {
            events[d].append(evt);
            eventList->addItem(evt);
            updateCalendarHighlights();
        }
    });

    connect(removeEventBtn, &QPushButton::clicked, this, [=]() {
        QDate d = calendar->selectedDate();
        auto selected = eventList->selectedItems();
        for (auto item : selected) {
            events[d].removeOne(item->text());
            delete item;
        }
        updateCalendarHighlights();
    });
    connect(ecoAnalysisBtn, &QPushButton::clicked,
            this, &MainWindow::showEcoConclusionDialog);
    btnShowChart = new QPushButton("Show City Chart");
    //layout->addWidget(btnShowChart);

    //connect(btnShowChart, &QPushButton::clicked, this, &CrudDialog::openChartDialog);
    //connect(refreshChartBtn, &QPushButton::clicked, this, &MainWindow::updateCityChart);



}

// --------------------------------------------------------------
// CRUD
// --------------------------------------------------------------
void MainWindow::afficherZones()
{
    tableView->setModel(Ztmp.afficher());
    setupSearchAndSort();  // Important
}

void MainWindow::ajouterZone()
{
    Zone z(idEdit->text(), nomEdit->text(), typeEdit->text(),
           supEdit->text().toDouble(), locEdit->text(),
           respEdit->text(), etatEdit->text());

    if (z.ajouter()) {
        afficherZones();
        calculerTotalZones();
        calculerTotalSuperficie();
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible d'ajouter la zone.");
    }
}

void MainWindow::modifierZone()
{
    Zone z(idEdit->text(), nomEdit->text(), typeEdit->text(),
           supEdit->text().toDouble(), locEdit->text(),
           respEdit->text(), etatEdit->text());

    if (z.modifier(idEdit->text())) {
        afficherZones();
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de modifier la zone.");
    }
}

void MainWindow::supprimerZone()
{
    if (Ztmp.supprimer(idEdit->text())) {
        afficherZones();
        calculerTotalZones();
        calculerTotalSuperficie();
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de supprimer la zone.");
    }
}

// --------------------------------------------------------------
// CALCULS
// --------------------------------------------------------------
void MainWindow::calculerTotalZones()
{
    QSqlQuery q("SELECT COUNT(*) FROM ZONES_VERTES");
    if (q.next())
        totalZonesLabel->setText("Total Zones: " + q.value(0).toString());
}

void MainWindow::calculerTotalSuperficie()
{
    QSqlQuery q("SELECT SUM(SUPERFICIE) FROM ZONES_VERTES");
    if (q.next())
        totalAreaLabel->setText("Superficie Totale: " + q.value(0).toString() + " ha");
}

// --------------------------------------------------------------
// CLASSIFICATION POUBELLES
// --------------------------------------------------------------
void MainWindow::classifierZonePoubelle()
{
    int nbP = QInputDialog::getInt(this, "Nombre de poubelles", "Entrez le nombre :", 10, 1, 100);
    double taux = QInputDialog::getDouble(this, "Taux remplissage", "En % :", 60, 0, 100);
    int jours = QInputDialog::getInt(this, "Dernier ramassage", "Jours écoulés :", 2, 0, 30);

    QString r = Poubelle::classifierPoubelle(nbP, taux, jours);
    QString couleur = (r == "Propre") ? "green" : "red";

    QMessageBox::information(this, "Classification",
                             "La zone est : <b><font color='" + couleur + "'>" + r + "</font></b>");
}

// --------------------------------------------------------------
// CALENDAR
// --------------------------------------------------------------




void MainWindow::checkEventNotifications()
{
    QDate today = QDate::currentDate();
    if (!events.contains(today)) return;

    for (const QString &evt : events[today])
        QMessageBox::information(this, "Rappel", "Aujourd'hui : " + evt);
}
// --------------------------------------------------------------
// CALENDAR - DATABASE INTEGRATION
// --------------------------------------------------------------
void MainWindow::loadEventsFromDb()
{
    events.clear();

    QSqlQuery q("SELECT ZONE, DATE_EVENT, ACTION FROM TABLE1");
    while (q.next()) {
        QString zone = q.value(0).toString();
        QDate d = q.value(1).toDate();
        QString action = q.value(2).toString();

        // Format affichage : "Zone: Action"
        events[d].append(zone + ": " + action);
    }

    updateCalendarHighlights();
    updateEventList();
}

void MainWindow::updateEventList()
{
    eventList->clear();
    QDate d = calendar->selectedDate();
    if (events.contains(d))
        eventList->addItems(events[d]);
}

void MainWindow::toggleCalendar()
{
    isCalendarVisible = !isCalendarVisible;

    calendar->setVisible(isCalendarVisible);
    eventList->setVisible(isCalendarVisible);
    addEventBtn->setVisible(isCalendarVisible);
    removeEventBtn->setVisible(isCalendarVisible);

    toggleCalendarBtn->setText(isCalendarVisible ? "Masquer Calendrier" : "Afficher Calendrier");

    if (isCalendarVisible)
        loadEventsFromDb();
}



void MainWindow::setupCalendarConnections()
{
    connect(calendar, &QCalendarWidget::selectionChanged, this, &MainWindow::updateEventList);

    connect(addEventBtn, &QPushButton::clicked, this, [=]() {
        QDate d = calendar->selectedDate();
        QString zone = QInputDialog::getText(this, "Zone", "Nom de la zone :");
        QString action = QInputDialog::getText(this, "Action", "Action :");

        if (!zone.isEmpty() && !action.isEmpty()) {
            QSqlQuery q;
            q.prepare("INSERT INTO TABLE1 (ZONE, DATE_EVENT, ACTION) VALUES (:zone, :d, :action)");
            q.bindValue(":zone", zone);
            q.bindValue(":d", d);
            q.bindValue(":action", action);

            if (q.exec()) {
                loadEventsFromDb();
            } else {
                QMessageBox::warning(this, "Erreur", q.lastError().text());
            }
        }
    });

    connect(removeEventBtn, &QPushButton::clicked, this, [=]() {
        auto selected = eventList->selectedItems();
        QDate d = calendar->selectedDate();

        for (auto item : selected) {
            QString text = item->text(); // Format "ZONE: ACTION"
            QStringList parts = text.split(": ");
            if (parts.size() == 2) {
                QString zone = parts[0];
                QString action = parts[1];

                QSqlQuery q;
                q.prepare("DELETE FROM TABLE1 WHERE ZONE = :zone AND DATE_EVENT = :d AND ACTION = :action");
                q.bindValue(":zone", zone);
                q.bindValue(":d", d);
                q.bindValue(":action", action);

                if (q.exec()) {
                    events[d].removeOne(text);
                    delete item;
                } else {
                    QMessageBox::warning(this, "Erreur", q.lastError().text());
                }
            }
        }
        updateCalendarHighlights();
    });
}

// --------------------------------------------------------------
// In MainWindow constructor, add:
// --------------------------------------------------------------
//setupCalendarConnections();
//loadEventsFromDb();


// --------------------------------------------------------------
// SEARCH + SORT
// --------------------------------------------------------------
void MainWindow::setupSearchAndSort()
{
    if (!proxyModel)
        proxyModel = new QSortFilterProxyModel(this);

    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setDynamicSortFilter(true);

    if (tableView->model())
        proxyModel->setSourceModel(tableView->model());

    tableView->setModel(proxyModel);
    tableView->setSortingEnabled(true);

    connect(searchEdit, &QLineEdit::textChanged, this,
            [=](const QString &text) { proxyModel->setFilterFixedString(text); });
}

void MainWindow::ouvrirTri()
{
    SortDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        if (!proxyModel)
            proxyModel = new QSortFilterProxyModel(this);

        int col = dialog.primaryColumn();
        Qt::SortOrder order = dialog.primaryOrder();

        proxyModel->sort(col, order);
        tableView->sortByColumn(col, order);

        int sec = dialog.secondaryColumn();
        if (sec != -1)
            proxyModel->sort(sec, dialog.secondaryOrder());
    }
}

// --------------------------------------------------------------
// PDF EXPORT
// --------------------------------------------------------------
void MainWindow::exportToPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "PDF (*.pdf)");

    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf")) fileName += ".pdf";

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(300);

    QPainter painter(&writer);

    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(QRect(0, 0, writer.width(), 50), Qt::AlignCenter, "Rapport - Zones Vertes");

    int y = 70;

    QPixmap tablePixmap(tableView->viewport()->size());
    tableView->viewport()->render(&tablePixmap);

    QPixmap scaled = tablePixmap.scaledToWidth(writer.width() - 40, Qt::SmoothTransformation);
    painter.drawPixmap(20, y, scaled);

    y += scaled.height() + 20;

    QSqlQuery q("SELECT COUNT(*), NVL(SUM(SUPERFICIE),0) FROM ZONES_VERTES");

    if (q.next()) {
        QString summary =
            "Total zones: " + q.value(0).toString() +
            "   |   Superficie totale: " + q.value(1).toString() + " ha";

        painter.setFont(QFont("Arial", 10));
        painter.drawText(20, y, summary);
    }

    painter.end();

    QMessageBox::information(this, "PDF Export", "PDF exporté avec succès !");
}

// --------------------------------------------------------------
// ENVIRONMENTAL CALCULATIONS
// --------------------------------------------------------------
double MainWindow::computeOxygenProduction(double superficieHa)
{
    return superficieHa * 1000.0; // kg/year
}

double MainWindow::computeCO2Reduction(double superficieHa)
{
    return superficieHa * 6000.0; // kg/year
}

// --------------------------------------------------------------
// SHOW DIALOG
// --------------------------------------------------------------
void MainWindow::showEcoConclusionDialog()
{
    // 1. Read total superficies
    QSqlQuery q1("SELECT NVL(SUM(SUPERFICIE),0) FROM ZONES_VERTES");
    q1.next();
    double superficieTotale = q1.value(0).toDouble();

    // 2. Read number of zones
    QSqlQuery q2("SELECT COUNT(*) FROM ZONES_VERTES");
    q2.next();
    int totalZones = q2.value(0).toInt();

    // 3. Compute values
    double oxygen = computeOxygenProduction(superficieTotale);
    double co2 = computeCO2Reduction(superficieTotale);

    QString msg =
        "Analyse Écologique:\n\n"
        "Superficie totale : " + QString::number(superficieTotale) + " ha\n"
                                              "Nombre de zones : " + QString::number(totalZones) + "\n\n"
                                        "Production estimée d'Oxygène : " + QString::number(oxygen) + " kg/an\n"
                                    "Absorption estimée de CO₂ : " + QString::number(co2) + " kg/an\n\n"
                                 "Conclusion :\n"
                                 "Une grande superficie végétale augmente la production d'oxygène et réduit "
                                 "significativement le CO₂, améliorant la qualité environnementale de la ville.";

    QMessageBox::information(this, "Analyse Environnementale", msg);
}

void MainWindow::updateCalendarHighlights()
{
    //QPalette pal = calendarWidget->palette();
    QTextCharFormat highlight;
    highlight.setBackground(QColor("#81c784"));  // Light green
    highlight.setForeground(Qt::white);          // Text color on highlighted day

    QTextCharFormat clear; // default

    // Clear previous formats
    for (const QDate &d : events.keys())
        calendar->setDateTextFormat(d, clear);

    // Apply highlights
    for (auto it = events.begin(); it != events.end(); ++it) {
        if (!it.value().isEmpty())
            calendar->setDateTextFormat(it.key(), highlight);
    }
}
