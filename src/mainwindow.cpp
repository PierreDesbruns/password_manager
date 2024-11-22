// Copyright (C) 2024 Pierre Desbruns
// SPDX-License-Identifier: LGPL-3.0-only

#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(windowSize);
    setWindowTitle(tr("Gestionnaire de mots de passe"));
    setWindowIcon(QIcon(":/logo"));

    clipboard = QApplication::clipboard();

    addButton = new QPushButton(tr("Ajouter"));
    delButton = new QPushButton(tr("Supprimer"));
    regButton = new QPushButton(tr("Re-générer"));

    searchModel = new QStringListModel;

    searchCompleter = new QCompleter;
    searchCompleter->setModel(searchModel);
    searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    searchCompleter->setCompletionMode(QCompleter::InlineCompletion);

    searchBar = new QLineEdit();
    searchBar->setCompleter(searchCompleter);

    entryTable = new QTableWidget(0,3);
    entryTable->horizontalHeader()->setVisible(false);
    entryTable->verticalHeader()->setVisible(false);
    entryTable->setColumnWidth(0,120);
    entryTable->setColumnWidth(1,120);
    entryTable->setColumnWidth(2,120);

    loginWindow = new LoginWindow();
    loginWindow->setWindowIcon(windowIcon());
    loginWindow->setModal(Qt::ApplicationModal);

    addWindow = new AddEntryWindow();
    addWindow->setWindowIcon(windowIcon());
    addWindow->setModal(Qt::ApplicationModal);
    addWindow->setEntrynameMaxLength(ENTRYNAME_MAXLEN);
    addWindow->setUsernameMaxLength(USERNAME_MAXLEN);
    addWindow->setPasswordMaxLength(PASSWORD_MAXLEN);

    delWindow = new DelEntryWindow();
    delWindow->setWindowIcon(windowIcon());
    delWindow->setModal(Qt::ApplicationModal);

    regWindow = new RegEntryWindow();
    regWindow->setWindowIcon(windowIcon());
    regWindow->setModal(Qt::ApplicationModal);
    regWindow->setPasswordMaxLength(PASSWORD_MAXLEN);

    mainLayout = new QGridLayout;
    mainLayout->addWidget(addButton,0,0);
    mainLayout->addWidget(delButton,0,1);
    mainLayout->addWidget(regButton,0,2);
    mainLayout->addWidget(searchBar,1,0,1,3);
    mainLayout->addWidget(entryTable,2,0,1,3);

    mainContent = new QWidget();
    mainContent->setLayout(mainLayout);
    setCentralWidget(mainContent);

    connect(entryTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(copyCell(int,int)));
    connect(searchBar, SIGNAL(textChanged(QString)), this, SLOT(updateTable(QString)));
    connect(addButton, SIGNAL(pressed()), this, SLOT(showAddWindow()));
    connect(delButton, SIGNAL(pressed()), this, SLOT(showDelWindow()));
    connect(regButton, SIGNAL(pressed()), this, SLOT(showRegWindow()));
    connect(addWindow, SIGNAL(accepted()), this, SLOT(addEntry()));
    connect(delWindow, SIGNAL(accepted()), this, SLOT(delEntry()));
    connect(regWindow, SIGNAL(accepted()), this, SLOT(regEntry()));
    connect(loginWindow, SIGNAL(accepted()), this, SLOT(loadEntries()));
    connect(loginWindow, SIGNAL(rejected()), this, SLOT(close()));

    loginWindow->show();
}

MainWindow::~MainWindow()
{
    // Clearing clipboard when closing window if it contains a password.
    if (passwords.contains(clipboard->text())) clipboard->clear();

    // Uncomment only if crypto parameters file is empty or parameters need to be changed
    //pwm::updateCryptoParams();

    // Encrypting with new master if modified
    // pwm::writeEntries(loginWindow->getNewPassword(), entrynames, usernames, passwords, dates);
}

