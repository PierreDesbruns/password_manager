#include "regentrywindow.h"

RegEntryWindow::RegEntryWindow()
{
    setWindowTitle(tr("Re-générer une entrée"));
    setFixedSize(windowSize);

    entrynameLabel = new QLabel(QString(tr("Nom de l'entrée")));
    usernameLabel = new QLabel(QString(tr("Nom d'utilisateur")));
    passwordLengthLabel = new QLabel(QString(tr("Nombre de caractères")));

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
    formLayout->addRow(entrynameLabel, entrynameBox);
    formLayout->addRow(usernameLabel, usernameBox);
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

void RegEntryWindow::verifications()
{
    if (usernameBox->currentText().isEmpty())
    {
        // Username is empty if, and only if entry name is incorrect.
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Veuillez préciser une entrée correcte.")
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
