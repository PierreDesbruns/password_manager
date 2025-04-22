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
     * @param row: Selected row of entry table.
     * @param col: Selected column of entry table.
     *
     * Called when a cell is double clicked.
     * Works only for col == 1 and col == 2.
     */
    void copyCell(const int row, const int col) const;
    /**
     * @brief Execute the action corresponding to the cell clicked.
     * @param row: Row index of the clicked cell.
     * @param col: Column index of the clicked cell.
     *
     * Works only for col == 3.
     */
    void buttonFromCell(const int row, const int col);
    /**
     * @brief Fills the table with all entries.
     * Called when the table needs to be reset.
     */
    void updateTable() const;
    /**
     * @brief Fills the table only with entries corresponding to given entry name.
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
     * @brief Add an entry to string lists, write entries in file, and update table.
     * Called when [addWindow] is accepted.
     * @note Check if entry already exists.
     */
    void addEntry();
    /**
     * @brief Remove an entry from string lists attributes, write entries in file, and update table.
     * Called when [delWindow] is accepted.
     * @note Given entry should exist due to [delwindow] verifications.
     */
    void delEntry();
    /**
     * @brief Re-generate a password for given entry and username, and write entries in file.
     * Called when [regWindow] is accepted.
     * @note Given entry should exist due to [regwindow] verifications.
     */
    void regEntry();
    /**
     * @brief Make entry and user names editable.
     * @param row: Row index of the entry being edited.
     * Called when the edit cell of an entry is clicked.
     * Disconnects searchbar text changed signal from table update to avoid
     * searching while editing an entry.
     * @note Close the window if a problem occurs while writing in file.
     */
    void editEntry(const int row);

signals:
    /**
     * @brief Signal emitted when the edit button is clicked
     * @param row: Row index of the entry being edited.
     * @see buttonFromCell()
     */
    void editEntryClicked(const int row);

private:
    QClipboard *clipboard;

    const QSize windowSize = QSize(420,500);

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
     * @param date: date of creation of the entry. Expected format: "yyyy.MM.dd".
     * @return Green icon if date is okay; Orange icon if date is about to be passed; Red icon otherwise.
     *
     * Red icon: date is passed (more than 6 months);
     * Orange icon: date is about to be passed (bwt. 3 and 6 months);
     * Green icon: date is okay (less than 3 months).
     */
    QIcon iconFrom(const QString &date) const;

    /**
     * @brief Give the string list index corresponding to entry and user names.
     * @param entryname: Name of the entry.
     * @param username: Name of the user.
     * @return Index of corresponding entry and user names; -1 if entry does not exist.
     */
    const int indexOf(const QString& entryname, const QString &username) const;

    /**
     * @brief Returns the row index of the entry being edited by user.
     * @return Row index if a row is being edited; -1 other wise.
     */
    const int entryRowBeingEdited() const;
};
#endif // MAINWINDOW_H