void MainWindow::copyCell(const int row, const int col) const
{
    if (col == 1) // column for usernames
        clipboard->setText(usernames[row]);
    else if (col == 2) // column for passwords
        clipboard->setText(passwords[row]);
}

void MainWindow::updateTable() const
{
    entryTable->setRowCount(0); // clearing table

    int nbRows = entrynames.size(); // <=> nb of entries
    int nbCols = entryTable->columnCount(); // should be 3

    for (int row = 0 ; row < nbRows ; ++row)
    {
        entryTable->insertRow(row);
        entryTable->setItem(row,0,new QTableWidgetItem(iconFrom(dates[row]),entrynames[row]));
        entryTable->setItem(row,1,new QTableWidgetItem(usernames[row]));
        entryTable->setItem(row,2,new QTableWidgetItem(QString("***************")));
        entryTable->setRowHeight(row,20);

        for (int col = 0 ; col < nbCols ; ++col)
            entryTable->item(row,col)->setFlags(Qt::ItemIsEnabled);
    }
}

void MainWindow::updateTable(const QString &entryname) const
{
    if (!entrynames.contains(entryname))
        updateTable(); // resetting table
    else
    {
        entryTable->setRowCount(0); // clearing table

        int currentIndex = entrynames.indexOf(entryname);

        int row = 0; // <=> each entry
        int nbCols = entryTable->columnCount(); // should be 3

        while (currentIndex != -1)
        {
            entryTable->insertRow(row);
            entryTable->setItem(row,0,new QTableWidgetItem(iconFrom(dates[currentIndex]), entrynames[currentIndex]));
            entryTable->setItem(row,1,new QTableWidgetItem(usernames[currentIndex]));
            entryTable->setItem(row,2,new QTableWidgetItem(QString("***************")));
            entryTable->setRowHeight(row,20);

            // Disabling modification for each cell
            for (int col = 0 ; col < nbCols ; ++col)
                entryTable->item(row,col)->setFlags(Qt::ItemIsEnabled);

            currentIndex = entrynames.indexOf(entryname, currentIndex+1);
            ++row;
        }
    }
}

void MainWindow::showAddWindow() const
{
    addWindow->clearFields();
    addWindow->show();
}

void MainWindow::showDelWindow() const
{
    delWindow->updateEntries(entrynames, usernames);
    delWindow->show();
}

void MainWindow::showRegWindow() const
{
    regWindow->updateEntries(entrynames, usernames);
    regWindow->show();
}

void MainWindow::loadEntries()
{
    // Temporary variables
    QStringList tempEntrynames = entrynames;
    QStringList tempUsernames = usernames;
    QStringList tempPasswords = passwords;
    QStringList tempDates = dates;

    QStringList entries = pwm::readEntries(loginWindow->getPassword());

    if (entries.isEmpty())
    {
        // Empty file or error in entries file reading
        qWarning() << "No entry loaded. Entry file may be empty.";
        return;
    }

    // Updating list attributes
    for (const auto &entry : entries)
    {
        QStringList entryFields = entry.split('\t');
        if (entryFields.size() != 4)
            qWarning() << "Format of entry" << entries.indexOf(entry) <<  "is incorrect. Skipped entry.";
        else
        {
            tempEntrynames.append(entryFields[0]);
            tempUsernames.append(entryFields[1]);
            tempPasswords.append(entryFields[2]);
            tempDates.append(entryFields[3]);
        }
    }

    // Re-writing file in case master password has changed
    if (pwm::writeEntries(loginWindow->getNewPassword(), tempEntrynames, tempUsernames, tempPasswords, tempDates) != 0)
    {
        // Error in file writing
        qCritical() << "Error in entries writing. Could not encrypt entries with new password.";
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Une erreur est survenue lors de la prise en compte du nouveau mot de passe.\n"
               "Si vous souhaitez conserver vos entrées, fermez l'application SANS AJOUTER\n"
               "D'ENTREE et revenez à l'ancien mot de passe.\n"
               "Sinon, toute entrée ajoutée écrasera les entrées précédemment enregistrées."
               )
            );
        return;
    }

    // Updating entries attributes
    entrynames = tempEntrynames;
    usernames = tempUsernames;
    passwords = tempPasswords;
    dates = tempDates;

    searchModel->setStringList(entrynames);
    updateTable();
}

