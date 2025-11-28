#ifndef SORTDIALOG_H
#define SORTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>

class SortDialog : public QDialog
{
    Q_OBJECT

public:
    SortDialog(QWidget *parent = nullptr);

    int primaryColumn() const;
    Qt::SortOrder primaryOrder() const;
    int secondaryColumn() const;
    Qt::SortOrder secondaryOrder() const;

private:
    QComboBox *primaryCombo;
    QRadioButton *primaryAsc;
    QRadioButton *primaryDesc;
    QButtonGroup *primaryGroup;

    QComboBox *secondaryCombo;
    QRadioButton *secondaryAsc;
    QRadioButton *secondaryDesc;
    QButtonGroup *secondaryGroup;

    QPushButton *okBtn;
    QPushButton *cancelBtn;
};

#endif // SORTDIALOG_H
