# Password manager

This simple application allows the user to safely store strong passwords on his/her local machine. It is developped with the [open-source version of Qt](https://www.qt.io/download-open-source) and it uses the [Sodium library](https://doc.libsodium.org/).

Author: Pierre Desbruns

## Install
Download the [password_manager-vX.X.X.zip file](https://github.com/PierreDesbruns/password_manager/releases/tag/v1.1.0) of the desired version (v1.0.0 is the latest) and extract all files.
Then, see [Usage](#usage).

## Build from source
This project has been built on Windows from [Qt Creator IDE](https://www.qt.io/product/development-tools) using [MinGW](https://sourceforge.net/projects/mingw/) and [CMake](https://cmake.org/). With these tools, here are the different steps to build the application:
1. Clone this repository or download it as .zip file.
2. In Qt Creator open `src/CMakeLists.txt` file.
3. Install a [Sodium pre-built library](https://download.libsodium.org/libsodium/releases/) and change path to it in `CMakeLists.txt`:
	```cmake
	# Including libsodium
	target_include_directories(password_manager PRIVATE "/path/to/libsodium-win64/include")

	target_link_libraries(password_manager
		PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
		PRIVATE "/path/to/libsodium-win64/lib/libsodium.a"
	)
	```
3. Build the project with release compiler. A directory called `password_manager-v1.0.0-Release` is created by Qt in parent directory of `src`.
4. Remove all files but `password_manager.exe` in release directory and execute `windeployqt.exe` (located inside Qt directory) in a command prompt with `/path/to/password_manager.exe` as argument.
5. Move the three files `default/crypto.params`, `default/entries.cipher`, and `default/master.hash` to release directory.
6. See [Usage](#usage).

> [!NOTE]
> Release folder can be moved anywhere. A shortcut to executable file can also be set at any convenient location.

## Usage
Run `password_manager.exe`. An authentication window pops up and asks for master password (default is *1234*).

A correct master password gives access to the main window containing all entries in a table: first column for entry names; second for usernames; third for passwords.

Double-click on username or password to copy it to clipboard. Entry names can be searched in top search bar. A colored circle next to entry name gives indication on password generation date: green for \< 3 months; orange for \< 6 months; red for \> 6 months.

Application can be closed by simply hitting close button. Entries are saved while closing.

## Features
- Generate an unpredictable password from any type of character (lower and upper cases, numbers and special characters can be chosen).
- Add / delete entries containing entry name, username, unpredictable password and date of last password update.
- Re-generate a given entry's password if it is outdated.
- Search for entries via entry name.
- Copy username or password to clipboard via a simple double click on it.
- Secure and simplify access to application via a single master password that can be changed at any moment.
- Safely store entries via secret-key cryptography.
- Safely store master via hashing.

## Versions
- 1.0.0-beta.4
	- Fixed wrong autocompletion in search bar.
- 1.0.0-beta.3
	- Updated log messages format.
	- Moved entries file writing from application closing to any entries updates (add, delete, re-generate) to fix empty file after login window cancelling.
- 1.0.0-beta.2
	- Fixed Tab key issue in login window.
	- Added message in login window when new typed master is same as previous.
	- Fixed wrong encryption when master had been changed.
- 1.0.0-beta.1
	- Initial version with all intended features.
	
## License
This project is licensed under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 3 (only). More information can be found in [LICENSE.txt](LICENSE.txt).

This project is based on Qt open-source for graphical interface and Sodium for cryptography.

### Qt
Qt modules used in this application are open-source and licensed under [GNU LESSER GENERAL PUBLIC LICENSE Version 3 (only)](https://www.gnu.org/licenses/lgpl-3.0.en.html).

Documentation for all Qt C++ classes is available [here](https://doc.qt.io/qt-6/classes.html). Clicking on a class will lead to its description and its implemented members. All members can be accessed by clicking *List of all members, including inherited members*.

Qt sources can be installed in [Qt Online Installer](https://doc.qt.io/qt-6/get-and-install-qt.html). Source packages are also available [through Git](https://doc.qt.io/qt-6/getting-sources-from-git.html), as archives in the [Qt Account](https://login.qt.io/login) (commercial users), and on [download.qt.io](https://download.qt.io/) (open-source users).

### Sodium
Sodium library is open-source and licensed under [ISC license](https://en.wikipedia.org/wiki/ISC_license).

Documentation is available [here](https://doc.libsodium.org/).

Sources are available either as [pre-compiled binaries](https://download.libsodium.org/libsodium/releases/) or via [GitHub repository](https://github.com/jedisct1/libsodium).