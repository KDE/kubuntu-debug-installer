/*
  Copyright © 2010 Harald Sitter <apachelogger@ubuntu.com>
  Copyright © 2010 Felix Geyer <debfx@fobos.de>

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

#include "dbglookupthread.h"

#include <QtCore/QStringList>

#include <libqapt/backend.h>

DbgLookupThread::DbgLookupThread(QObject *parent, QStringList *files) :
        QThread(parent),
        m_backend(0),
        m_files(files)
{
}

QApt::Package *DbgLookupThread::getDebPkg(QApt::Package *package)
{
    QString srcPkg;
    // TODO: map packages names for Qt
    if (package->sourcePackage() == "qt4-x11") {
        srcPkg = "libqt4";
    } else {
        srcPkg = package->sourcePackage();
    }

    QApt::Package *dbgPkg;

    dbgPkg = m_backend->package(srcPkg + "-dbgsym");
    if (dbgPkg) {
        return dbgPkg;
    }

    dbgPkg = m_backend->package(srcPkg + "-dbg");
    if (dbgPkg) {
        return dbgPkg;
    }

    return 0;
}

void DbgLookupThread::run()
{
    m_backend = new QApt::Backend;
    m_backend->init();

    foreach (const QString &file, *m_files) {
        QApt::Package *package = m_backend->packageForFile(file);

        QApt::Package *dbgPkg = getDebPkg(package);
        if (!dbgPkg) {
            emit foundNoDbgPkg(file);
        } else if (dbgPkg->isInstalled()) {
            emit alreadyInstalled();
        } else {
            emit foundDbgPkg(dbgPkg->name());
        }
    }

    m_backend->deleteLater();
    exec();
}
