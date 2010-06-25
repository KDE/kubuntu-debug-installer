/***************************************************************************
 *   Copyright © 2010 Harald Sitter <apachelogger@ubuntu.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef DBGINSTALLER_H
#define DBGINSTALLER_H

#include <KProgressDialog>

#define ERR_RANDOM_ERR 1
#define ERR_NO_SYMBOLS 2
#define ERR_CANCEL 3

class DbgInstaller : public KProgressDialog {
    Q_OBJECT
public:
    explicit DbgInstaller(KProgressDialog *parent = 0, const QString &caption = "",
                          QStringList *args = 0);
    ~DbgInstaller();
    void run();

private:
    void install();
    void askMissing();
    void askInstall();
    void checkListEmpty() const;
    void incrementProgress();

    QStringList *m_args;
    QStringList *m_dbgpkgs;
    QStringList *m_nodbgpkgs;

private slots:
    void foundDbgPkg(QString);
    void foundNoDbgPkg(QString);

};

#endif // DBGINSTALLER_H
