// Copyright (C) 2025 Pierre Desbruns
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

    searchModel = new QStringListModel;

    searchCompleter = new QCompleter;
    searchCompleter->setModel(searchModel);
    searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    searchCompleter->setCompletionMode(QCompleter::InlineCompletion);

    searchBar = new QLineEdit();
    searchBar->setCompleter(searchCompleter);

    entryTable = new QTableWidget(0,6);
    entryTable->horizontalHeader()->setVisible(false);
    entryTable->verticalHeader()->setVisible(false);
    entryTable->setColumnWidth(0,120); // entry names
    entryTable->setColumnWidth(1,120); // user names
    entryTable->setColumnWidth(2,100); // passwords
    entryTable->setColumnWidth(3,20);  // edit buttons
    entryTable->setColumnWidth(4,20);  // re-generate buttons
    entryTable->setColumnWidth(5,20);  // delete buttons

    loginWindow = new LoginWindow();
    loginWindow->setWindowIcon(windowIcon());
    loginWindow->setModal(Qt::ApplicationModal);

    addWindow = new AddEntryWindow();
    addWindow->setWindowIcon(windowIcon());
    addWindow->setModal(Qt::ApplicationModal);
    addWindow->setEntrynameMaxLength(ENTRYNAME_MAXLEN);
    addWindow->setUsernameMaxLength(USERNAME_MAXLEN);
    addWindow->setPasswordMaxLength(PASSWORD_MAXLEN);

    regWindow = new RegEntryWindow(this);
    regWindow->setWindowIcon(windowIcon());
    regWindow->setPasswordMaxLength(PASSWORD_MAXLEN);

    mainLayout = new QGridLayout;
    mainLayout->addWidget(addButton,0,0,1,2);
    mainLayout->addWidget(searchBar,1,0,1,2);
    mainLayout->addWidget(entryTable,2,0,1,2);

    mainContent = new QWidget();
    mainContent->setLayout(mainLayout);
    setCentralWidget(mainContent);

    connect(entryTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(copyCell(int,int)));
    connect(entryTable, SIGNAL(cellClicked(int,int)), this, SLOT(buttonFromCell(int,int)));
    connect(this, SIGNAL(editEntryClicked(int)), this, SLOT(editEntry(int)));
    connect(searchBar, SIGNAL(textChanged(QString)), this, SLOT(updateTable(QString)));
    connect(addButton, SIGNAL(pressed()), this, SLOT(showAddWindow()));
    connect(addWindow, SIGNAL(accepted()), this, SLOT(addEntry()));
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
        clipboard->setText(entryTable->item(row,col)->text());
    else if (col == 2) // column for passwords
        clipboard->setText(passwords[indexOf(entryTable->item(row,0)->text(),entryTable->item(row,1)->text())]);
}

void MainWindow::buttonFromCell(const int row, const int col)
{
    switch (col)
    {
    case 3: emit editEntryClicked(row); break;
    case 4: regWindow->open(entryTable->item(row,0)->text(),entryTable->item(row,1)->text()); break;
    case 5: delEntry(row);
    default: break;
    }
}

void MainWindow::updateTable() const
{
    int nbRows = entrynames.size(); // <=> nb of entries

    entryTable->setRowCount(0); // clearing table

    for (int row = 0 ; row < nbRows ; ++row)
        addRow(row);
}

