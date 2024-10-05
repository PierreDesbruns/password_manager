// Copyright (C) 2024 Pierre Desbruns
// SPDX-License-Identifier: LGPL-3.0-only

#include "loginwindow.h"

LoginWindow::LoginWindow()
{
    setWindowTitle(tr("Authentification"));
    setFixedSize(windowSmallSize);

    passwordLabel = new QLabel(QString(tr("Mot de passe")));
    newPasswordLabel = new QLabel(QString(tr("Nouveau")));
    confirmNewPasswordLabel = new QLabel(QString(tr("Confirmation")));

    passwordLine = new QLineEdit();
    passwordLine->setEchoMode(QLineEdit::Password);
    passwordLine->setMaxLength(MASTER_MAXLEN);

    newPasswordLine = new QLineEdit();
    newPasswordLine->setEchoMode(QLineEdit::Password);
    newPasswordLine->setMaxLength(MASTER_MAXLEN);

    confirmNewPasswordLine = new QLineEdit();
    confirmNewPasswordLine->setEchoMode(QLineEdit::Password);
    confirmNewPasswordLine->setMaxLength(MASTER_MAXLEN);

    confirmButton = new QPushButton(QString(tr("Confirmer")));
    cancelButton = new QPushButton(QString(tr("Annuler")));
    changePwdButton = new QPushButton(QString(tr("Changer")));
    confirmButton->setDefault(true);

    formLayout = new QFormLayout();
    formLayout->addRow(passwordLabel, passwordLine);
    formLayout->addRow(newPasswordLabel, newPasswordLine);
    formLayout->addRow(confirmNewPasswordLabel, confirmNewPasswordLine);
    formLayout->setRowVisible(1, false);
    formLayout->setRowVisible(2, false);

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(changePwdButton);
    buttonsLayout->addWidget(confirmButton);
    buttonsLayout->addWidget(cancelButton);

    mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonsLayout);

    mainContent = new QWidget(this);
    mainContent->setFixedSize(windowSmallSize);
    mainContent->setLayout(mainLayout);

    connect(changePwdButton, SIGNAL(pressed()), this, SLOT(changePassword()));
    connect(confirmButton, SIGNAL(pressed()), this, SLOT(verifications()));
    connect(cancelButton, SIGNAL(pressed()), this, SLOT(reject()));
}

void LoginWindow::verifications()
{
    const QString password = passwordLine->text();

    // Uncomment only if master password hash file is empty.
    // pwm::updateMasterHash(password);

    if (!pwm::masterIsCorrect(password))
    {
        // Wrong password
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Mot de passe incorrect.")
            );
        return;
    }

    if (passwordChanged)
    {
        const QString newPassword = newPasswordLine->text();
        const int newPasswordLength = newPassword.size();

        if (newPassword != confirmNewPasswordLine->text())
        {
            // New password and confirmation do not match
            QMessageBox::critical(
                this,
                this->windowTitle(),
                tr("Veuillez entrer deux fois le même mot de passe.")
            );
            return;
        }

        if (newPasswordLength <= MASTER_MINLEN)
        {
            // Passwword too short (too big covered with line max length, cf. constructor)
            QMessageBox::critical(
                this,
                this->windowTitle(),
                tr("Mot de passe trop court.")
            );
            return;
        }

        pwm::updateMasterHash(newPassword);

        QMessageBox::information(
            this,
            this->windowTitle(),
            tr("Mot de passe modifié avec succès.")
            );
    }

    accept();
}

void LoginWindow::changePassword()
{
    // Resizing window
    setFixedSize(windowLargeSize);
    mainContent->setFixedSize(windowLargeSize);

    // Showing widgets
    formLayout->setRowVisible(1, true);
    formLayout->setRowVisible(2, true);
    passwordLabel->setText(tr("Ancien"));
    changePwdButton->setVisible(false);

    // Updating flag
    passwordChanged = true;
}
