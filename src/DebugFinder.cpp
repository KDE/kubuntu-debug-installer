/*
  Copyright © 2010 Harald Sitter <apachelogger@ubuntu.com>
  Copyright © 2010 Felix Geyer <debfx@fobos.de>
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

#include "DebugFinder.h"

#include <QtCore/QStringList>
#include <QtCore/QThread>

#include <QApt/Backend>

DebugFinder::DebugFinder(QObject *parent) :
    QObject(parent),
    m_backend(new QApt::Backend(this)),
    m_stop(false)
{
    m_backend->init();
}

struct PackageMapEntryStruct
{
    QString src;
    QString tgt;
};

QApt::Package *DebugFinder::getDbgPkg(QApt::Package *package)
{
    if (!package) {
        return nullptr;
    }

    QString srcPkg = package->sourcePackage();

    static const struct { QString src; QString tgt; } sourceMap[] = {
        { QStringLiteral("qt4-x11"), QStringLiteral("libqt4") },
    };
    for (unsigned i = 0; i < sizeof sourceMap / sizeof *sourceMap; ++i) {
        auto m = sourceMap[i];
        if (m.src == package->sourcePackage()) {
            srcPkg = m.tgt;
            break;
        }
    }

    QApt::Package *dbgPkg = 0;

    dbgPkg = m_backend->package(srcPkg + "-dbgsym");
    if (dbgPkg) {
        return dbgPkg;
    }

    dbgPkg = m_backend->package(srcPkg + "-dbg");
    if (dbgPkg) {
        return dbgPkg;
    }

    dbgPkg = m_backend->package(package->name() + "-dbg");
    if (dbgPkg) {
        return dbgPkg;
    }

    // Qt 5 is spread over multiple sources, so static mapping would be
    // rather error prone. Instead attempt to modify suitable source names
    // according to convention and see if that yields a dbg package.
    if (package->sourcePackage().endsWith(QStringLiteral("-opensource-src")) &&
            package->upstreamVersion().startsWith(QStringLiteral("5."))) {
        // TODO: this possibly should simply repeat the entire function?
        QString name = package->sourcePackage();
        name.replace(QStringLiteral("-opensource-src"), QStringLiteral("5"));
        dbgPkg = m_backend->package(name + QStringLiteral("-dbg"));
        if (dbgPkg) {
            return dbgPkg;
        }
    }

    return nullptr;
}

void DebugFinder::find(const QString &file)
{
    if (m_stop) {
        return;
    }

    QApt::Package *package = m_backend->packageForFile(file);
    QApt::Package *dbgPkg = getDbgPkg(package);
    if (!dbgPkg) {
        emit foundNoDbgPkg(file);
    } else if (dbgPkg->isInstalled()) {
        emit alreadyInstalled();
    } else {
        emit foundDbgPkg(dbgPkg->name());
    }
}

void DebugFinder::stop()
{
    m_stop = true;
}
