#include <QtCore/QSettings>

#include <QtWidgets/QApplication>

#include "MainWindowImpl.h"

#define ApplicationVersion "0.0.0.6"

int main (int argc, char **argv)
{
    QApplication app (argc, argv);
    app.setOrganizationDomain ("panter.org");
    app.setOrganizationName ("PanteR");
    app.setApplicationName ("Panther Commander");
    app.setApplicationVersion (ApplicationVersion);

    app.connect (&app, SIGNAL(lastWindowClosed ()), &app, SLOT(quit ()));

    QSettings::setDefaultFormat (QSettings::IniFormat);

    MainWindowImpl win;
    win.setWindowTitle (app.applicationName () + " " + app.applicationVersion ());
    win.show ();

    return app.exec ();
}
