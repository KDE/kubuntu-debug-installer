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

#include "dbginstaller.h"
#include "dbglookupthread.h"

#include <KMessageBox>
#include <QProcess>

DbgInstaller::DbgInstaller(KProgressDialog *parent, QString caption,
                           QStringList *args) :
    KProgressDialog(parent, caption),
    m_args(args),
    m_dbgpkgs(new QStringList()),
    m_nodbgpkgs(new QStringList())
{
    setWindowIcon(KIcon("kbugbuster"));

    if (m_args->empty()) {
        KMessageBox::error(this, i18n("No file paths were provided, so no debug"
                                      " packages can be found."),
                           i18n("Can't lookup debug packages"));
        exit(ERR_RANDOM_ERR);
    }
}

DbgInstaller::~DbgInstaller()
{
    if (wasCancelled()) {
        exit(ERR_CANCEL);
    }
    delete m_dbgpkgs;
    delete m_nodbgpkgs;
}

void DbgInstaller::install()
{
    hide();
    QProcess *install = new QProcess(this);
    install->start(QString("kdesudo -- install-package --install %1")
                   .arg(m_dbgpkgs->join(" ")));
    install->waitForFinished();
    if(install->exitCode() != 0) {
        exit(ERR_RANDOM_ERR);
    }

    close();
}

void DbgInstaller::askMissing()
{
    QString msgtext = i18n("For the below listed files no debug packages could"
                           " be found.\n"
                           "Do you want to continue anyway?");
    QString msgcaption = i18n("Couldn't find debug packages");
    int ret = KMessageBox::warningYesNoList(this, msgtext, *m_nodbgpkgs,
                                            msgcaption);
    if (ret != KMessageBox::Yes) {
        exit(ERR_NO_SYMBOLS);
    }
}

void DbgInstaller::askInstall()
{
    QString msgtext = i18n("Do you want to install the following debug packages"
                           " so that the necessary debug symbols become available?");
    QString msgcaption = i18n("Do you want to install the debug packages?");
    int ret = KMessageBox::questionYesNoList(this, msgtext, *m_dbgpkgs,
                                             msgcaption);
    if(ret != KMessageBox::Yes) {
        exit(ERR_CANCEL);
    }

    install();
}

void DbgInstaller::incrementProgress()
{
    progressBar()->setValue(progressBar()->value()+1);
    if (progressBar()->value() == progressBar()->maximum()) {
        if (!m_nodbgpkgs->empty() && !m_dbgpkgs->empty()) {
            askMissing();
        } else if (m_dbgpkgs->empty()) {
            exit(ERR_NO_SYMBOLS);
        }
        askInstall();
    }
}

void DbgInstaller::foundDbgPkg(QString dbgpkg)
{
    if (!m_dbgpkgs->contains(dbgpkg)) {
        m_dbgpkgs->append(dbgpkg);
    }
    incrementProgress();
}

void DbgInstaller::foundNoDbgPkg(QString file)
{
    m_nodbgpkgs->append(file);
    incrementProgress();
}

void DbgInstaller::run()
{
    setLabelText(i18n("Looking up debug packages"));

    progressBar()->setMaximum(m_args->count());
    incrementProgress();

    DbgLookupThread *t = new DbgLookupThread(this, m_args);
    connect(t, SIGNAL(foundDbgPkg(QString)), this, SLOT(foundDbgPkg(QString)));
    connect(t, SIGNAL(foundNoDbgPkg(QString)), this, SLOT(foundNoDbgPkg(QString)));
    t->start();
}
