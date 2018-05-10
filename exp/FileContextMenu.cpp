#include "FileContextMenu.h"

#define WINVER        0x0500
#define _WIN32_WINNT  0x0500
#define _WIN32_IE     0x0500

#include <qt_windows.h>
#include <objbase.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <QtCore/QDir>
#include <QtCore/QPointer>
#include <QtCore/QVarLengthArray>

#include <QtGui/QMoveEvent>

#include <qdebug.h>

// workaround for mingw
#ifndef CMIC_MASK_PTINVOKE
#  define CMIC_MASK_PTINVOKE 0x20000000L

typedef struct _CMInvokeCommandInfoEx
{
    DWORD cbSize;
    DWORD fMask;
    HWND hwnd;
    LPCSTR lpVerb;
    LPCSTR lpParameters;
    LPCSTR lpDirectory;
    int nShow;
    DWORD dwHotKey;
    HANDLE hIcon;
    LPCSTR lpTitle;
    LPCWSTR lpVerbW;
    LPCWSTR lpParametersW;
    LPCWSTR lpDirectoryW;
    LPCWSTR lpTitleW;
    POINT ptInvoke;
} CMINVOKECOMMANDINFOEX, *LPCMINVOKECOMMANDINFOEX;
#endif

#define MIN_ID 1
#define MAX_ID 10000

class FileContextMenuPrivate
{
public:
    FileContextMenuPrivate ();

    ~FileContextMenuPrivate ();

    void parseNativeMenuItem (HMENU menu, int index, FileContextMenuAction *action);

    void parseNativeMenu (HMENU menu, QMenu *qmenu);

    void _q_nativeActionTriggered ();

    FileContextMenu *q;

    WId hWnd;
    LPCONTEXTMENU pContextMenu;
    HMENU menu;

    QPoint pos;

    IShellFolder *psfFolder;

    static LRESULT CALLBACK
    HookWndProc(HWND
    hWnd,
    UINT message, WPARAM
    wParam,
    LPARAM lParam
    );
};

FileContextMenuPrivate::FileContextMenuPrivate ()
    : q (0)
    , hWnd (0)
    , pContextMenu (0)
    , menu (0)
    , psfFolder (0)
{
}

FileContextMenuPrivate::~FileContextMenuPrivate ()
{
    if (menu) {
        DestroyMenu (menu);
    }
    if (pContextMenu) {
        pContextMenu->Release ();
    }
    if (psfFolder) {
        psfFolder->Release ();
    }
    psfFolder = 0;
}

void FileContextMenuPrivate::_q_nativeActionTriggered ()
{
    FileContextMenuAction *action = qobject_cast<FileContextMenuAction *> (q->sender ());
    if (action && pContextMenu) {
        int idCommand = action->itemData;
        if (idCommand >= MIN_ID && idCommand <= MAX_ID) {
            CMINVOKECOMMANDINFOEX command = {0};
            command.cbSize = sizeof (CMINVOKECOMMANDINFOEX);
            command.hwnd = hWnd;
            command.lpVerb = (LPSTR) MAKEINTRESOURCE (idCommand - MIN_ID);
            command.nShow = SW_SHOWNORMAL;

            if (!pos.isNull ()) {
                command.fMask |= CMIC_MASK_PTINVOKE;
                command.ptInvoke.x = pos.x ();
                command.ptInvoke.y = pos.y ();
            }

            pContextMenu->InvokeCommand ((LPCMINVOKECOMMANDINFO) & command);
        }
    }
}

FileContextMenu::FileContextMenu (QWidget *parent)
    : QMenu (parent)
    , d (new FileContextMenuPrivate)
{
    d->q = this;
    d->hWnd = window ()->internalWinId ();
    // force native window creation
    if (!d->hWnd) {
        d->hWnd = winId ();
    }
}

