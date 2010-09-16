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

#include "DebugInstaller.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include <KLocale>
#include <KMessageBox>

#include "DebugFinder.h"

// Because qapp->exit only ends event processing the current function would not
// end, hence it is necessary to ensure that the function returns immediately.
// Also ensure that the finderthread's eventloop gets stopped to prevent
// crashes from access to implicitly shared QString data.
#define exit(x) m_finder->stop(); m_finderThread->quit(); m_finderThread->wait(); qApp->exit(x); return;

DebugInstaller::DebugInstaller(KProgressDialog *parent, const QString &caption,
                           QStringList *args) :
    KProgressDialog(parent, caption),
    m_args(args),
    m_dbgpkgs(new QStringList()),
    m_nodbgpkgs(new QStringList()),
    m_gotAlreadyInstalled(false)
{
    setWindowIcon(KIcon("kbugbuster"));

    if (m_args->empty()) {
        KMessageBox::error(this, i18nc("@info Error message", "No file paths"
                                       " were provided, so no debug"
                                       " packages can be found."),
                           i18nc("@title:window", "Can't lookup debug packages"));
        exit(ERR_RANDOM_ERR);
    }
}

DebugInstaller::~DebugInstaller()
{
    delete m_dbgpkgs;
    delete m_nodbgpkgs;
}

void DebugInstaller::install()
{
    QProcess install;
    install.start(QString("/usr/bin/qapt-batch --install %1")
                   .arg(m_dbgpkgs->join(QString(' '))));
    // use blocking function since we do not show any UI
    install.waitForFinished(-1);
    if (install.exitCode() != 0) {
        exit(ERR_RANDOM_ERR);
    }

    close();
}

void DebugInstaller::askMissing()
{
    checkListEmpty();
    QString msgtext = i18nc("@info", "No debug packages could be found for the files"
                           " listed below.\n"
                           "Do you want to continue anyway?");
    QString msgcaption = i18nc("@title:window", "Couldn't find debug packages");
    int ret = KMessageBox::warningYesNoList(this, msgtext, *m_nodbgpkgs,
                                            msgcaption,
                                            KStandardGuiItem::cont(),
                                            KStandardGuiItem::cancel());
    if (ret != KMessageBox::Yes) {
        exit(ERR_NO_SYMBOLS);
    }
}

void DebugInstaller::askInstall()
{
    checkListEmpty();
    QString msgtext = i18nc("@info", "Do you want to install the following debug packages"
                           " so that the necessary debug symbols become available?");
    QString msgcaption = i18nc("@title:window", "Confirm package installation");
    int ret = KMessageBox::questionYesNoList(this, msgtext, *m_dbgpkgs,
                                             msgcaption,
                                             KGuiItem(i18nc("@action:button", "Install"),
                                                      "dialog-ok"),
                                             KStandardGuiItem::cancel());
    if (ret != KMessageBox::Yes) {
        exit(ERR_CANCEL);
    }

    install();
}

void DebugInstaller::checkListEmpty() const
{
    if (!m_dbgpkgs->isEmpty()) {
        return;
    }

    exit(ERR_NO_SYMBOLS);
}

void DebugInstaller::incrementProgress()
{
    progressBar()->setValue(progressBar()->value() + 1);
    if (progressBar()->value() == progressBar()->maximum()) {
        if (!m_nodbgpkgs->isEmpty() && !m_dbgpkgs->isEmpty()) {
            askMissing();
        } else if (m_dbgpkgs->isEmpty() && m_nodbgpkgs->isEmpty() && m_gotAlreadyInstalled) {
            exit(0);
        } else if (m_dbgpkgs->isEmpty()) {
            exit(ERR_NO_SYMBOLS);
        }
        askInstall();
    }
}

void DebugInstaller::foundDbgPkg(const QString &dbgpkg)
{
    if (!m_dbgpkgs->contains(dbgpkg)) {
        m_dbgpkgs->append(dbgpkg);
    }
    incrementProgress();
}

void DebugInstaller::foundNoDbgPkg(const QString &file)
{
    m_nodbgpkgs->append(file);
    incrementProgress();
}

void DebugInstaller::alreadyInstalled()
{
    m_gotAlreadyInstalled = true;
    incrementProgress();
}

void DebugInstaller::reject()
{
    KProgressDialog::reject();
    exit(ERR_CANCEL);
}

void DebugInstaller::run()
{
    setLabelText(i18nc("@info:progress", "Looking for debug packages"));

    progressBar()->setMaximum(m_args->count());
    incrementProgress();

    m_finder = new DebugFinder(0, m_args);
    connect(m_finder, SIGNAL(foundDbgPkg(QString)), this, SLOT(foundDbgPkg(QString)));
    connect(m_finder, SIGNAL(foundNoDbgPkg(QString)), this, SLOT(foundNoDbgPkg(QString)));
    connect(m_finder, SIGNAL(alreadyInstalled()), this, SLOT(alreadyInstalled()));

    m_finderThread = new QThread(this);
    m_finder->moveToThread(m_finderThread);
    m_finderThread->start();

    foreach (const QString &file, *m_args) {
        QMetaObject::invokeMethod(m_finder, "find", Qt::QueuedConnection,
                                  Q_ARG(QString, file));
    }
}
