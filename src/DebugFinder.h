/*
  Copyright © 2010 Harald Sitter <apachelogger@ubuntu.com>
  Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEBUGFINDER_H
#define DEBUGFINDER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

namespace QApt {
    class Backend;
    class Package;
}

class DebugFinder : public QObject
{
    Q_OBJECT
public:
    explicit DebugFinder(QObject *parent = 0);

public slots:
    void find(const QString &file);
    void find(const QStringList &files);
    void stop();

signals:
    void foundDbgPkg(const QString &dbgpkg);
    void foundNoDbgPkg(const QString &file);
    void alreadyInstalled();

private:
    QApt::Package *getDbgPkg(QApt::Package *package);
    QApt::Backend *m_backend;

    bool m_stop;
};

#endif // DEBUGFINDER_H
