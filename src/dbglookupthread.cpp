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

#include "dbglookupthread.h"

#include <QFile>
#include <QProcess>

DbgLookupThread::DbgLookupThread(QObject *parent, QStringList *files) :
        QThread(parent),
        m_files(files)
{}

QString DbgLookupThread::getPkgName(QString file)
{
    QProcess *query = new QProcess();
    query->start("dpkg-query", QStringList() << "-S" << file);
    query->waitForFinished(-1);
    return query->readAll().split(':')[0]; // really only return first hit?
}

QString DbgLookupThread::getSrcPkg(QString pkg)
{
    QProcess *query = new QProcess();
    query->start("dpkg-query", QStringList() << "-W" << "-f=${Source}" << pkg);
    query->waitForFinished(-1);
    return query->readAll();
}

QString DbgLookupThread::getDebPkg(QString pkg)
{
    // TODO: map packages names for Qt
    if (pkg.contains("qt4-x11"))
    {
        pkg = "libqt4";
    }

    QProcess *query = new QProcess();

    query->start(QString("apt-cache show %1-dbgsym").arg(pkg));
    query->waitForFinished(-1);
    if (query->exitCode() == 0 &&
        !QFile::exists(QString("/var/lib/dpkg/info/%1-dbgsym.list").arg(pkg))) {
          return QString("%1-dbgsym").arg(pkg);
    }

    query->start(QString("apt-cache show %1-dbg").arg(pkg));
    query->waitForFinished(-1);
    if (query->exitCode() == 0 &&
        !QFile::exists(QString("/var/lib/dpkg/info/%1-dbg.list").arg(pkg)) ) {
          return QString("%1-dbg").arg(pkg);
    }

    return "";
}

void DbgLookupThread::run()
{
    int i=0;
    for(; i < m_files->count(); i++) {
        QString pkg;
        QString dbgpkg;

        pkg = getPkgName(m_files->at(i));
        dbgpkg = getDebPkg(pkg);

        if (dbgpkg.isEmpty()) {
            QString srcpkg = getSrcPkg(pkg);
            dbgpkg = getDebPkg(srcpkg);
        }

        if (dbgpkg.isEmpty()) {
            emit foundNoDbgPkg(m_files->at(i));
        } else {
            emit foundDbgPkg(dbgpkg);
        }
    }

    exec();
}