void MainWindow::addEntry()
{
    // User inputs
    QString entryname = addWindow->getEntryname();
    QString username = addWindow->getUsername();
    int passwordLength = addWindow->getPasswordLength();
    bool hasLowCase = addWindow->hasLowCase();
    bool hasUpCase = addWindow->hasUpCase();
    bool hasNumbers = addWindow->hasNumbers();
    bool hasSpecials = addWindow->hasSpecials();

    // Temporary variables
    QStringList tempEntrynames = entrynames;
    QStringList tempUsernames = usernames;
    QStringList tempPasswords = passwords;
    QStringList tempDates = dates;

    if (exists(entryname, username))
    {
        QMessageBox::warning(
            this,
            addWindow->windowTitle(),
            tr("L'entrée existe déjà.")
            );

        // Showing existing entry to the user
        searchBar->setText(entryname);

        return;
    }

    QString password = pwm::generatePassword(passwordLength, hasLowCase, hasUpCase, hasNumbers, hasSpecials);

    if (password.isEmpty())
    {
        // Error in password generation
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Une erreur est survenue lors de la génération du mot de passe.\n"
               "L'entrée suivante n'a pas été ajoutée:\n\n"
               "\t%1\n"
               "\t%2"
               ).arg(entryname, username)
            );
        return;
    }

    tempEntrynames.append(entryname);
    tempUsernames.append(username);
    tempPasswords.append(password);
    tempDates.append(QDate::currentDate().toString("yyyy.MM.dd"));

    // Writing entries in file
    if (pwm::writeEntries(loginWindow->getNewPassword(), tempEntrynames, tempUsernames, tempPasswords, tempDates) != 0)
    {
        // Error in file writing
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Une erreur est survenue lors de l'enregistrement des entrées.\n"
               "L'entrée suivante n'a pas été ajoutée:\n\n"
               "\t%1\n"
               "\t%2"
               ).arg(entryname, username)
            );
        return;
    }

    // Updating entries attributes
    entrynames = tempEntrynames;
    usernames = tempUsernames;
    passwords = tempPasswords;
    dates = tempDates;

    QMessageBox::information(
        this,
        this->windowTitle(),
        tr("Entrée ajoutée avec succès.")
        );

    searchModel->setStringList(entrynames);
    updateTable();
}

void MainWindow::delEntry()
{
    // User inputs
    QString entryname = delWindow->getEntryname();
    QString username = delWindow->getUsername();
    int indexToRemove = entrynames.indexOf(entryname);

    // Temporary variables
    QStringList tempEntrynames = entrynames;
    QStringList tempUsernames = usernames;
    QStringList tempPasswords = passwords;
    QStringList tempDates = dates;

    while (indexToRemove != -1)
    {
        if (tempUsernames[indexToRemove] == username)
        {
            // Should be executed only once because entries cannot contains same entry name and user name
            tempEntrynames.removeAt(indexToRemove);
            tempUsernames.removeAt(indexToRemove);
            tempPasswords.removeAt(indexToRemove);
            tempDates.removeAt(indexToRemove);
        }
        indexToRemove = tempEntrynames.indexOf(entryname, indexToRemove+1);
    }

    // Writing entries in file
    if (pwm::writeEntries(loginWindow->getNewPassword(), tempEntrynames, tempUsernames, tempPasswords, tempDates) != 0)
    {
        // Error in file writing
        qCritical() << "Failed to write entry. Entry" << entryname << username << "not added.";
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Une erreur est survenue lors l'enregistrement des entrées.\n"
               "L'entrée suivante n'a pas été ajoutée:\n\n"
               "\t%1\n"
               "\t%2"
               ).arg(entryname, username)
            );
        return;
    }

    // Updating entries attributes
    entrynames = tempEntrynames;
    usernames = tempUsernames;
    passwords = tempPasswords;
    dates = tempDates;

    QMessageBox::information(
        this,
        this->windowTitle(),
        tr("Entrée supprimée avec succès.")
        );

    searchModel->setStringList(entrynames);
    updateTable();
}