FileContextMenu::FileContextMenu (const QString &title, QWidget *parent)
    : QMenu (title, parent)
    , d (new FileContextMenuPrivate)
{
    d->q = this;
    d->hWnd = window ()->internalWinId ();
    // force native window creation
    if (!d->hWnd) {
        d->hWnd = winId ();
    }
}

FileContextMenu::~FileContextMenu ()
{
    delete d;
}

LRESULT CALLBACK
FileContextMenuPrivate::HookWndProc(HWND
hWnd,
UINT message, WPARAM
wParam,
LPARAM lParam
)
{
switch(message)
{
case WM_MENUCHAR:
{
// only supported by IContextMenu3
IContextMenu2 *pContextMenu2 = (LPCONTEXTMENU2) GetProp (hWnd, TEXT ("ContextMenu"));
IContextMenu3 *pContextMenu3 = 0;
if(pContextMenu2->
QueryInterface(IID_IContextMenu3,
(void**)&pContextMenu3) == NOERROR)
{
LRESULT lResult = 0;
pContextMenu3->
HandleMenuMsg2(message, wParam, lParam, &lResult
);
return
lResult;
}
}
break;

case WM_DRAWITEM:
case WM_MEASUREITEM:
if(wParam)
{
// if wParam != 0 then the message is not menu-related
break;
}

case WM_INITMENUPOPUP:
{
IContextMenu2 *pContextMenu2 = (LPCONTEXTMENU2) GetProp (hWnd, TEXT ("ContextMenu"));
IContextMenu3 *pContextMenu3 = 0;
if(pContextMenu2->
QueryInterface(IID_IContextMenu3,
(void**)&pContextMenu3) == NOERROR)
pContextMenu3->
HandleMenuMsg(message, wParam, lParam
);
else
pContextMenu2->
HandleMenuMsg(message, wParam, lParam
);
return (message == WM_INITMENUPOPUP ? 0 : TRUE); // inform caller that we handled WM_INITPOPUPMENU by ourself
}
break;

default:
break;
}

return
::CallWindowProc((WNDPROC)
GetProp(hWnd, TEXT ("OldWndProc")
), hWnd, message, wParam, lParam);
}

void FileContextMenuPrivate::parseNativeMenuItem (HMENU menu, int index, FileContextMenuAction *action)
{
    QMenu *qmenu = qobject_cast<QMenu *> (action->parentWidget ());
    Q_ASSERT(qmenu);

    QVarLengthArray<wchar_t, 64> buf (63);

    MENUITEMINFO info = {0};
    info.cbSize = sizeof (MENUITEMINFO);

    info.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_DATA | MIIM_CHECKMARKS | MIIM_SUBMENU;
    info.cch = buf.size ();
    info.dwTypeData = buf.data ();

    if (!GetMenuItemInfo (menu, index, true, &info))
        qWarning () << "!!!!!!!";

    if (info.fMask & (MIIM_FTYPE | MIIM_TYPE)) {
        if (info.fType & MFT_RADIOCHECK) {
            action->setCheckable (true);

            // !msdn: if the MFT_RADIOCHECK type value is specified,
            // the default bitmap is a bullet; otherwise, it is a check mark.
            if (!info.hbmpChecked) {
                // ### TODO
            }
        }
    }

    if (info.fMask & MIIM_STATE) {
        action->setChecked (info.fState & MFS_CHECKED);
        action->setDisabled (info.fState & MFS_DISABLED);
        if (info.fState & MFS_DEFAULT) {
            qmenu->setDefaultAction (action);
        }
        if (info.fState & MFS_HILITE) {
            qmenu->setActiveAction (action);
        }
    }

    if (info.fMask & MIIM_ID) {
        action->itemData = info.wID;
    }

    if (info.fMask & MIIM_DATA) {
        action->itemUserData = info.dwItemData;
    }

    if (info.fMask & MIIM_CHECKMARKS) {
        if (info.hbmpUnchecked || info.hbmpChecked) {
            HBITMAP bmp = info.hbmpChecked ? info.hbmpChecked : info.hbmpUnchecked;
            QIcon icon (QPixmap::fromWinHBITMAP(bmp, QPixmap::Alpha));
            action->setIcon (icon);
        }
    }

    if (info.fMask & MIIM_SUBMENU) {
        if (info.hSubMenu) {
            QMenu *subMenu = new QMenu (qmenu);
            action->setMenu (subMenu);

            parseNativeMenu (info.hSubMenu, subMenu);
        }
    }

    if (info.fType == MFT_SEPARATOR) {
        action->setSeparator (true);
    }

    if (info.fType == MFT_BITMAP) {
        if (info.hbmpItem && info.hbmpItem != HBMMENU_CALLBACK) {
            QIcon icon (QPixmap::fromWinHBITMAP (info.hbmpItem, QPixmap::Alpha));
            action->setIcon (icon);
        }
    }

    int cch = info.cch + 1;
    if ((info.dwTypeData && info.dwTypeData != buf.data ()) || cch >= buf.size ()) {
        if (cch >= buf.size ()) {
            buf.resize (cch);
        }

        info.fMask = MIIM_STRING | MIIM_FTYPE;
        info.cch = cch;
        info.dwTypeData = buf.data ();

        if (!GetMenuItemInfo (menu, index, true, &info))
            qWarning () << "!!!!!!!";
    }

    if (info.fType == MIIM_STRING || info.dwTypeData == buf.data ()) {
        action->setText (QString::fromWCharArray (buf.data (), info.cch));
    }
}

