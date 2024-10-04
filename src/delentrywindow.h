#ifndef DELENTRYWINDOW_H
#define DELENTRYWINDOW_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCompleter>
#include <QMessageBox>


class DelEntryWindow : public QDialog
{
    Q_OBJECT

public:
    DelEntryWindow();
    QString getEntryname() const { return entrynameBox->currentText(); }
    QString getUsername() const { return usernameBox->currentText(); }
    /**
     * @brief Update string lists and entry names from given entry names and usernames.
     *
     * @param entrynames: list of entry names.
     * @param usernames: list of user names.
     *
     * @note Muse be called before show().
     */
    void updateEntries(const QStringList &entrynames, const QStringList &usernames);

private slots:
    /**
     * @brief Call accept() after verifications.
     *
     * Called when confirm button is pressed.
     * Verifications:
     * 1. Entry name must be correct.
     */
    void verifications();
    /**
     * @brief Update usernames combobox depending on entry name.
     *
     * @param entryBoxText: current text of entry name combobox.
     *
     * Called when entry name text is changed.
     * Used to ensure that user enters an existing username associated with entry name.
     * If entry name is not valid, username box is empty.
     */
    void updateUsernameBox(const QString &entryBoxText) const;

private:
    const QSize windowSize = QSize(300,100);

    QWidget *mainContent;

    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QHBoxLayout *buttonsLayout;

    QLabel *entrynameLabel;
    QLabel *usernameLabel;

    QComboBox *entrynameBox;
    QComboBox *usernameBox;

    QLineEdit *entrynameLine;
    QCompleter *entrynameCompleter;
    QStringListModel *entrynameModel;

    QPushButton *confirmButton;
    QPushButton *cancelButton;

    QStringList fullEntrynameList;
    QStringList fullUsernameList;
    QStringList displayedEntrynameList; // sorted entries without duplicates
};

#endif // DELENTRYWINDOW_H