void MainWindow::regEntry()
{
    // User inputs
    QString entryname = regWindow->getEntryname();
    QString username = regWindow->getUsername();
    int passwordLength = regWindow->getPasswordLength();
    bool hasLowCase = regWindow->hasLowCase();
    bool hasUpCase = regWindow->hasUpCase();
    bool hasNumbers = regWindow->hasNumbers();
    bool hasSpecials = regWindow->hasSpecials();
    int indexToReset = entrynames.indexOf(entryname);

    // Temporary variables
    QStringList tempEntrynames = entrynames;
    QStringList tempUsernames = usernames;
    QStringList tempPasswords = passwords;
    QStringList tempDates = dates;

    QString password = pwm::generatePassword(passwordLength, hasLowCase, hasUpCase, hasNumbers, hasSpecials);

    if (password.isEmpty())
    {
        // Error in password generation
        qWarning() << "No password was generated. Kept entry" << entryname << username << "with old password.";
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Une erreur est survenue lors de la génération du mot de passe.\n"
               "Le mot de passe de l'entrée suivante n'a pas été re-généré:\n\n"
               "\t%1\n"
               "\t%2"
               ).arg(entryname, username)
            );
        return;
    }

    while (indexToReset != -1)
    {
        if (tempUsernames[indexToReset] == username)
        {
            // Should be executed only once because entries cannot contains same entry name and user name
            tempPasswords[indexToReset] = password;
            tempDates[indexToReset] = QDate::currentDate().toString("yyyy.MM.dd");
        }
        indexToReset = tempEntrynames.indexOf(entryname, indexToReset+1);
    }

    // Writing entries in file
    if (pwm::writeEntries(loginWindow->getNewPassword(), tempEntrynames, tempUsernames, tempPasswords, tempDates) != 0)
    {
        // Error in file writing
        qCritical() << "Failed to write entry. Entry" << entryname << username << "not added.";
        QMessageBox::critical(
            this,
            this->windowTitle(),
            tr("Une erreur est survenue lors de l'enregistrement de l'entrée.\n"
               "Le mot de passe de l'entrée n'a pas été re-généré:\n\n"
               "\t%1\n"
               "\t%2"
               ).arg(entryname, username)
            );
        return;
    }

    // Updating entries attributes
    entrynames = tempEntrynames;
    usernames = tempUsernames;
    passwords = tempPasswords;
    dates = tempDates;

    QMessageBox::information(
        this,
        this->windowTitle(),
        tr("Mot de passe re-généré avec succès.")
        );

    updateTable();
}

QIcon MainWindow::iconFrom(const QString &date) const
{
    QDate currentDate = QDate::currentDate();
    QDate pwdDate = QDate::fromString(date, "yyyy.MM.dd");
    int monthDifference = 0; // number of months separating [currentDate] from [pwdDate]

    if (pwdDate.isNull())
    {
        // Date format is incorrect
        qWarning() << "Date format of " << date << "is incorrect. Date icon will not appear.";
        return QIcon();
    }

    monthDifference = (currentDate.year() - pwdDate.year()) * 12 + (currentDate.month() - pwdDate.month());

    if (monthDifference < 3) return QIcon(":/green");
    else if (monthDifference < 6) return QIcon(":/orange");

    return QIcon(":/red");
}

const bool MainWindow::exists(const QString &entryname, const QString &username) const
{
    int currentIndex = entrynames.indexOf(entryname);

    while (currentIndex != -1)
    {
        if (usernames[currentIndex] == username) return true;
        currentIndex = entrynames.indexOf(entryname, currentIndex+1);
    }
    return false;
}
