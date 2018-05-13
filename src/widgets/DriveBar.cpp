#include <QtCore/QDir>

#include <QtWidgets/QAction>
#include <QtWidgets/QFileIconProvider>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolButton>

#ifdef Q_WS_WIN
#  include "filecontextmenu.h"
#endif

#include "FlowLayout.h"
#include "src/io/VolumeInfoProvider.h"

#include "DriveBar.h"

static bool isDrive (const QString &path)
{
    const int length = path.length ();
    return ((length == 1 && path.at (0) == QLatin1Char ('/'))
            || (length >= 2 && length <= 3 && path.at (0).isLetter () && path.at (1) == QLatin1Char (':')));
}

DriveBar::DriveBar (QWidget *parent)
    : QFrame (parent)
{
    setFocusPolicy (Qt::NoFocus);

    lastCheckedAction_ = 0;

    actionGroup_ = new QActionGroup (this);
    connect (actionGroup_, &QActionGroup::triggered, this, &DriveBar::actionTriggered);

    volumeInfoProvider_ = new VolumeInfoProvider (this);

    iconProvider_ = std::make_unique<QFileIconProvider> ();

    loadDrivesList ();

    connect (volumeInfoProvider_, &VolumeInfoProvider::volumeAdded, this, &DriveBar::refresh);
    connect (volumeInfoProvider_, &VolumeInfoProvider::volumeRemoved, this, &DriveBar::refresh);
}

DriveBar::~DriveBar ()
{
}

void DriveBar::loadDrivesList ()
{
    FlowLayout *layout = new FlowLayout ();
    layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (0);

    for (const QStorageInfo &info : VolumeInfoProvider ().volumes ()) {
        const QString &path = info.rootPath ();

        QAction *action = new QAction (this);
        action->setText (isDrive (path) ? path.left (1) : info.rootPath ());
        action->setIcon (iconProvider_->icon (QFileIconProvider::Drive));
        action->setToolTip (QDir::toNativeSeparators (path));
        action->setData (path);
        action->setCheckable (true);
        actionGroup_->addAction (action);

        QToolButton *button = new QToolButton (this);
        button->setIconSize (QSize (16, 16));
        button->setDefaultAction (action);
        button->setToolButtonStyle (Qt::ToolButtonTextBesideIcon);
        button->setAutoRaise (true);
        button->setFocusPolicy (Qt::NoFocus);
        button->setContextMenuPolicy (Qt::CustomContextMenu);
        connect (button, &QToolButton::customContextMenuRequested, this, &DriveBar::showContextMenu);

        layout->addWidget (button);
    }
    setLayout (layout);
}

void DriveBar::refresh ()
{
    QList<QToolButton *> buttons = findChildren<QToolButton *> ();
    qDeleteAll (buttons);
    qDeleteAll (actionGroup_->actions ());
    delete layout ();
    loadDrivesList ();
}

void DriveBar::actionTriggered (QAction *action)
{
    if (!action) {
        return;
    }

    const QString &path = action->data ().toString ();
    if (QFileInfo (path).isReadable ()) {
        lastCheckedAction_ = action;
        emit discChanged (path);
    } else {
        QMessageBox::critical (this, "", tr ("Drive %1 is not ready.").arg (QDir::toNativeSeparators (path)));
        if (lastCheckedAction_) {
            lastCheckedAction_->setChecked (true);
        }
    }
}

void DriveBar::slotSetDisc (const QString &path)
{
    const QString &m_path = QDir::fromNativeSeparators (path);

        foreach(QAction *action, actionGroup_->actions ()) {
            if (m_path.startsWith (action->data ().toString ())) {
                action->setChecked (true);
                lastCheckedAction_ = action;
                break;
            }
            action->setChecked (false);
        }
}

void DriveBar::showContextMenu (const QPoint &position)
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
