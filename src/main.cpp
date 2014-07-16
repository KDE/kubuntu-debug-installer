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

#include "DebugInstaller.h"

#include <QApplication>
#include <QCommandLineParser>

#include <KAboutData>
#include <KLocalizedString>

#include "Version.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KAboutData aboutData("kubuntu-debug-installer",
                         i18n("Debug package installer"),
                         version,
                         i18n("A debug package installer for Kubuntu"),
                         KAboutLicense::LicenseKey::GPL,
                         i18n("(C) 2010-2014 Harald Sitter"));

    aboutData.addAuthor(i18n("Harald Sitter"), QString(), QStringLiteral("apachelogger@ubuntu.com"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("FILES",
                                 i18n("Files to find debug packages for"));
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    // do not restore!
    if (app.isSessionRestored()) {
        exit(0);
    }

    DebugInstaller *installer = new DebugInstaller(parser.positionalArguments());
    installer->show();
    installer->run();

    return app.exec();
}
