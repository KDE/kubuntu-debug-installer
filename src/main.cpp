#include "dbginstaller.h"

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KMessageBox>

#define ERR_NO_ARGS 1

static const char description[] =
    I18N_NOOP("A debug symbol installer for KDE");

static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("installdbgsymbols", 0, ki18n("Debug symbol installer"),
                     version, ki18n(description), KAboutData::License_GPL,
                     ki18n("(C) 2010 Harald Sitter"), KLocalizedString(), 0,
                     "apachelogger@ubuntu.com");
    about.addAuthor( ki18n("Harald Sitter"), KLocalizedString(),
                     "apachelogger@ubuntu.com" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[FILES]", ki18n( "Files to find debug symbols for" ));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    // do not restore!
    if (app.isSessionRestored())
    {
      exit(0);
    }

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->count() == 0)
    {
      KMessageBox::error(0, i18n("No args, sorry sorry sorry"),
                         i18n("No args caption"));
      exit(ERR_NO_ARGS);
    }

    DbgInstaller *installer = new DbgInstaller(args);
    installer->setWindowTitle(about.programName());
    installer->show();
    installer->run();

    args->clear();

    return app.exec();
}
