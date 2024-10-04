#include "pwmsecurity.h"

namespace pwm {

int updateMasterHash(const QString &password)
{
    FILE * masterHashFile = fopen("master.hash", "wb");
    int returnValue = -1;

    // Generating hash
    char hash[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(
            hash,
            password.toStdString().c_str(),
            password.size(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
    {
        // Out of memory
        qCritical() << "Failed to generate hash for given password. Aborted before master hash file update.";
        goto ret;
    }

    // Writing hash in file
    if (fwrite(hash, sizeof hash[0], sizeof hash, masterHashFile) == 0)
    {
        // Error in file opening
        qCritical() << "Failed to write password hash in file. Aborted master hash file update.";
        goto ret;
    }

    returnValue = 0;
ret:
    fclose(masterHashFile);
    return returnValue;

}

int updateCryptoParams()
{
    FILE * cryptoFile = fopen("crypto.params", "wb");
    int returnValue = -1;

    // Generating unpredictible salt
    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, crypto_pwhash_SALTBYTES);

    // Defining parameters
    unsigned long long opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
    size_t memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
    int alg = crypto_pwhash_ALG_DEFAULT;

    // Writing parameters in file
    if (fwrite(salt, sizeof salt[0], sizeof salt, cryptoFile) == 0)
    {
        qCritical() << "Failed to write salt in file. Aborted crypto parameters update.";
        goto ret;
    }
    if (fwrite(&opslimit, sizeof opslimit, 1, cryptoFile) == 0)
    {
        qCritical() << "Failed to write opslimit in file. Aborted crypto parameters update.";
        goto ret;
    }
    if (fwrite(&memlimit, sizeof memlimit, 1, cryptoFile) == 0)
    {
        qCritical() << "Failed to write memlimit in file. Aborted crypto parameters update.";
        goto ret;
    }
    if (fwrite(&alg, sizeof alg, 1, cryptoFile) == 0)
    {
        qCritical() << "Failed to write algorithm in file. Aborted crypto parameters update.";
        goto ret;
    }

    returnValue = 0;
ret:
    fclose(cryptoFile);
    return returnValue;
}

QString generatePassword(const int passwordLength, const bool hasLowCase, const bool hasUpCase, const bool hasNumbers, const bool hasSpecials)
{
    QString characters = "";
    QString password = "";
    int nbCharacters = 0;

    if (hasLowCase) characters.append("abcdefghijklmnopqrstuvwxyz");
    if (hasUpCase) characters.append("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (hasNumbers) characters.append("0123456789");
    if (hasSpecials) characters.append("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");

    if (characters.isEmpty())
    {
        // All booleans are false
        qCritical() << "Cannot generate password without any charater. Returned empty password.";
        return "";
    }

    nbCharacters = characters.size();

    // Generating unpredictible password from selected characters
    while (password.size() < passwordLength)
        password.append(characters[randombytes_uniform(nbCharacters-1)]);

    return password;
}

int generateSecretKey(unsigned char secretKey[crypto_secretstream_xchacha20poly1305_KEYBYTES], const QString &master)
{
    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned long long opslimit;
    size_t memlimit;
    int alg;

    FILE * cryptoFile;
    cryptoFile = fopen("crypto.params", "rb");
    int returnValue = -1;

    // Reading crypto parameters file
    if (fread(salt, sizeof salt[0], sizeof salt, cryptoFile) == 0)
    {
        qCritical() << "Failed to read salt from file. Aborted before key generation.";
        goto ret;
    }
    if (fread(&opslimit, sizeof opslimit, 1, cryptoFile) == 0)
    {
        qCritical() << "Failed to read opslimit from file. Aborted before key generation.";
        goto ret;
    }
    if (fread(&memlimit, sizeof memlimit, 1, cryptoFile) == 0)
    {
        qCritical() << "Failed to read memlimie from file. Aborted before key generation.";
        goto ret;
    }
    if (fread(&alg, sizeof alg, 1, cryptoFile) == 0)
    {
        qCritical() << "Failed to read algorithm from file. Aborted before key generation.";
        goto ret;
    }

    // Generating secret key from parameters and password
    if (crypto_pwhash(
            secretKey,
            crypto_secretstream_xchacha20poly1305_KEYBYTES,
            master.toStdString().c_str(),
            master.size(),
            salt,
            opslimit,
            memlimit,
            alg) != 0)
    {
        qCritical() << "Failed to generate secret key from given password and parameters. Aborted key generation.";
        goto ret;
    }

    returnValue = 0;
ret:
    fclose(cryptoFile);
    return returnValue;
}

QStringList readEntries(const QString &master)
{
    QStringList entries;

    FILE * entriesFile = fopen("entries.cipher", "rb");

    unsigned char entryPlain[ENTRY_MAXLEN];
    unsigned char entryCipher[ENTRY_MAXLEN + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state state;
    unsigned char tag = 0;

    // Generating decryption key
    unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    if (generateSecretKey(key, master) != 0)
    {
        qCritical() << "Failed to generate secret key. Aborted before entries file reading.";
        goto ret;
    }

    // Header pull
    if (fread(header, sizeof header[0], sizeof header, entriesFile) == 0)
    {
        qCritical() << "Failed to read header. Aborted entries file reading.";
        goto ret;
    }
    if (crypto_secretstream_xchacha20poly1305_init_pull(&state, header, key) != 0)
    {
        // Incomplete header
        qCritical() << "Failed to recognize header. Aborted entries file reading.";
        goto ret;
    }

    // Entries pull
    while (tag != crypto_secretstream_xchacha20poly1305_TAG_FINAL)
    {
        // Reading entry
        if (fread(entryCipher, 1, sizeof entryCipher, entriesFile) == 0)
        {
            // End of file
            goto ret;
        }

        // Decrypting entry
        if (crypto_secretstream_xchacha20poly1305_pull(&state, entryPlain, NULL, &tag, entryCipher, sizeof entryCipher, NULL, 0) != 0)
        {
            // Corrupted chunk
            qCritical() << "Failed to decrypt entry. Aborted entries file reading.";
            goto ret;
        }

        // Converting UChar entry to QString and appending to entries list
        QString entryAsString;
        int c = 0;
        while(entryPlain[c] != '\0')
        {
            entryAsString.append(QChar(entryPlain[c]));
            c++;
        }
        entries << entryAsString;
    }

ret:
    fclose(entriesFile);
    return entries;
}

int writeEntries(const QString &master, const QStringList &entrynames, const QStringList &usernames, const QStringList &passwords, const QStringList &dates)
{
    int returnValue = -1;

    const int nbEntries = entrynames.size();

    // Checking size equality
    if (!(nbEntries == usernames.size() && nbEntries == passwords.size() && nbEntries == dates.size()))
    {
        qCritical() << "Number of elements of each entry fields do not match. Aborted before entries file writing";
        return returnValue;
    }

    FILE * entriesFile = fopen("entries.cipher", "wb");
    unsigned char entryPlain[ENTRY_MAXLEN];
    unsigned char entryCipher[ENTRY_MAXLEN + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state state;
    unsigned char tag;

    // Generating decryption key
    unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    if (generateSecretKey(key, master) != 0)
    {
        qCritical() << "Failed to generate secret key. Aborted before entries file writing.";
        goto ret;
    }

    // Header push
    crypto_secretstream_xchacha20poly1305_init_push(&state, header, key);
    if (fwrite(header, sizeof header[0], sizeof header, entriesFile) == 0)
    {
        qCritical() << "Failed to write header. Aborted entries file writing.";
        goto ret;
    }

    // Entries push
    for (int entry = 0 ; entry < nbEntries ; ++entry)
    {
        // Concatenating entry fields as an entry
        QString entryAsString;
        int entryLength; // without '\0'
        entryAsString.append(entrynames[entry]);
        entryAsString.append('\t');
        entryAsString.append(usernames[entry]);
        entryAsString.append("\t");
        entryAsString.append(passwords[entry]);
        entryAsString.append("\t");
        entryAsString.append(dates[entry]);
        entryLength = entryAsString.size();

        if (entryLength + 1 > ENTRY_MAXLEN) // +1 to consider '\0'
        {
            // Should never happen if max lengths are respected during entry creation
            qWarning() << "Entry is too long. Skipping entry.";
        }

        // Writing QString entry into UChar
        for (int c = 0 ; c < entryLength ; c++)
            entryPlain[c] = (unsigned char)entryAsString[c].toLatin1();
        entryPlain[entryLength] = '\0'; // used to ignore characters beyond [entryLength] when reading

        // Checking for last entry to update tag
        tag = (entry == (nbEntries - 1)) ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;

        // Encrypting entry
        crypto_secretstream_xchacha20poly1305_push(&state, entryCipher, NULL, entryPlain, sizeof entryPlain, NULL, 0, tag);

        // Writing entry to entries file
        if (fwrite(entryCipher, 1, sizeof entryCipher, entriesFile) == 0)
        {
            qCritical() << "Failed to write entry. Aborted entries file writing.";
            goto ret;
        }
    }

    returnValue = 0;
ret:
    fclose(entriesFile);
    return returnValue;
}

const bool masterIsCorrect(const QString &master)
{
    char hash[crypto_pwhash_STRBYTES];
    FILE * masterHashFile = fopen("master.hash", "rb");

    // Reading hash from file
    if (fread(hash, sizeof hash[0], sizeof hash, masterHashFile) == 0)
    {
        // Error in file reading
        qCritical() << "Failed to read master hash.";
        return false;
    }

    // Verifying password with hash
    if (crypto_pwhash_str_verify(
            hash,
            master.toStdString().c_str(),
            master.size()) == 0)
        return true;

    return false;
}

} // namespace pwm
