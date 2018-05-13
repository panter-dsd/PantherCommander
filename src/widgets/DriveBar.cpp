#include <QtCore/QDir>

#include <QtWidgets/QAction>
#include <QtWidgets/QFileIconProvider>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolButton>

#include "DriveBar.h"
#include "src/AppSettings.h"

#ifdef Q_WS_WIN
#  include "filecontextmenu.h"
#endif

#include "FlowLayout.h"
#include "src/io/VolumeInfoProvider.h"

static bool isDrive (const QString &path)
{
    int length = path.length ();
    return ((length == 1 && path.at (0) == QLatin1Char ('/'))
            || (length >= 2 && length <= 3 && path.at (0).isLetter () && path.at (1) == QLatin1Char (':')));
}

DriveBar::DriveBar (QWidget *parent)
    : QFrame (parent)
{
    setFocusPolicy (Qt::NoFocus);

    lastChecked = 0;

    actionGroup = new QActionGroup (this);
    connect (actionGroup, SIGNAL(triggered (QAction * )), this, SLOT(_q_actionTriggered (QAction * )));

    provider = new VolumeInfoProvider (this);

    iconProvider = new QFileIconProvider ();

    loadDrivesList ();

    connect (provider, SIGNAL(volumeAdded (
                                  const QString&)), this, SLOT(volumeAdd (QString)));
    connect (provider, SIGNAL(volumeRemoved (
                                  const QString&)), this, SLOT(volumeRemove (QString)));
}

DriveBar::~DriveBar ()
{
    delete iconProvider;
}

void DriveBar::loadDrivesList ()
{
    FlowLayout *layout = new FlowLayout ();
    layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (0);

    QAction *action;
    QToolButton *button;

    for (const QStorageInfo &info : VolumeInfoProvider ().volumes ()) {
        const QString &path = info.rootPath ();

        action = new QAction (this);
        action->setText (isDrive (path) ? path.left (1) : info.rootPath ());
#ifdef Q_WS_WIN
        action->setIcon(iconProvider->icon(fi));
#else
        action->setIcon (iconProvider->icon (QFileIconProvider::Drive));
#endif
        action->setToolTip (QDir::toNativeSeparators (path));
        action->setData (path);
        action->setCheckable (true);
        actionGroup->addAction (action);

        button = new QToolButton (this);
        button->setIconSize (QSize (16, 16));
        button->setDefaultAction (action);
        button->setToolButtonStyle (Qt::ToolButtonTextBesideIcon);
        button->setAutoRaise (true);
        button->setFocusPolicy (Qt::NoFocus);
        button->setContextMenuPolicy (Qt::CustomContextMenu);
        connect (button, SIGNAL(customContextMenuRequested (QPoint)),
                 this, SLOT(_q_showContextMenu (QPoint)));

        layout->addWidget (button);
    }
    setLayout (layout);
}

void DriveBar::volumeAdd (const QString &)
{
    QList<QToolButton *> buttons = findChildren<QToolButton *> ();
    qDeleteAll (buttons);
    qDeleteAll (actionGroup->actions ());
    delete layout ();
    loadDrivesList ();
}

void DriveBar::volumeRemove (const QString &)
{
    QList<QToolButton *> buttons = findChildren<QToolButton *> ();
    qDeleteAll (buttons);
    qDeleteAll (actionGroup->actions ());
    delete layout ();
    loadDrivesList ();
}

void DriveBar::_q_actionTriggered (QAction *action)
{
    if (!action) {
        return;
    }

    const QString &path = action->data ().toString ();
    if (QFileInfo (path).isReadable ()) {
        lastChecked = action;
        emit discChanged (path);
    } else {
        QMessageBox::critical (this, "", tr ("Drive %1 is not ready.").arg (QDir::toNativeSeparators (path)));
        if (lastChecked) {
            lastChecked->setChecked (true);
        }
    }
}

void DriveBar::slotSetDisc (const QString &path)
{
    const QString &m_path = QDir::fromNativeSeparators (path);

        foreach(QAction *action, actionGroup->actions ()) {
            if (m_path.startsWith (action->data ().toString ())) {
                action->setChecked (true);
                lastChecked = action;
                break;
            }
            action->setChecked (false);
        }
}

void DriveBar::_q_showContextMenu (const QPoint &position)
{
#ifdef QT_NO_MENU
    Q_UNUSED(position);
#else
    QToolButton *button = qobject_cast<QToolButton *> (sender ());
    if (!button) {
        return;
    }

    const QString &path = button->defaultAction ()->data ().toString ();

    QPoint globalPos = button->mapToGlobal (position);
#ifdef Q_WS_WIN
    FileContextMenu menu(this);
    menu.setPath(path);
    menu.executeNativeMenu(globalPos);
#endif // Q_WS_WIN
#endif // QT_NO_MENU
}
