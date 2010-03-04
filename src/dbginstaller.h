#ifndef DBGINSTALLER_H
#define DBGINSTALLER_H

#include <KCmdLineArgs>
#include <KDebug>

#include <QWidget>

namespace Ui {
    class DbgInstaller;
}

class DbgInstaller : public QWidget {
    Q_OBJECT
public:
    DbgInstaller(KCmdLineArgs *args, QWidget *parent = 0);
    ~DbgInstaller();
    void run();

protected:
    void changeEvent(QEvent *e);

private:
    QString getPkgName(QString);
    QString getSrcPkg(QString);
    QString getDebPkg(QString);

    KCmdLineArgs *m_args;
    QList<QString> *m_dbgpkgs;
    Ui::DbgInstaller *ui;
};

#endif // DBGINSTALLER_H
