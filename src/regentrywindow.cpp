// Copyright (C) 2024 Pierre Desbruns
// SPDX-License-Identifier: LGPL-3.0-only

#include "regentrywindow.h"

RegEntryWindow::RegEntryWindow(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Re-générer une entrée"));
    setFixedSize(windowSize);

    entrynameFieldLabel = new QLabel(QString(tr("Nom de l'entrée")));
    usernameFieldLabel = new QLabel(QString(tr("Nom d'utilisateur")));
    passwordLengthLabel = new QLabel(QString(tr("Nombre de caractères")));

    entrynameLabel = new QLabel(QString());
    usernameLabel = new QLabel(QString());

    entrynameLine = new QLineEdit();

    entrynameModel = new QStringListModel;

    entrynameCompleter = new QCompleter;
    entrynameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    entrynameCompleter->setModel(entrynameModel);
    entrynameCompleter->setCompletionMode(QCompleter::PopupCompletion);

    entrynameBox = new QComboBox;
    entrynameBox->setLineEdit(entrynameLine);
    entrynameBox->setCompleter(entrynameCompleter);

    usernameBox = new QComboBox;

    passwordLengthBox = new QSpinBox();

    enableLowCaseBox = new QCheckBox(QString("a..z"));
    enableUpCaseBox = new QCheckBox(QString("A..Z"));
    enableNumbersBox = new QCheckBox(QString("0..9"));
    enableSpecialsBox = new QCheckBox(QString("$..!"));

    confirmButton = new QPushButton(QString(tr("Re-générer")));
    cancelButton = new QPushButton(QString(tr("Annuler")));

    formLayout = new QFormLayout();
    formLayout->addRow(entrynameFieldLabel, entrynameLabel);
    formLayout->addRow(usernameFieldLabel, usernameLabel);
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
    connect(entrynameBox, SIGNAL(currentTextChanged(QString)), this, SLOT(updateUsernameBox(QString)));
}

void RegEntryWindow::updateEntries(const QStringList &entries, const QStringList &usernames)
{
    fullEntrynameList = entries;
    fullUsernameList = usernames;

    displayedEntrynameList = entries;
    displayedEntrynameList.removeDuplicates();
    displayedEntrynameList.sort(Qt::CaseInsensitive);

    entrynameModel->setStringList(displayedEntrynameList); // updating completer's model

    entrynameBox->clear();
    entrynameBox->addItems(displayedEntrynameList);
    passwordLengthBox->setValue(0);
    enableLowCaseBox->setChecked(true);
    enableUpCaseBox->setChecked(true);
    enableNumbersBox->setChecked(true);
    enableSpecialsBox->setChecked(true);
}

void RegEntryWindow::open(const QString &entryname, const QString &username)
{
    // Clearing fields
    passwordLengthBox->setValue(0);
    enableLowCaseBox->setChecked(true);
    enableUpCaseBox->setChecked(true);
    enableNumbersBox->setChecked(true);
    enableSpecialsBox->setChecked(true);
    // Setting entry and user names
    entrynameLabel->setText(entryname);
    usernameLabel->setText(username);
    // Opening window
    QDialog::open();
}

void RegEntryWindow::verifications()
{
    if (passwordLengthBox->value() == 0)
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

void RegEntryWindow::updateUsernameBox(const QString &entryBoxText) const
{
    usernameBox->clear();

    int currentIndex = fullEntrynameList.indexOf(entryBoxText);

    // Iterating over [fullEntrynameList].
    // Each time an entry name is same as [entryBoxText] is found,
    // the corresponding username is added to [usernameBox].
    while (currentIndex != -1)
    {
        usernameBox->addItem(fullUsernameList[currentIndex]);
        currentIndex = fullEntrynameList.indexOf(entryBoxText, currentIndex+1);
    }
}