void FileContextMenuPrivate::parseNativeMenu (HMENU menu, QMenu *qmenu)
{
    int itemCount = GetMenuItemCount (menu);
    for (int i = 0; i < itemCount; ++i) {
        FileContextMenuAction *action = new FileContextMenuAction (qmenu);
        q->connect (action, SIGNAL(triggered (bool)), q, SLOT(_q_nativeActionTriggered ()));
        qmenu->addAction (action);

        parseNativeMenuItem (menu, i, action);
    }
}

QAction *FileContextMenu::executeNativeMenu (const QPoint &pos)
{
    QAction *ret = 0;

    // subclass window to handle messages
    WNDPROC OldWndProc = 0;
    // only subclass if its version 2 or 3
    IContextMenu2 *tmpContextMenu2 = 0;
    if (d->pContextMenu->QueryInterface (IID_IContextMenu3, (void **) &tmpContextMenu2) == NOERROR) {
        if (SetProp (d->hWnd, TEXT ("ContextMenu"), d->pContextMenu)) {
            OldWndProc = (WNDPROC)
        }
        SetWindowLongA (d->hWnd, GWL_WNDPROC, (LONG) FileContextMenuPrivate::HookWndProc);
    }

    QPointer<QObject> guard = this;
    int idCommand = TrackPopupMenuEx (d->menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
                                      pos.x (), pos.y (), d->hWnd, 0
    );
    if (guard.isNull ()) {
        return 0;
    }

    // unsubclass
    if (OldWndProc) {
        SetWindowLongA (d->hWnd, GWL_WNDPROC, (LONG) OldWndProc);
        RemoveProp (d->hWnd, TEXT ("ContextMenu"));
    }

    if (idCommand >= MIN_ID && idCommand <= MAX_ID) {
            foreach(QAction *act, actions ()) {
                FileContextMenuAction *action = qobject_cast<FileContextMenuAction *> (act);
                if (action && action->itemData == idCommand) {
                    act->trigger ();
                    ret = act;
                }
            }
    }

    return ret;
}

void FileContextMenu::moveEvent (QMoveEvent *event)
{
    d->pos = isVisible () ? event->pos () : QPoint ();

    QMenu::moveEvent (event);
}

void FileContextMenu::setPath (const QString &path)
{
    setPaths (QStringList (path));
}

