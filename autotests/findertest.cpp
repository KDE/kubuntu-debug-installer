#include <QTest>
#include <QSignalSpy>

#include "../src/DebugFinder.h"


/*
  /usr/lib/x86_64-linux-gnu/libQt5Core.so.5
  /usr/lib/x86_64-linux-gnu/libkdeinit5_kwrite.so
  /lib/x86_64-linux-gnu/libglib-2.0.so.0
*/

#include <QApt/Backend>

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
