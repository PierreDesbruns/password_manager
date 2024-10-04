#ifndef REGENTRYWINDOW_H
#define REGENTRYWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QPushButton>


class RegEntryWindow : public QDialog
{
    Q_OBJECT

public:
    RegEntryWindow();
    QString getEntryname() const { return entrynameBox->currentText(); }
    QString getUsername() const { return usernameBox->currentText(); }
    int getPasswordLength() const { return passwordLengthBox->value(); }
    bool hasLowCase() const { return enableLowCaseBox->isChecked(); }
    bool hasUpCase() const { return enableUpCaseBox->isChecked(); }
    bool hasNumbers() const { return enableNumbersBox->isChecked(); }
    bool hasSpecials() const { return enableSpecialsBox->isChecked(); }
    void setPasswordMaxLength(const int length) { passwordLengthBox->setMaximum(length); }
    /**
     * @brief Update string list attributes with given entries and usernames; clear input fields.
     *
     * @param entries: list of entry names.
     * @param usernames: list of user names.
     *
     * @note Must be called before show().
     */
    void updateEntries(const QStringList &entries, const QStringList &usernames);

private slots:
    /**
     * @brief Call accept() after verifications.
     *
     * Called when confirm button is pressed.
     * Verifications:
     * 1. Entry name must be correct.
     * 2. Password length must be non zero.
     * 3. At least one box must be checked.
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
    const QSize windowSize = QSize(300,150);

    QWidget *mainContent;

    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QHBoxLayout *spinBoxesLayout;
    QHBoxLayout *buttonsLayout;

    QLabel *entrynameLabel;
    QLabel *usernameLabel;
    QLabel *passwordLengthLabel;

    QComboBox *entrynameBox;
    QComboBox *usernameBox;

    QLineEdit *entrynameLine;
    QCompleter *entrynameCompleter;
    QStringListModel *entrynameModel;

    QSpinBox *passwordLengthBox;

    QCheckBox *enableLowCaseBox;
    QCheckBox *enableUpCaseBox;
    QCheckBox *enableNumbersBox;
    QCheckBox *enableSpecialsBox;

    QPushButton *confirmButton;
    QPushButton *cancelButton;

    QStringList fullEntrynameList;
    QStringList fullUsernameList;
    QStringList displayedEntrynameList; // sorted entries without duplicates
};

#endif // REGENTRYWINDOW_H
