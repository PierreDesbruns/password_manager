#ifndef PWMSECURITY_H
#define PWMSECURITY_H

#include <QString>
#include <QStringList>
#include <QDebug>

#include <sodium.h>
#include <cstdio>

// ENTRY_MAXLEN =
// ENTRYNAME_MAXLEN
// + USERNAME_MAXLEN
// + PASSWORD_MAXLEN
// + 10 date characters
// + 3 separating characters ('\t')
// + end character '\0'
#define ENTRY_MAXLEN 128
#define ENTRYNAME_MAXLEN 22
#define USERNAME_MAXLEN 32
#define PASSWORD_MAXLEN 60

#define MASTER_MINLEN crypto_pwhash_PASSWD_MIN
#define MASTER_MAXLEN crypto_pwhash_PASSWD_MAX


namespace pwm {

/**
 * @brief Update master password hash file.
 *
 * @param password: Password to hash.
 *
 * @return 0 if successfully updated password hash file; -1 otherwise.
 *
 * File structure:
 * hash (char)
 *
 * @attention Access to current password could be lost.
 */
int updateMasterHash(const QString &password);

/**
 * @brief Update crypto parameters file with random salt.
 *
 * @return 0 if successfully updated crypto parameters file; -1 otherwise.
 *
 * File structure:
 * salt      (unsigned char)
 * opslimit  (unsigned long long)
 * memlimit  (size_t)
 * alg       (int)
 *
 * @attention Access to entries file could be lost if called before decryption.
 */
int updateCryptoParams();

/**
 * @brief Verify if given password corresponds to master password.
 *
 * @param master: Master password to verify.
 * @return True if given password is correct; False otherwise.
 */
const bool masterIsCorrect(const QString &master);

/**
 * @brief Generate an unpredictible password from given parameters.
 *
 * @param passwordLength: Must be greater than 0.
 * @param hasLowCase: 1 to insert low case letters; 0 to not.
 * @param hasUpCase: 1 to insert high case letters; 0 to not.
 * @param hasNumbers: 1 to insert numbers; 0 to not.
 * @param hasSpecials: 1 to insert special characters; 0 to not.
 * @return Unpredictible password.
 */
QString generatePassword(const int passwordLength, const bool hasLowCase, const bool hasUpCase, const bool hasNumbers, const bool hasSpecials);

/**
 * @brief Generate an encryption/decryption key from password and crypto parameters.
 *
 * @param secretKey: Array where key is going to be stored.
 * @param master: Master password used to generate secret key.
 * @return 0 if successfully generated key; -1 otherwise.
 *
 * Crypto parameters are extracted from crypto.params file.
 * File structure:
 * salt\n       (unsigned char, do not forget to cast!)
 * opslimit\n   (unsigned long long)
 * memlimit\n   (int)
 * alg          (int)
 */
int generateSecretKey(unsigned char secretKey[], const QString &master);

/**
 * @brief Read entries encrypted data from entries file.
 *
 * @param master: Master password used to generate encryption/decryption key.
 * @return List of each line of password file.
 *
 * 1. Secret key is generated from password and parameters.
 * 2. Each line of password file is decrypted and stored as string.
 * 3. All strings are returned as a list.
 *
 * File structure (encrypted):
 * entryname1\tusername1\tpassword1\tdate1\0
 * entryname2\tusername2\tpassword2\tdate2\0
 * entryname3\tusername3\tpassword3\tdate3\0
 * ...
 */
QStringList readEntries(const QString &master);

/**
 * @brief Write entries encrypted data to entries file.
 *
 * @return 0 if successfully read entries file and entry fields have same number of elements; -1 otherwise.
 *
 * 1. Secret key is generated from password and parameters.
 * 2. Each entry is concanetated as a line.
 * 3. Each line is encrypted.
 * 4. Each encrypted line is writton to entries file.
 *
 * File structure (encrypted):
 * entryname1\tusername1\tpassword1\tdate1\0
 * entryname2\tusername2\tpassword2\tdate2\0
 * entryname3\tusername3\tpassword3\tdate3\0
 * ...
 */
int writeEntries(const QString &master, const QStringList &entrynames, const QStringList &usernames, const QStringList &passwords, const QStringList &dates);

} // namespace pwm

#endif // PWMSECURITY_H
