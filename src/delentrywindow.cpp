#include "delentrywindow.h"

DelEntryWindow::DelEntryWindow()
{
    setWindowTitle("Supprimer une entrée");
    setFixedSize(windowSize);

    entrynameLabel = new QLabel(QString(tr("Nom de l'entrée")));
    usernameLabel = new QLabel(QString(tr("Nom d'utilisateur")));

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

    confirmButton = new QPushButton(QString(tr("Supprimer")));
    cancelButton = new QPushButton(QString(tr("Annuler")));

    formLayout = new QFormLayout;
    formLayout->addRow(entrynameLabel, entrynameBox);
    formLayout->addRow(usernameLabel, usernameBox);

    buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(confirmButton);
    buttonsLayout->addWidget(cancelButton);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonsLayout);

    mainContent = new QWidget(this);
    mainContent->setFixedSize(windowSize);
    mainContent->setLayout(mainLayout);

    connect(confirmButton, SIGNAL(pressed()), this, SLOT(verifications()));
    connect(cancelButton, SIGNAL(pressed()), this, SLOT(reject()));
    connect(entrynameBox, SIGNAL(currentTextChanged(QString)), this, SLOT(updateUsernameBox(QString)));
}

void DelEntryWindow::updateEntries(const QStringList &entrynames, const QStringList &usernames)
{
    fullEntrynameList = entrynames;
    fullUsernameList = usernames;

    displayedEntrynameList = entrynames;
    displayedEntrynameList.removeDuplicates();
    displayedEntrynameList.sort(Qt::CaseInsensitive);

    entrynameModel->setStringList(displayedEntrynameList); // updating completer's model

    entrynameBox->clear();
    entrynameBox->addItems(displayedEntrynameList);
}

void DelEntryWindow::verifications()
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
    else accept();
}

void DelEntryWindow::updateUsernameBox(const QString &entryBoxText) const
{
    usernameBox->clear();

    int currentIndex = fullEntrynameList.indexOf(entryBoxText);

    // Iterating over [fullEntrynameList].
    // Each time an entry name is same as [entryBoxText],
    // the corresponding username is added to [usernameBox].
    while (currentIndex != -1)
    {
        usernameBox->addItem(fullUsernameList[currentIndex]);
        currentIndex = fullEntrynameList.indexOf(entryBoxText, currentIndex+1);
    }
}
