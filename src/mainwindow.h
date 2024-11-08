// Copyright (C) 2024 Pierre Desbruns
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QIcon>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QSize>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QString>
#include <QStringList>
#include <QClipboard>
#include <QDate>
#include <QMessageBox>
#include <QDebug>

#include "pwmsecurity.h"
#include "loginwindow.h"
#include "addentrywindow.h"
#include "delentrywindow.h"
#include "regentrywindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /**
     * @brief Copy username or password to clipboard.
     *
     * @param row: Selected row of entry table.
     * @param col: Selected column of entry table.
     *
     * Called when a cell is double clicked.
     *
     * @note Does nothing for col == 1.
     */
    void copyCell(const int row, const int col) const;
    /**
     * @brief Fills the table with all entries.
     * Called when the table needs to be reset.
     */
    void updateTable() const;
    /**
     * @brief Fills the table only with entries corresponding to given entry name.
     *
     * @param entryname: name of entries to display.
     *
     * Called when an entry name is selected by auto-completion.
     */
    void updateTable(const QString &entryname) const;

    /**
     * @brief Show window responsible for adding an entry.
     */
    void showAddWindow() const;
    /**
     * @brief Show window responsible for deleting an entry.
     */
    void showDelWindow() const;
    /**
     * @brief Show window responsible for re-generating an entry password.
     */
    void showRegWindow() const;

    /**
     * @brief Load entries from entries file and store each field in corresponding string list.
     * Entries file is re-encrypted in case master password has changed.
     * Called when [loginwindow] is accepted.
     */
    void loadEntries();
    /**
     * Add an entry to string lists, write entries in file, and update table.
     * Called when [addWindow] is accepted.
     * @note Check if entry already exists.
     */
    void addEntry();
    /**
     * Remove an entry from string lists attributes, write entries in file, and update table.
     * Called when [delWindow] is accepted.
     * @note Given entry should exist due to [delwindow] verifications.
     */
    void delEntry();
    /**
     * Re-generate a password for given entry and username, and write entries in file,.
     * Called when [regWindow] is accepted.
     * @note Given entry should exist due to [regwindow] verifications.
     */
    void regEntry();

private:
    QClipboard *clipboard;

    const QSize windowSize = QSize(400,500);

    QWidget *mainContent;

    QGridLayout *mainLayout;

    QPushButton *addButton;
    QPushButton *delButton;
    QPushButton *regButton;

    QLineEdit *searchBar;
    QCompleter *searchCompleter;
    QStringListModel *searchModel; // must be updated whenever [entrynames] is updated

    QTableWidget *entryTable;

    LoginWindow *loginWindow; // window responsible for authentification. Shows only on start.
    AddEntryWindow *addWindow; // window responsible for adding entries
    DelEntryWindow *delWindow; // window responsible for deleting entries
    RegEntryWindow *regWindow; // window responsible for re-generate entries password

    QStringList entrynames;
    QStringList usernames;
    QStringList passwords;
    QStringList dates;

    /**
     * @brief Return an icon corresponding to an entry's date of creation.
     *
     * @param date: date of creation of the entry. Expected format: "yyyy.MM.dd".
     * @return Green icon if date is okay; Orange icon if date is about to be passed; Red icon otherwise.
     *
     * Red icon: date is passed (more than 6 months);
     * Orange icon: date is about to be passed (bwt. 3 and 6 months);
     * Green icon: date is okay (less than 3 months).
     */
    QIcon iconFrom(const QString &date) const;

    /**
     * @brief Detect if given entry and username already exist.
     *
     * @param entryname: name of entry.
     * @param username: usernam associated to entry.
     * @return true if an entry with same username already exists; false otherwise.
     *
     * Used to avoid several entries with same name and username.
     */
    const bool exists(const QString &entryname, const QString &username) const;
};
#endif // MAINWINDOW_H
