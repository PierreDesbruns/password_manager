#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QSize>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "pwmsecurity.h"


class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    LoginWindow();
    QString getPassword() const { return passwordLine->text(); }

private slots:
    /**
     * @brief Call accept() after verifications.
     *
     * Called when confirm button is pressed.
     * Verifications:
     * 1. Password must be correct.
     * 2. (if change password is selected) New password and confirmation must match.
     * 3. (if change password is selected) New password must be long enough.
     */
    void verifications();
    /**
     * @brief Set change password interface to visible.
     * Update [passwordChanged] to true.
     */
    void changePassword();

private:
    const QSize windowSmallSize = QSize(300,90);
    const QSize windowLargeSize = QSize(300,140);

    QWidget *mainContent;

    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QHBoxLayout *buttonsLayout;

    QLabel *passwordLabel;
    QLabel *newPasswordLabel;
    QLabel *confirmNewPasswordLabel;

    QLineEdit *passwordLine;
    QLineEdit *newPasswordLine;
    QLineEdit *confirmNewPasswordLine;

    QPushButton *confirmButton;
    QPushButton *cancelButton;
    QPushButton *changePwdButton;

    bool passwordChanged = false; // flag to indicate if user asks to change master password
};

#endif // LOGINWINDOW_H
