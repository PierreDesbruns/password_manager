// Copyright (C) 2024 Pierre Desbruns
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef ADDENTRYWINDOW_H
#define ADDENTRYWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>


class AddEntryWindow : public QDialog
{
    Q_OBJECT

public:
    AddEntryWindow();
    QString getEntryname() const { return entrynameLine->text(); }
    QString getUsername() const { return usernameLine->text(); }
    int getPasswordLength() const { return passwordLengthBox->value(); }
    bool hasLowCase() const { return enableLowCaseBox->isChecked(); }
    bool hasUpCase() const { return enableUpCaseBox->isChecked(); }
    bool hasNumbers() const { return enableNumbersBox->isChecked(); }
    bool hasSpecials() const { return enableSpecialsBox->isChecked(); }
    void setEntrynameMaxLength(const int length) { entrynameLine->setMaxLength(length); }
    void setUsernameMaxLength(const int length) { usernameLine->setMaxLength(length); }
    void setPasswordMaxLength(const int length) { passwordLengthBox->setMaximum(length); }
    /**
     * @brief Clear value of every field.
     *
     * Used to provide a standard window everytime it is open.
     *
     * @note Must be called before show().
     */
    void clearFields() const;

private slots:
    /**
     * @brief Call accept() after verifications.
     *
     * Called when confirm button is pressed.
     * Verifications:
     * 1. Entry and user names must not be empty.
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

    QLabel *entrynameLabel;
    QLabel *usernameLabel;
    QLabel *passwordLengthLabel;

    QLineEdit *entrynameLine;
    QLineEdit *usernameLine;
    QSpinBox *passwordLengthBox;

    QCheckBox *enableLowCaseBox;
    QCheckBox *enableUpCaseBox;
    QCheckBox *enableNumbersBox;
    QCheckBox *enableSpecialsBox;

    QPushButton *confirmButton;
    QPushButton *cancelButton;
};

#endif // ADDENTRYWINDOW_H
