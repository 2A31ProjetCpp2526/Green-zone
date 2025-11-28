#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QSortFilterProxyModel>
#include <QMainWindow>
#include <QMap>
#include <QDate>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QWidget;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableView;
class QCalendarWidget;
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QTimer;
QT_END_NAMESPACE

#include "zone.h"
#include "poubelle.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // CRUD Actions
    void afficherZones();
    void ajouterZone();
    void modifierZone();
    void supprimerZone();

    // Stats
    void calculerTotalZones();
    void calculerTotalSuperficie();

    // Poubelle classifier
    void classifierZonePoubelle();

    // Calendar actions
    void toggleCalendar();
    void updateCalendarHighlights();
    void checkEventNotifications();

    // Export
    void exportToPdf();

    void openChartDialog();





private:
    // UI Builders
    void setupUI();
    void setupSidebar();
    void setupForm();
    void setupButtons();
    void setupTable();
    void setupConnections();
    void ouvrirTri();
    void setupSearchAndSort() ;
    void showEcoConclusionDialog();
    double computeOxygenProduction(double superficieHa);
    double computeCO2Reduction(double superficieHa);
    //QtCharts::QChartView *chartView;




    // Data
    Zone Ztmp;
    QMap<QDate, QStringList> events;
    bool isCalendarVisible = false;

    // Sidebar
    QWidget *sidebarWidget;
    QVBoxLayout *sidebarLayout;
    QPushButton *calcTotalZonesBtn;
    QPushButton *calcTotalAreaBtn;
    QPushButton *classifierPoubelleBtn;
    QPushButton *toggleCalendarBtn;
    QPushButton *ecoAnalysisBtn;
private:
    // ------------------- Calendrier -------------------

    QSortFilterProxyModel *proxyModel;  // Pour la recherche et le tri dynamique

    QCalendarWidget *calendar;
    QListWidget *eventList;
    QPushButton *addEventBtn;
    QPushButton *removeEventBtn;
    //bool isCalendarVisible;

    // Structure pour stocker les événements en mémoire
    //QMap<QDate, QStringList> events;

    // Méthodes
    void loadEventsFromDb();            // Charger les événements depuis la base
    void updateEventList();             // Mettre à jour la liste des événements affichée
    //void updateCalendarHighlights();    // Surligner les dates avec événements
    void setupCalendarConnections();    // Connecter les boutons et le calendrier

    QPushButton *btnShowChart;


    // Form fields
    QGridLayout *formLayout;
    QLineEdit *idEdit;
    QLineEdit *nomEdit;
    QLineEdit *typeEdit;
    QLineEdit *supEdit;
    QLineEdit *locEdit;
    QLineEdit *respEdit;
    QLineEdit *etatEdit;

    // Info labels
    QLabel *totalZonesLabel;
    QLabel *totalAreaLabel;

    // Search bar
    QLineEdit *searchEdit;
    QPushButton *searchClearBtn;

    // Table
    QTableView *tableView;

    // Table buttons
    QHBoxLayout *btnLayout;
    QPushButton *addBtn;
    QPushButton *updateBtn;
    QPushButton *deleteBtn;
    QPushButton *refreshBtn;
    QPushButton *exportPdfBtn;
    QPushButton *refreshChartBtn;


    // Calendar & events
    //QCalendarWidget *calendar;
    //QListWidget *eventList;
    //QPushButton *addEventBtn;
    //QPushButton *removeEventBtn;
    //QSortFilterProxyModel *proxyModel;
    QPushButton *sortBtn;

    // Notification timer
    QTimer *notificationTimer;
};

#endif // MAINWINDOW_H
