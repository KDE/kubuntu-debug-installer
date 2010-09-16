/*
  Copyright © 2010 Harald Sitter <apachelogger@ubuntu.com>

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

#include "dbginstaller.h"

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>

static const char description[] =
        I18N_NOOP("A debug package installer for Kubuntu");

static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("kubuntu-debug-installer", 0, ki18n("Debug package installer"),
                     version, ki18n(description), KAboutData::License_GPL,
                     ki18n("(C) 2010 Harald Sitter"), KLocalizedString(), 0,
                     "apachelogger@ubuntu.com");
    about.addAuthor( ki18n("Harald Sitter"), KLocalizedString(),
                     "apachelogger@ubuntu.com" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[FILES]", ki18n( "Files to find debug packages for" ));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    // do not restore!
    if (app.isSessionRestored()) {
        exit(0);
    }

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QStringList *arglist = new QStringList();
    for(int i = 0; i < args->count(); i++) {
        arglist->append(args->arg(i));
    }

    args->clear();

    DbgInstaller *installer = new DbgInstaller(0, about.programName(), arglist);
    installer->show();
    installer->run();

    return app.exec();
}
