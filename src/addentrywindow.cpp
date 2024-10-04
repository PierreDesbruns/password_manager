// Copyright (C) 2024 Pierre Desbruns
// SPDX-License-Identifier: LGPL-3.0-only

#include "addentrywindow.h"

AddEntryWindow::AddEntryWindow()
{
    setWindowTitle(tr("Nouvelle entrée"));
    setFixedSize(windowSize);

    entrynameLabel = new QLabel(QString(tr("Nom de l'entrée")));
    usernameLabel = new QLabel(QString(tr("Nom d'utilisateur")));
    passwordLengthLabel = new QLabel(QString(tr("Nombre de caractères")));

    entrynameLine = new QLineEdit();
    usernameLine = new QLineEdit();
    passwordLengthBox = new QSpinBox();

    enableLowCaseBox = new QCheckBox(QString("a..z"));
    enableUpCaseBox = new QCheckBox(QString("A..Z"));
    enableNumbersBox = new QCheckBox(QString("0..9"));
    enableSpecialsBox = new QCheckBox(QString("$..!"));

    confirmButton = new QPushButton(QString(tr("Ajouter")));
    cancelButton = new QPushButton(QString(tr("Annuler")));

    formLayout = new QFormLayout();
    formLayout->addRow(entrynameLabel, entrynameLine);
    formLayout->addRow(usernameLabel, usernameLine);
    formLayout->addRow(passwordLengthLabel, passwordLengthBox);

    spinBoxesLayout = new QHBoxLayout();
    spinBoxesLayout->addWidget(enableLowCaseBox);
    spinBoxesLayout->addWidget(enableUpCaseBox);
    spinBoxesLayout->addWidget(enableNumbersBox);
    spinBoxesLayout->addWidget(enableSpecialsBox);

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(confirmButton);
    buttonsLayout->addWidget(cancelButton);

    mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(spinBoxesLayout);
    mainLayout->addLayout(buttonsLayout);

    mainContent = new QWidget(this);
    mainContent->setFixedSize(windowSize);
    mainContent->setLayout(mainLayout);

    connect(confirmButton, SIGNAL(pressed()), this, SLOT(verifications()));
    connect(cancelButton, SIGNAL(pressed()), this, SLOT(reject()));
}

void AddEntryWindow::clearFields() const
{
    entrynameLine->clear();
    usernameLine->clear();
    passwordLengthBox->setValue(0);
    enableLowCaseBox->setChecked(true);
    enableUpCaseBox->setChecked(true);
    enableNumbersBox->setChecked(true);
    enableSpecialsBox->setChecked(true);
}

void AddEntryWindow::verifications()
{
    if (entrynameLine->text().isEmpty())
    {
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Veuillez préciser le nom de l'entrée.")
        );
    }
    else if (usernameLine->text().isEmpty())
    {
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Veuillez préciser le nom d'utilisateur.")
        );
    }
    else if (passwordLengthBox->value() == 0)
    {
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Veuillez préciser la longueur du mot de passe.")
        );
    }
    else if (!enableLowCaseBox->isChecked() && !enableUpCaseBox->isChecked() && !enableNumbersBox->isChecked() && !enableSpecialsBox->isChecked())
    {
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Veuillez cocher au moins un type de caractère.")
        );
    }
    else accept();
}