void MainWindow::updateTable(const QString &entryname) const
{
    if (!entrynames.contains(entryname))
        updateTable(); // resetting table
    else
    {
        int currentIndex = entrynames.indexOf(entryname);
        int row = 0; // <=> each entry

        entryTable->setRowCount(0); // clearing table

        while (currentIndex != -1)
        {
            addRow(currentIndex);
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

    if (indexOf(entryname, username) != -1)
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
               "     %1\n"
               "     %2"
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
               "     %1\n"
               "     %2"
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

void MainWindow::delEntry(const int row)
{
    // User inputs
    QString entryname = entryTable->item(row,0)->text();
    QString username = entryTable->item(row,1)->text();
    int indexToRemove = indexOf(entryname, username);

    // Asking user to confirm deletion
    int answer = QMessageBox::warning(
        this,
        this->windowTitle(),
        tr("L'entrée suivante va être supprimée :\n\n"
           "     %1\n"
           "     %2\n\n"
           "Voulez-vous vraiment la supprimer ?"
           ).arg(entryname, username),
        QMessageBox::Ok,
        QMessageBox::Cancel
        );

    if (answer == QMessageBox::Cancel) return;

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
               "     %1\n"
               "     %2"
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
               "     %1\n"
               "     %2"
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
}

void MainWindow::editEntry(const int row)
{
    int rowEdited = entryRowBeingEdited();

    if (rowEdited == -1) // no entry is being edited
    {
        int indexToEdit = indexOf(entryTable->item(row,0)->text(), entryTable->item(row,1)->text());

        // Setting entry name to editable and changing color
        entryTable->item(row,0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        entryTable->item(row,0)->setBackground(QColor(30,150,190));
        entryTable->item(row,0)->setForeground(QColor(255,255,255));
        // Setting user name to editable and changing color
        entryTable->item(row,1)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        entryTable->item(row,1)->setBackground(QColor(30,150,190));
        entryTable->item(row,1)->setForeground(QColor(255,255,255));
        // Setting edit icon to validate icon
        entryTable->item(row,3)->setIcon(QIcon(":/validate"));

        // Memorizing entry to edit
        entrynames[indexToEdit] = "entrynametoBeEdited";
        usernames[indexToEdit] = "usernametoBeEdited";

        // Disabling search bar, buttons and cell copy while editing
        disconnect(searchBar, SIGNAL(textChanged(QString)), this, SLOT(updateTable(QString)));
        disconnect(addButton, SIGNAL(pressed()), this, SLOT(showAddWindow()));
        disconnect(entryTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(copyCell(int,int)));
    }
    else if (rowEdited == row) // user validates modifications
    {
        int indexToEdit = indexOf("entrynametoBeEdited", "usernametoBeEdited");

        // Resetting entry name to read only and resetting color
        entryTable->item(row,0)->setFlags(Qt::ItemIsEnabled);
        entryTable->item(row,0)->setBackground(QColor(255,255,255));
        entryTable->item(row,0)->setForeground(QColor(0,0,0));
        // Resetting user name to read only and resetting color
        entryTable->item(row,1)->setFlags(Qt::ItemIsEnabled);
        entryTable->item(row,1)->setBackground(QColor(255,255,255));
        entryTable->item(row,1)->setForeground(QColor(0,0,0));
        // Resetting validate icon to edit icon
        entryTable->item(row,3)->setIcon(QIcon(":/edit"));

        // Updating new entry and user names
        entrynames[indexToEdit] = entryTable->item(row,0)->text();
        usernames[indexToEdit] = entryTable->item(row,1)->text();

        // Writing entries in file
        if (pwm::writeEntries(loginWindow->getNewPassword(), entrynames, usernames, passwords, dates) != 0)
        {
            // Error in file writing
            QMessageBox::critical(
                this,
                this->windowTitle(),
                tr("Une erreur est survenue lors de l'enregistrement des entrées.\n"
                   "L'entrée suivante n'a pas été modifiée:\n"
                   "     %1\n"
                   "     %2\n"
                   "L'application doit être redémarrée pour recharger les entrées correctes."
                   ).arg(entrynames[indexToEdit], usernames[indexToEdit])
                );
            close();
        }

        // Re-enabling search bar, buttons and cell copy
        connect(searchBar, SIGNAL(textChanged(QString)), this, SLOT(updateTable(QString)));
        connect(addButton, SIGNAL(pressed()), this, SLOT(showAddWindow()));
        connect(entryTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(copyCell(int,int)));
    }
    else // another entry is being modified
    {
        QMessageBox::warning(
            this,
            this->windowTitle(),
            tr("L'entrée\n"
               "     %1\n"
               "     %2\n"
               "est en cours de modification.\n"
               "Veuillez valider les modifications pour éditer une autre entrée."
               ).arg(entryTable->item(rowEdited, 0)->text(), entryTable->item(rowEdited, 1)->text())
            );
    }
}

void MainWindow::addRow(const int entryIndex) const
{
    int row = entryTable->rowCount();
    int nCols = entryTable->columnCount();

    entryTable->insertRow(row);

    if (entryIndex < 0)
    {
        for (int col = 0 ; col < nCols ; ++col)
            entryTable->setItem(row,col,new QTableWidgetItem());
    }
    else
    {
        entryTable->setItem(row,0,new QTableWidgetItem(iconFrom(dates[entryIndex]),entrynames[entryIndex]));
        entryTable->setItem(row,1,new QTableWidgetItem(usernames[entryIndex]));
        entryTable->setItem(row,2,new QTableWidgetItem(QString("***************")));
        entryTable->setItem(row,3,new QTableWidgetItem(QIcon(":/edit"), QString()));
        entryTable->setItem(row,4,new QTableWidgetItem(QIcon(":/regenerate"), QString()));
        entryTable->setItem(row,5,new QTableWidgetItem(QIcon(":/delete"), QString()));
        entryTable->setRowHeight(row,20);
    }

    // Disabling modification for each cell
    for (int col = 0 ; col < nCols ; ++col)
        entryTable->item(row,col)->setFlags(Qt::ItemIsEnabled);
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

const int MainWindow::indexOf(const QString &entryname, const QString &username) const
{
    int currentIndex = entrynames.indexOf(entryname);

    while (currentIndex != -1)
    {
        if (usernames[currentIndex] == username) return currentIndex;
        currentIndex = entrynames.indexOf(entryname, currentIndex+1);
    }

    return -1;
}

const int MainWindow::entryRowBeingEdited() const
{
    for (int row = 0 ; row < entryTable->rowCount() ; row++)
    {
        // Entry and user names can be edited at the same time,
        // so checking entry name is enough.
        if (entryTable->item(row, 0)->flags() == (Qt::ItemIsEnabled | Qt::ItemIsEditable))
            return row;
    }

    return -1;
}
