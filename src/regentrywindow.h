// Copyright (C) 2024 Pierre Desbruns
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef REGENTRYWINDOW_H
#define REGENTRYWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QPushButton>


class RegEntryWindow : public QDialog
{
    Q_OBJECT

public:
    RegEntryWindow(QWidget *parent = nullptr);
    QString getEntryname() const { return entrynameLabel->text(); }
    QString getUsername() const { return usernameLabel->text(); }
    int getPasswordLength() const { return passwordLengthBox->value(); }
    bool hasLowCase() const { return enableLowCaseBox->isChecked(); }
    bool hasUpCase() const { return enableUpCaseBox->isChecked(); }
    bool hasNumbers() const { return enableNumbersBox->isChecked(); }
    bool hasSpecials() const { return enableSpecialsBox->isChecked(); }
    void setPasswordMaxLength(const int length) { passwordLengthBox->setMaximum(length); }

public slots:
    /**
     * @brief Overwrite QDialog::open() to display entry and user names after clearing user input fields.
     * @param entryname: Name of entry to be re-generated.
     * @param username: Name of the user to be re-generated.
     */
    void open(const QString &entryname, const QString &username);

private slots:
    /**
     * @brief Call accept() after verifications.
     *
     * Called when confirm button is pressed.
     * Verifications:
     * 1. Entry name must be correct.
     * 2. Password length must be non zero.
     * 3. At least one box must be checked.
     */
    void verifications();

private:
    const QSize windowSize = QSize(300,150);

    QWidget *mainContent;

    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QHBoxLayout *spinBoxesLayout;
    QHBoxLayout *buttonsLayout;

    QLabel *entrynameFieldLabel;
    QLabel *usernameFieldLabel;
    QLabel *passwordLengthLabel;

    QLabel *entrynameLabel;
    QLabel *usernameLabel;

    QSpinBox *passwordLengthBox;

    QCheckBox *enableLowCaseBox;
    QCheckBox *enableUpCaseBox;
    QCheckBox *enableNumbersBox;
    QCheckBox *enableSpecialsBox;

    QPushButton *confirmButton;
    QPushButton *cancelButton;
};

#endif // REGENTRYWINDOW_H
