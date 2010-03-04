#include "dbginstaller.h"
#include "ui_dbginstaller.h"

#include <QProcess>
#include <QProgressBar>

DbgInstaller::DbgInstaller(KCmdLineArgs *args, QWidget *parent) :
    QWidget(parent),
    m_args(args),
    m_dbgpkgs(new QList<QString>()),
    ui(new Ui::DbgInstaller)
{
    ui->setupUi(this);

    setWindowIcon(KIcon("kbugbuster"));
    setFixedSize(sizeHint());
}

DbgInstaller::~DbgInstaller()
{
    delete ui;
}

QString DbgInstaller::getPkgName(QString file)
{
    QProcess *query = new QProcess(this);
    query->start("dpkg-query", QStringList() << "-S" << file);
    query->waitForFinished();
    return query->readAll().split(':')[0];
}

QString DbgInstaller::getSrcPkg(QString pkg)
{
//    kDebug()<<pkg;
    QProcess *query = new QProcess(this);
    query->start("dpkg-query", QStringList() << "-W"
                      << "-f=${Source}" << pkg);
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
    setFixedSize(sizeHint());
    ui->progressBar->setMaximum(m_args->count());

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
            // yield error
        }

        m_dbgpkgs->append(dbgpkg);
    }

    kDebug() << *m_dbgpkgs;

    ui->progressBar->setValue(ui->progressBar->maximum());
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
