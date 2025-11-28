// sortdialog.cpp
#include "sortdialog.h"
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

SortDialog::SortDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Tri Avancé");
    setFixedSize(300, 250);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- Primary sort ---
    QLabel *pLabel = new QLabel("Colonne principale :");
    primaryCombo = new QComboBox();
    primaryCombo->addItems({"ID", "Nom", "Type", "Superficie", "Localisation", "Responsable", "État"});

    primaryAsc = new QRadioButton("Croissant");
    primaryDesc = new QRadioButton("Décroissant");
    primaryAsc->setChecked(true);
    primaryGroup = new QButtonGroup(this);
    primaryGroup->addButton(primaryAsc);
    primaryGroup->addButton(primaryDesc);

    QHBoxLayout *pOrderLayout = new QHBoxLayout();
    pOrderLayout->addWidget(primaryAsc);
    pOrderLayout->addWidget(primaryDesc);

    mainLayout->addWidget(pLabel);
    mainLayout->addWidget(primaryCombo);
    mainLayout->addLayout(pOrderLayout);

    // --- Secondary sort ---
    QLabel *sLabel = new QLabel("Colonne secondaire :");
    secondaryCombo = new QComboBox();
    secondaryCombo->addItem("Aucune"); // index 0 = none
    secondaryCombo->addItems({"ID", "Nom", "Type", "Superficie", "Localisation", "Responsable", "État"});

    secondaryAsc = new QRadioButton("Croissant");
    secondaryDesc = new QRadioButton("Décroissant");
    secondaryAsc->setChecked(true);
    secondaryGroup = new QButtonGroup(this);
    secondaryGroup->addButton(secondaryAsc);
    secondaryGroup->addButton(secondaryDesc);

    QHBoxLayout *sOrderLayout = new QHBoxLayout();
    sOrderLayout->addWidget(secondaryAsc);
    sOrderLayout->addWidget(secondaryDesc);

    mainLayout->addWidget(sLabel);
    mainLayout->addWidget(secondaryCombo);
    mainLayout->addLayout(sOrderLayout);

    // --- OK / Cancel ---
    QHBoxLayout *btnLayout = new QHBoxLayout();
    okBtn = new QPushButton("OK");
    cancelBtn = new QPushButton("Annuler");
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, &SortDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &SortDialog::reject);
}

// --- Getters ---
int SortDialog::primaryColumn() const
{
    return primaryCombo->currentIndex();
}

Qt::SortOrder SortDialog::primaryOrder() const
{
    return primaryAsc->isChecked() ? Qt::AscendingOrder : Qt::DescendingOrder;
}

int SortDialog::secondaryColumn() const
{
    int idx = secondaryCombo->currentIndex() - 1; // -1 = Aucune
    return (idx >= 0) ? idx : -1;
}

Qt::SortOrder SortDialog::secondaryOrder() const
{
    return secondaryAsc->isChecked() ? Qt::AscendingOrder : Qt::DescendingOrder;
}


