#include <QtCore/QSettings>

#include <QtWidgets/QApplication>

#include "MainWindow.h"

#define ApplicationVersion "0.0.0.6"

int main (int argc, char **argv)
{
    QApplication app (argc, argv);
    app.setOrganizationDomain ("panter.org");
    app.setOrganizationName ("PanteR");
    app.setApplicationName ("Panther Commander");
    app.setApplicationVersion (ApplicationVersion);

    app.connect (&app, &QApplication::lastWindowClosed, &app, &QApplication::quit);

    QSettings::setDefaultFormat (QSettings::IniFormat);

    MainWindow win;
    win.setWindowTitle (app.applicationName () + " " + app.applicationVersion ());
    win.show ();

    return app.exec ();
}
