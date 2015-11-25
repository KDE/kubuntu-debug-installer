/*
  Copyright © 2015 Harald Sitter <sitter@kde.org>

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

#include <QTest>
#include <QSignalSpy>

#include "../src/DebugFinder.h"

#include <QApt/Backend>

/*
  /usr/lib/x86_64-linux-gnu/libQt5Core.so.5
  /usr/lib/x86_64-linux-gnu/libkdeinit5_kwrite.so
  /lib/x86_64-linux-gnu/libglib-2.0.so.0
*/

class FinderTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testAlreadyInstalled()
    {
        QApt::Backend apt;
        apt.init();
        auto libc6_dbg = apt.package("libc6-dbg");
        apt.installPackages(QApt::PackageList() << libc6_dbg);
        apt.reloadCache();
        auto intalledFiles = apt.package("libc6")->installedFilesList();
        QString path;
        Q_FOREACH(const QString &file, intalledFiles) {
            if (!file.endsWith(".so")) {
                continue;
            }
            path = file;
            break;
        }
        QVERIFY(!path.isEmpty());
        qDebug() << path;

        DebugFinder finder;
        QSignalSpy spy(&finder, &DebugFinder::alreadyInstalled);
        QMetaObject::invokeMethod(&finder, "find", Qt::QueuedConnection,
                                  Q_ARG(QString, path));
        QVERIFY(spy.wait());
        QCOMPARE(spy.count(), 1);
    }

    void testDebugFound()
    {
        QApt::Backend apt;
        apt.init();
        apt.removePackages(QApt::PackageList() << apt.package("ksysguard-dbg"));

        DebugFinder finder;
        QSignalSpy spy(&finder, &DebugFinder::foundDbgPkg);
        QMetaObject::invokeMethod(&finder, "find", Qt::QueuedConnection,
                                  Q_ARG(QString, QString("/usr/bin/ksysguard")));
        QVERIFY(spy.wait());
        QCOMPARE(spy.count(), 1);
        QList<QVariant> sig = spy.takeAt(0);
        QCOMPARE(sig.count(), 1); // 1 arg
        QCOMPARE(sig.at(0).toString(), QStringLiteral("ksysguard-dbg"));
    }

    void testNoDebugFound()
    {
        DebugFinder finder;
        QSignalSpy spy(&finder, &DebugFinder::foundNoDbgPkg);
        QMetaObject::invokeMethod(&finder, "find", Qt::QueuedConnection,
                                  Q_ARG(QString, QString("/usr/lib/yolokitten.so.5")));
        QVERIFY(spy.wait());
        QCOMPARE(spy.count(), 1);
    }

    void testQt4X11()
    {
        QApt::Backend apt;
        apt.init();
        apt.removePackages(QApt::PackageList() << apt.package("libqt4-dbg"));

        DebugFinder finder;
        QSignalSpy spy(&finder, &DebugFinder::foundDbgPkg);
        QMetaObject::invokeMethod(&finder, "find", Qt::QueuedConnection,
                                  Q_ARG(QString, QString("/usr/lib/x86_64-linux-gnu/libQtCore.so.4")));
        QVERIFY(spy.wait());
        QCOMPARE(spy.count(), 1);
    }
};

QTEST_MAIN(FinderTest)

#include "findertest.moc"
