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
#include "ui_dbginstaller.h"

#include <KMessageBox>
#include <QProcess>
#include <QProgressBar>

DbgInstaller::DbgInstaller(KCmdLineArgs *args, KDialog *parent) :
    KDialog(parent),
    m_args(args),
    m_dbgpkgs(new QStringList()),
    m_nodbgpkgs(new QStringList()),
    ui(new Ui::DbgInstaller)
{
    setMainWidget(new QWidget);
    ui->setupUi(mainWidget());

    setButtons(0);
    setWindowIcon(KIcon("kbugbuster"));
    centerOnScreen(this);

    connect(this, SIGNAL(invokeRun()), this, SLOT(run()));

    show();
    emit invokeRun();
}

DbgInstaller::~DbgInstaller()
{
    delete ui;
}

void DbgInstaller::askInstall()
{
    kDebug();
    show();
    ui->progressBar->deleteLater();
    ui->label->setText(m_dbgpkgs->join("\n"));
    update();
    setButtons(KDialog::Ok | KDialog::Cancel);
    connect(this, SIGNAL(cancelClicked()), this, SLOT(close()));
}

QString DbgInstaller::getPkgName(QString file)
{
    QProcess *query = new QProcess(this);
    query->start("dpkg-query", QStringList() << "-S" << file);
    query->waitForFinished();
    return query->readAll().split(':')[0]; // really only return first hit?
}

QString DbgInstaller::getSrcPkg(QString pkg)
{
//    kDebug()<<pkg;
    QProcess *query = new QProcess(this);
    query->start("dpkg-query", QStringList() << "-W" << "-f=${Source}" << pkg);
    query->waitForFinished();
    return query->readAll();
}

QString DbgInstaller::getDebPkg(QString pkg)
{
    // TODO: map packages names

    QProcess *query = new QProcess;

    query->start(QString("apt-cache show %1-dbg").arg(pkg));
    query->waitForFinished();
    kdDebug() << query->exitCode();
    if (query->exitCode() == 0) {
        kdDebug() << query->exitCode();
        return QString("%1-dbg").arg(pkg);
    }

    query->start(QString("apt-cache show %1-dbgsym").arg(pkg));
    query->waitForFinished();
    if (query->exitCode() == 0) {
        return QString("%1-dbgsym").arg(pkg);
    }

    return "";
}

void DbgInstaller::run()
{
    ui->label->setText(i18n("looking up packages"));
    ui->progressBar->setMaximum(m_args->count());
    update();

    int i=0;
    for(; i < m_args->count(); i++)
    {
        kDebug()<<m_args->arg(i);
        ui->progressBar->setValue(i);

        QString pkg = getPkgName(m_args->arg(i));
        kDebug()<<"pkg: " << pkg;

        QString dbgpkg = getDebPkg(pkg);
        if (dbgpkg.isEmpty()) {
            QString srcpkg = getSrcPkg(pkg);
            kDebug()<<"srcpkg: " << srcpkg;
            dbgpkg = getDebPkg(srcpkg);
        }

        kDebug() << "dbg: " << dbgpkg;

        if (dbgpkg.isEmpty()) {
            m_nodbgpkgs->append(m_args->arg(i));
            continue;
        }

        m_dbgpkgs->append(dbgpkg);
    }

    ui->progressBar->setValue(ui->progressBar->maximum());

    kDebug() << *m_dbgpkgs;
    kDebug() << *m_nodbgpkgs;

    if (!m_nodbgpkgs->isEmpty()) {
        QString msgtext = i18n("I'm in ur repos, stealin ur dbg pacKagez."
                               " No newline for u! And no white space either"
                               "Do you want me to search anywayz?");
        int ret = KMessageBox::warningYesNoList(this, msgtext,
                                                *m_nodbgpkgs,
                                                i18n("Aint no debug packages"),
                                                KStandardGuiItem::yes(),
                                                KStandardGuiItem::no(),
                                                QString(),
                                                KMessageBox::Dangerous |
                                                KMessageBox::Notify);
        if (ret != KMessageBox::Yes) {
            exit(2);
        }
    }

    askInstall();
}

void DbgInstaller::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