static UINT GetPIDLSize (LPCITEMIDLIST pidl)
{
    if (!pidl) {
        return 0;
    }
    int nSize = 0;
    LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;
    while (pidlTemp->mkid.cb) {
        nSize += pidlTemp->mkid.cb;
        pidlTemp = (LPITEMIDLIST) (((LPBYTE) pidlTemp) + pidlTemp->mkid.cb);
    }
    return nSize;
}

static LPITEMIDLIST CopyPIDL (LPCITEMIDLIST pidl)
{
    int cb = GetPIDLSize (pidl); // Calculate size of list.

    LPITEMIDLIST pidlRet = (LPITEMIDLIST) calloc (cb + sizeof (USHORT), sizeof (BYTE));
    if (pidlRet) {
        CopyMemory (pidlRet, pidl, cb);
    }

    return pidlRet;
}

void FileContextMenu::setPaths (const QStringList &paths1)
{
    // free all allocated datas
    if (d->psfFolder) {
        d->psfFolder->Release ();
    }
    d->psfFolder = 0;
    LPITEMIDLIST *m_pidlArray = 0;

    if (paths1.isEmpty ()) {
        return;
    }

    QStringList paths;
        foreach(const QString &path, paths1) {
            paths.append (QDir::toNativeSeparators (QDir::cleanPath (path)));
        }

    // get IShellFolder interface of Desktop (root of shell namespace)
    IShellFolder *psfDesktop = 0;
    SHGetDesktopFolder (&psfDesktop);    // needed to obtain full qualified pidl

    // ParseDisplayName creates a PIDL from a file system path relative to the IShellFolder interface
    // but since we use the Desktop as our interface and the Desktop is the namespace root
    // that means that it's a fully qualified PIDL, which is what we need
    LPITEMIDLIST pidl = 0;
    psfDesktop->ParseDisplayName (0, 0, (wchar_t *) paths.first ().utf16 (), 0, &pidl, 0);

    // now we need the parent IShellFolder interface of pidl, and the relative PIDL to that interface
    SHBindToParent (pidl, IID_IShellFolder, (void **) &d->psfFolder, 0);
    qWarning () << paths << pidl;
    CoTaskMemFree (pidl);

    // now we have the IShellFolder interface to the parent folder specified in the first element in strArray
    // since we assume that all objects are in the same folder (as it's stated in the MSDN)
    // we now have the IShellFolder interface to every objects parent folder

    LPITEMIDLIST pidlItem = 0;    // relative pidl

    IShellFolder *psfFolder = 0;
    int size = paths.size ();
    for (int i = 0; i < size; ++i) {
        psfDesktop->ParseDisplayName (0, 0, (wchar_t *) paths.at (i).utf16 (), 0, &pidl, 0);

        m_pidlArray = (LPITEMIDLIST *) realloc (m_pidlArray, (i + 1) * sizeof (LPITEMIDLIST));
        // get relative pidl via SHBindToParent
        SHBindToParent (pidl, IID_IShellFolder, (void **) &psfFolder, (LPCITEMIDLIST * ) & pidlItem);
        m_pidlArray[i] = CopyPIDL (pidlItem);    // copy relative pidl to pidlArray
        CoTaskMemFree (pidlItem);
        CoTaskMemFree (pidl);    // free pidl allocated by ParseDisplayName
        psfFolder->Release ();
    }

    psfDesktop->Release ();

    d->psfFolder->GetUIObjectOf (d->hWnd, size, (LPCITEMIDLIST *) m_pidlArray, IID_IContextMenu, 0,
                                 (void **) &d->pContextMenu
    );
    CoTaskMemFree (m_pidlArray);

    d->menu = CreatePopupMenu ();

    d->pContextMenu->QueryContextMenu (d->menu, 0, MIN_ID, MAX_ID, CMF_NORMAL | CMF_EXPLORE | CMF_CANRENAME);

    d->parseNativeMenu (d->menu, this);
}

#include "moc_filecontextmenu.cpp"
