#include "filecontextmenu.h"

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

#include <QtGui/QLabel>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>

#include <qdebug.h>

// workaround for mingw
#ifndef CMIC_MASK_PTINVOKE
#  define CMIC_MASK_PTINVOKE 0x20000000L

typedef struct _CMInvokeCommandInfoEx {
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
#ifndef ODS_NOACCEL
#  define ODS_NOACCEL 0x0100
#endif
#ifndef ODS_NOFOCUSRECT
#  define ODS_NOFOCUSRECT 0x0200
#endif

#define MIN_ID 1
#define MAX_ID 10000
#include <QCoreApplication>
static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
MSG msg;
msg.hwnd = hWnd;
msg.message = message;
msg.wParam = wParam;
msg.lParam = lParam;
if(message != WM_ENTERIDLE && message != WM_PAINT && message != WM_MOUSEMOVE)
	qWarning() << msg;

	switch(message)
	{
		case WM_MENUCHAR:
		{
			// only supported by IContextMenu3
			IContextMenu2* pContextMenu2 = (LPCONTEXTMENU2)GetProp(hWnd, TEXT("ContextMenu"));
			IContextMenu3* pContextMenu3 = 0;
			if(pContextMenu2->QueryInterface(IID_IContextMenu3, (void**)&pContextMenu3) == NOERROR)
			{
				LRESULT lResult = 0;
				pContextMenu3->HandleMenuMsg2(message, wParam, lParam, &lResult);
				return lResult;
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
			else if(message == WM_MEASUREITEM)
			{
		MEASUREITEMSTRUCT* mi = (MEASUREITEMSTRUCT*)lParam;
qWarning() << "MEASUREITEMSTRUCT:"
		<< "CtlType:" << mi->CtlType
		<< "CtlID:" << mi->CtlID
		<< "itemID:" << mi->itemID
		<< "itemWidth:" << mi->itemWidth
		<< "itemHeight:" << mi->itemHeight
		<< "itemData:" << mi->itemData;
			}
			else if(message == WM_DRAWITEM)
			{
		DRAWITEMSTRUCT* di = (DRAWITEMSTRUCT*)lParam;
//di->hwndItem = 0;
//di->hDC = GetDC(0);
//di->itemID = 108;
//di->itemData = 3;
		QRect r(QPoint(di->rcItem.left, di->rcItem.top), QPoint(di->rcItem.right, di->rcItem.bottom));
qWarning() << "DRAWITEMSTRUCT:"
		<< "CtlType:" << di->CtlType
		<< "CtlID:" << di->CtlID
		<< "itemID:" << di->itemID
		<< "itemAction:" << di->itemAction
		<< "itemState:" << di->itemState
		<< "hwndItem:" << di->hwndItem
		<< "hDC:" << di->hDC
		<< "rcItem:" << r
		<< "itemData:" << di->itemData;
			}

		case WM_INITMENUPOPUP:
		{
			IContextMenu2* pContextMenu2 = (LPCONTEXTMENU2)GetProp(hWnd, TEXT("ContextMenu"));
			IContextMenu3* pContextMenu3 = 0;
			if(pContextMenu2->QueryInterface(IID_IContextMenu3, (void**)&pContextMenu3) == NOERROR)
				pContextMenu3->HandleMenuMsg(message, wParam, lParam);
			else
				pContextMenu2->HandleMenuMsg(message, wParam, lParam);

			if(message == WM_MEASUREITEM)
			{
		MEASUREITEMSTRUCT* mi = (MEASUREITEMSTRUCT*)lParam;
qWarning() << "MEASUREITEMSTRUCT:"
		<< "CtlType:" << mi->CtlType
		<< "CtlID:" << mi->CtlID
		<< "itemID:" << mi->itemID
		<< "itemWidth:" << mi->itemWidth
		<< "itemHeight:" << mi->itemHeight
		<< "itemData:" << mi->itemData;
			}
			else if(message == WM_DRAWITEM)
			{
		DRAWITEMSTRUCT* di = (DRAWITEMSTRUCT*)lParam;
		QRect r(QPoint(di->rcItem.left, di->rcItem.top), QPoint(di->rcItem.right, di->rcItem.bottom));
qWarning() << "DRAWITEMSTRUCT:"
		<< "CtlType:" << di->CtlType
		<< "CtlID:" << di->CtlID
		<< "itemID:" << di->itemID
		<< "itemAction:" << di->itemAction
		<< "itemState:" << di->itemState
		<< "hwndItem:" << di->hwndItem
		<< "hDC:" << di->hDC
		<< "rcItem:" << r
		<< "itemData:" << di->itemData;
			}

			return (message == WM_INITMENUPOPUP ? 0 : TRUE); // inform caller that we handled WM_INITPOPUPMENU by ourself
		}
			break;

		default:
			break;
	}

	// call original WndProc of window to prevent undefined bevhaviour of window
	return ::CallWindowProc((WNDPROC)GetProp(hWnd, TEXT("OldWndProc")), hWnd, message, wParam, lParam);
}


class FileContextMenuActionPrivate
{
public:
	FileContextMenuActionPrivate() :
		menu(0), itemData(0), itemUserData(0), ownerDraw(0)
	{}
	~FileContextMenuActionPrivate()
	{}

	HMENU menu;
	int itemData;
	qint64 itemUserData;
	bool ownerDraw;
	QPixmap pixmap;
};


FileContextMenuAction::FileContextMenuAction(QObject* parent) : QWidgetAction(parent),
	d(new FileContextMenuActionPrivate)
{
}

FileContextMenuAction::~FileContextMenuAction()
{
	delete d;
}


class FileContextMenuPrivate
{
public:
	FileContextMenuPrivate();
	~FileContextMenuPrivate();

	bool paintNativeMenuItem(EvilWidget* w);
	void parseNativeMenu(HMENU menu, QMenu* qmenu);

	void registerHook(HWND hWnd);
	void unregisterHook(HWND hWnd);

	void _q_aboutToShow();
	void _q_aboutToHide();
	void _q_nativeActionTriggered();

	FileContextMenu* q;

	WId hWnd;
	LPCONTEXTMENU pContextMenu;
	HMENU menu;

	QPoint pos;

	IShellFolder* psfFolder;
};

FileContextMenuPrivate::FileContextMenuPrivate() : q(0),
	hWnd(0), pContextMenu(0), menu(0),
	psfFolder(0)
{
}

FileContextMenuPrivate::~FileContextMenuPrivate()
{
	if(menu)
		DestroyMenu(menu);
	if(pContextMenu)
		pContextMenu->Release();
	if(psfFolder)
		psfFolder->Release();
	psfFolder = 0;
}

void FileContextMenuPrivate::registerHook(HWND hWnd)
{
	// subclass window to handle messages
	// if context menu version 2 or 3
	IContextMenu2* tmpContextMenu2 = 0;
	if(pContextMenu->QueryInterface(IID_IContextMenu3, (void**)&tmpContextMenu2) == NOERROR)
	{
		if(SetProp(hWnd, TEXT("ContextMenu"), pContextMenu))
		{
			WNDPROC oldWndProc = (WNDPROC)SetWindowLongA(hWnd, GWL_WNDPROC, (LONG)HookWndProc);
			//WNDPROC oldWndProc = (WNDPROC)GetWindowLongA(hWnd, GWL_WNDPROC);
			SetProp(hWnd, TEXT("OldWndProc"), (void*)oldWndProc);
		}
	}
}

void FileContextMenuPrivate::unregisterHook(HWND hWnd)
{
	// unsubclass
	WNDPROC oldWndProc = (WNDPROC)GetProp(hWnd, TEXT("OldWndProc"));
	if(oldWndProc)
	{
		SetWindowLongA(hWnd, GWL_WNDPROC, (LONG)oldWndProc);
		RemoveProp(hWnd, TEXT("ContextMenu"));
	}
}

void FileContextMenuPrivate::_q_aboutToShow()
{
	registerHook(hWnd);
}

void FileContextMenuPrivate::_q_aboutToHide()
{
	unregisterHook(hWnd);
}

void FileContextMenuPrivate::_q_nativeActionTriggered()
{
	FileContextMenuAction* action = qobject_cast<FileContextMenuAction*>(q->sender());
	if(action && pContextMenu)
	{
		int idCommand = action->d->itemData;
		if(idCommand >= MIN_ID && idCommand <= MAX_ID)
		{
qWarning() << "InvokeCommand" << idCommand;

			CMINVOKECOMMANDINFOEX command = { 0 };
			command.cbSize = sizeof(CMINVOKECOMMANDINFOEX);
			command.hwnd = hWnd;
			command.lpVerb = (LPSTR)MAKEINTRESOURCE(idCommand - MIN_ID);
			command.nShow = SW_SHOWNORMAL;

			if(!pos.isNull())
			{
				command.fMask |= CMIC_MASK_PTINVOKE;
				command.ptInvoke.x = pos.x();
				command.ptInvoke.y = pos.y();
			}

			pContextMenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&command);
		}
	}
}


FileContextMenu::FileContextMenu(QWidget* parent) : QMenu(parent),
	d(new FileContextMenuPrivate)
{
	d->q = this;
	d->hWnd = window()->internalWinId();
	// force native window creation
	if(!d->hWnd)
		d->hWnd = winId();

	//connect(this, SIGNAL(aboutToShow()), this, SLOT(_q_aboutToShow()));
	//connect(this, SIGNAL(aboutToHide()), this, SLOT(_q_aboutToHide()));
}

FileContextMenu::FileContextMenu(const QString& title, QWidget* parent) : QMenu(title, parent),
	d(new FileContextMenuPrivate)
{
	d->q = this;
	d->hWnd = window()->internalWinId();
	// force native window creation
	if(!d->hWnd)
		d->hWnd = winId();
}

FileContextMenu::~FileContextMenu()
{
	delete d;
}


EvilWidget::EvilWidget(QWidget* parent) : QWidget(parent),
	qmenu(0), action(0), drawn(0)
{
	// force native window creation
	winId();
	setAttribute(Qt::WA_NoSystemBackground);
}

void EvilWidget::paintEvent(QPaintEvent* event)
{
	if(!drawn)
	{
		//drawn = true;
		qWarning() << "EvilWidget::paintEvent" << this;
		qmenu->d->paintNativeMenuItem(this);
	}

	/*QPainter p(this);
	p.save();
	p.setOpacity(0.0);
	//p.drawPixmap(0, 0, action->pixmap);
	p.fillRect(rect(), Qt::white);
	p.restore();*/
	//hide();
}

bool FileContextMenuPrivate::paintNativeMenuItem(EvilWidget* w)
{
	FileContextMenuAction* action = w->action;

	if(!action || !action->d->ownerDraw)// || !action->pixmap.isNull())
		return false;

	HMENU menu = action->d->menu;
	FileContextMenu* qmenu = w->qmenu;

	HWND hWnd = w->winId();
	QRect r = w->rect();//.adjusted(1, 1, -1, -1);

	qmenu->d->registerHook(hWnd);

	//HookWndProc(hWnd, WM_INITMENU, (WPARAM)menu, 0);

	//HookWndProc(hWnd, WM_INITMENUPOPUP, (WPARAM)menu, 0);


	/*RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	qWarning() << rcClient.left << rcClient.top << rcClient.right << rcClient.bottom;*/

//	r = QRect(0, 0, mi.itemWidth, mi.itemHeight);
//r.setWidth(r.width() * 3);
//r.setWidth(q->width());

	/*RECT itemRect;
	bool ok = GetMenuItemRect(hWnd, menu, index, &itemRect);
	if(ok)
	{
		r = QRect(QPoint(itemRect.left, itemRect.top), QPoint(itemRect.right, itemRect.bottom));
		r.translate(itemRect.left, itemRect.top);
	}
qWarning() << ok << r;
r = QRect(0, 0, 300, 50);

	widget->setFixedSize(r.size());*/


		/*MEASUREITEMSTRUCT mi = { 0 };

		mi.CtlType = ODT_MENU;
		mi.CtlID = 0; // not used for menus
		mi.itemID = action->d->itemData;
		mi.itemWidth = r.width();
		mi.itemHeight = r.height();
		mi.itemData = action->d->itemUserData;

		HookWndProc(hWnd, WM_MEASUREITEM, 0, (LPARAM)&mi);*/


	HDC hdc = GetDC(hWnd);

	// Create a compatible DC which is used in a BitBlt from the window DC
	//HDC hMemDC = CreateCompatibleDC(hdc);

	// Create a compatible bitmap from the Window DC
	//HBITMAP hBitmap = CreateCompatibleBitmap(hdc, r.width(), r.height());

	/*void* lpBits;
	// Create the DIB section with an alpha channel
	BITMAPV5HEADER bh5 = { 0 };
	bh5.bV5Size = sizeof(BITMAPV5HEADER);

	bh5.bV5Width = r.width();
	bh5.bV5Height = r.height();
	bh5.bV5Planes = 1;
	bh5.bV5BitCount = 32;
	bh5.bV5Compression = BI_BITFIELDS;
	// The following mask specification specifies a supported 32 BPP
	// alpha format for Windows XP
	bh5.bV5RedMask = 0x00FF0000;
	bh5.bV5GreenMask = 0x0000FF00;
	bh5.bV5BlueMask = 0x000000FF;
	bh5.bV5AlphaMask = 0xFF000000;

	HBITMAP hBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bh5, DIB_RGB_COLORS,
										(void**)&lpBits, 0, (DWORD)0);*/
/*	if(!hBitmap)
	{
		//widget->deleteLater();
		qWarning() << "!!! CreateCompatibleBitmap has failed";
		return false;
	}
	else*/
	{
		//HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);


		DRAWITEMSTRUCT di = { 0 };

		di.CtlType = ODT_MENU;
		di.CtlID = 0; // not used for menus
		di.itemID = action->d->itemData;
		di.itemAction = ODA_DRAWENTIRE;
		di.itemState = ODS_NOACCEL | ODS_NOFOCUSRECT;
		di.hwndItem = (HWND)menu;
		di.hDC = hdc;
		di.rcItem.left = r.left();
		di.rcItem.top = r.top();
		di.rcItem.right = r.right();
		di.rcItem.bottom = r.bottom();
		di.itemData = action->d->itemUserData;

		HookWndProc(hWnd, WM_DRAWITEM, 0, (LPARAM)&di);


		//if(!BitBlt(hMemDC, 0, 0, r.width(), r.height(), hdc, r.x(), r.y(), SRCCOPY))
		//	qWarning() << "!!! BitBlt has failed";

		//SelectObject(hMemDC, hOldBitmap);

		//action->d->pixmap = QPixmap::fromWinHBITMAP(hBitmap, QPixmap::Alpha);
//action->setIcon(QIcon(action->d->pixmap));
		/*QLabel* label = new QLabel(q);
		label->setPixmap(action->d->pixmap);
		label->setFixedSize(action->d->pixmap.size());
		action->setDefaultWidget(label);*/
//action->d->pixmap.save(QString("E:/opt/PantherCommander/1/img%1.png").arg(action->d->itemData));

		//DeleteObject(hBitmap);
	}

	//DeleteDC(hMemDC);
	ReleaseDC(hWnd, hdc);


	//HookWndProc(hWnd, WM_UNINITMENUPOPUP, (WPARAM)menu, 0);

	qmenu->d->unregisterHook(hWnd);

	return true;
}

void FileContextMenuPrivate::parseNativeMenu(HMENU menu, QMenu* qmenu)
{
	HookWndProc(hWnd, WM_INITMENUPOPUP, (WPARAM)menu, 0);

	int itemCount = GetMenuItemCount(menu);
	for(int index = 0; index < itemCount; ++index)
	{
		FileContextMenuAction* action = new FileContextMenuAction(qmenu);
		action->d->menu = menu;


{
	QVarLengthArray<wchar_t, 64> buf(63);

	MENUITEMINFO info = { 0 };
	info.cbSize = sizeof(MENUITEMINFO);

	info.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_DATA | MIIM_CHECKMARKS | MIIM_SUBMENU;
	info.cch = buf.size();
	info.dwTypeData = buf.data();

	if(!GetMenuItemInfo(menu, index, true, &info))
		qWarning() << "!!!!!!!";

	if(info.fMask & (MIIM_FTYPE | MIIM_TYPE))
	{
		if(info.fType & MFT_RADIOCHECK)
		{
			action->setCheckable(true);

			// !msdn: if the MFT_RADIOCHECK type value is specified,
			// the default bitmap is a bullet; otherwise, it is a check mark.
			if(!info.hbmpChecked)
			{
				// ### TODO
				//QActionGroup* ag = new QActionGroup(qmenu);
				//ag->setExclusive(true);
				//action->setActionGroup(ag);
			}
		}
	}

	if(info.fMask & MIIM_STATE)
	{
		action->setChecked(info.fState & MFS_CHECKED);
		action->setDisabled(info.fState & MFS_DISABLED);
		if(info.fState & MFS_DEFAULT)
			qmenu->setDefaultAction(action);
		if(info.fState & MFS_HILITE)
			qmenu->setActiveAction(action);
	}

	if(info.fMask & MIIM_ID)
	{
		action->d->itemData = info.wID;
	}

	if(info.fMask & MIIM_DATA)
	{
		action->d->itemUserData = info.dwItemData;
	}

	if(info.fMask & MIIM_CHECKMARKS)
	{
		if(info.hbmpUnchecked || info.hbmpChecked)
		{
			HBITMAP bmp = info.hbmpChecked ? info.hbmpChecked : info.hbmpUnchecked;
			QIcon icon(QPixmap::fromWinHBITMAP(bmp, QPixmap::Alpha));
			action->setIcon(icon);
		}
	}

	if(info.fMask & MIIM_SUBMENU)
	{
		if(info.hSubMenu)
		{
			QMenu* subMenu = new QMenu(qmenu);
			action->setMenu(subMenu);

			parseNativeMenu(info.hSubMenu, subMenu);
		}
	}

	if(info.fType == MFT_SEPARATOR)
	{
		action->setSeparator(true);
	}

	if(info.fType == MFT_BITMAP)
	{
		if(info.hbmpItem == HBMMENU_CALLBACK)
		{
			info.fType = MFT_OWNERDRAW;
		}
		else if(info.hbmpItem)
		{
			action->d->pixmap = QPixmap::fromWinHBITMAP(info.hbmpItem, QPixmap::Alpha);
//action->setIcon(QIcon(action->pixmap));
			QLabel* label = new QLabel(qmenu);
			label->setPixmap(action->d->pixmap);
			label->setFixedSize(action->d->pixmap.size());
			action->setDefaultWidget(label);
		}
	}

qWarning() << "\n     "
		<< "info.fMask:" << info.fMask
		<< "info.fType:" << info.fType
		<< "info.fState:" << info.fState
		<< "info.hbmpChecked:" << info.hbmpChecked
		<< "info.hbmpUnchecked:" << info.hbmpUnchecked
		<< "info.wID:" << info.wID
		<< "info.dwItemData:" << info.dwItemData
		<< "info.dwTypeData:" << info.dwTypeData
		<< "info.cch:" << info.cch
		<< "info.hbmpItem:" << info.hbmpItem
		<< "\n"
		<< QString::fromWCharArray(buf.data(), info.cch);

	if(info.fType == MFT_OWNERDRAW)
	{
		action->d->ownerDraw = true;

		QRect r(0, 0, 0, 16);


		MEASUREITEMSTRUCT mi = { 0 };

		mi.CtlType = ODT_MENU;
		mi.CtlID = 0; // not used for menus
		mi.itemID = action->d->itemData;
		mi.itemWidth = r.width();
		mi.itemHeight = r.height();
		mi.itemData = action->d->itemUserData;

		HookWndProc(hWnd, WM_MEASUREITEM, 0, (LPARAM)&mi);

		r = QRect(0, 0, mi.itemWidth, mi.itemHeight);

		EvilWidget* widget = new EvilWidget();
		widget->qmenu = q;
		widget->action = action;
		widget->setMinimumSize(r.size());
		widget->resize(r.size());
		action->setDefaultWidget(widget);
	}

	int cch = info.cch + 1;
	if(cch > 1 && ((info.dwTypeData && info.dwTypeData != buf.data()) || cch >= buf.size()))
	{
		if(cch >= buf.size())
			buf.resize(cch);

		info.fMask = MIIM_STRING | MIIM_FTYPE;
		info.cch = cch;
		info.dwTypeData = buf.data();

		if(!GetMenuItemInfo(menu, index, true, &info))
			qWarning() << "!!!!!!!";

qWarning() << "***"
		<< "info.fMask:" << info.fMask
		<< "info.fType:" << info.fType
		<< "info.fState:" << info.fState
		<< "info.hbmpChecked:" << info.hbmpChecked
		<< "info.hbmpUnchecked:" << info.hbmpUnchecked
		<< "info.wID:" << info.wID
		<< "info.dwItemData:" << info.dwItemData
		<< "info.dwTypeData:" << info.dwTypeData
		<< "info.cch:" << info.cch
		<< "info.hbmpItem:" << info.hbmpItem
		<< "\n"
		<< QString::fromWCharArray(buf.data(), info.cch);
	}

	if(info.fType == MIIM_STRING || info.dwTypeData == buf.data())
		action->setText(QString::fromWCharArray(buf.data(), info.cch));
}


		qmenu->addAction(action);
		q->connect(action, SIGNAL(triggered(bool)), q, SLOT(_q_nativeActionTriggered()));

		//pContextMenu->GetCommandString(UINT_PTR(0), GCS_VERBW, 0, buf.data(), buf.size());
		//pContextMenu->GetCommandString(UINT_PTR(0), GCS_HELPTEXTW, 0, buf.data(), buf.size());
	}

	HookWndProc(hWnd, WM_UNINITMENUPOPUP, (WPARAM)menu, 0);
}

QAction* FileContextMenu::executeNativeMenu(const QPoint& pos)
{
	QAction* ret = 0;

	d->registerHook(d->hWnd);

	QPointer<QObject> guard = this;
	int idCommand = TrackPopupMenuEx(d->menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
										pos.x(), pos.y(), d->hWnd, 0);
	if(guard.isNull())
		return 0;

	d->unregisterHook(d->hWnd);

	if(idCommand >= MIN_ID && idCommand <= MAX_ID)
	{
		foreach(QAction* act, actions())
		{
			FileContextMenuAction* action = qobject_cast<FileContextMenuAction*>(act);
			if(action && action->d->itemData == idCommand)
			{
				act->trigger();
				ret = act;
			}
		}
	}

	return ret;
}

void FileContextMenu::moveEvent(QMoveEvent* event)
{
	d->pos = isVisible() ? event->pos() : QPoint();

	QMenu::moveEvent(event);
}

void FileContextMenu::paintEvent(QPaintEvent* event)
{
/*	bool changed = false;
	QList<QAction*> actionList = actions();
	for(int i = 0; i < actionList.size(); ++i)
	{
		FileContextMenuAction* action = qobject_cast<FileContextMenuAction*>(actionList.at(i));
		changed |= d->paintNativeMenuItem(d->menu, i, action);
	}
	if(changed)
		return;
*/

	QMenu::paintEvent(event);


	QPainter p(this);

	//draw the items that need updating..
	const QList<QAction*>& actionList = actions();
	for(int i = 0; i < actionList.size(); ++i)
	{
		QAction* act = actionList.at(i);
		QRect adjustedActionRect = actionGeometry(act);
		FileContextMenuAction* action = qobject_cast<FileContextMenuAction*>(act);
		if(event->rect().intersects(adjustedActionRect) && action)
		{
			//set the clip region to be extra safe (and adjust for the scrollers)
			QRegion adjustedActionReg(adjustedActionRect);
			p.setClipRegion(adjustedActionReg);

			QStyleOptionMenuItem opt;
			initStyleOption(&opt, action);
			opt.rect = adjustedActionRect;
			//style()->drawControl(QStyle::CE_MenuItem, &opt, &p, this);
			//p.drawPixmap(adjustedActionRect.topLeft(), action->pixmap);
		}
	}
}

void FileContextMenu::setPath(const QString& path)
{
	setPaths(QStringList(path));
}

static UINT GetPIDLSize(LPCITEMIDLIST pidl)
{
	if (!pidl) 
		return 0;
	int nSize = 0;
	LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;
	while (pidlTemp->mkid.cb)
	{
		nSize += pidlTemp->mkid.cb;
		pidlTemp = (LPITEMIDLIST) (((LPBYTE) pidlTemp) + pidlTemp->mkid.cb);
	}
	return nSize;
}

static LPITEMIDLIST CopyPIDL(LPCITEMIDLIST pidl)
{
	int cb = GetPIDLSize(pidl); // Calculate size of list.

	LPITEMIDLIST pidlRet = (LPITEMIDLIST)calloc(cb + sizeof(USHORT), sizeof(BYTE));
	if (pidlRet)
		CopyMemory(pidlRet, pidl, cb);

	return pidlRet;
}

void FileContextMenu::setPaths(const QStringList& paths1)
{
	// free all allocated datas
	if(d->psfFolder)
		d->psfFolder->Release();
	d->psfFolder = 0;
	LPITEMIDLIST* m_pidlArray = 0;

	if(paths1.isEmpty())
		return;

	QStringList paths;
	foreach(const QString& path, paths1)
		paths.append(QDir::toNativeSeparators(path));

	// get IShellFolder interface of Desktop (root of shell namespace)
	IShellFolder* psfDesktop = 0;
	SHGetDesktopFolder(&psfDesktop);	// needed to obtain full qualified pidl

	// ParseDisplayName creates a PIDL from a file system path relative to the IShellFolder interface
	// but since we use the Desktop as our interface and the Desktop is the namespace root
	// that means that it's a fully qualified PIDL, which is what we need
	LPITEMIDLIST pidl = 0;
	psfDesktop->ParseDisplayName(0, 0, (wchar_t*)paths.first().utf16(), 0, &pidl, 0);

	// now we need the parent IShellFolder interface of pidl, and the relative PIDL to that interface
	SHBindToParent(pidl, IID_IShellFolder, (void**)&d->psfFolder, 0);
qWarning() << paths << pidl;
	CoTaskMemFree(pidl);

	// now we have the IShellFolder interface to the parent folder specified in the first element in strArray
	// since we assume that all objects are in the same folder (as it's stated in the MSDN)
	// we now have the IShellFolder interface to every objects parent folder

	LPITEMIDLIST pidlItem = 0;	// relative pidl

	IShellFolder* psfFolder = 0;
	int size = paths.size();
	for(int i = 0; i < size; ++i)
	{
		psfDesktop->ParseDisplayName(0, 0, (wchar_t*)paths.at(i).utf16(), 0, &pidl, 0);

		m_pidlArray = (LPITEMIDLIST *) realloc (m_pidlArray, (i + 1) * sizeof (LPITEMIDLIST));
		// get relative pidl via SHBindToParent
		SHBindToParent(pidl, IID_IShellFolder, (void**)&psfFolder, (LPCITEMIDLIST*)&pidlItem);
		m_pidlArray[i] = CopyPIDL (pidlItem);	// copy relative pidl to pidlArray
		CoTaskMemFree(pidlItem);
		CoTaskMemFree(pidl);	// free pidl allocated by ParseDisplayName
		psfFolder->Release();
	}

	psfDesktop->Release();

	d->psfFolder->GetUIObjectOf(d->hWnd, size, (LPCITEMIDLIST*)m_pidlArray, IID_IContextMenu, 0, (void**)&d->pContextMenu);
	CoTaskMemFree(m_pidlArray);

	d->menu = CreatePopupMenu();

	d->pContextMenu->QueryContextMenu(d->menu, 0, MIN_ID, MAX_ID, CMF_NORMAL | CMF_EXPLORE | CMF_CANRENAME);

	QPixmap pixmap(style()->standardPixmap(QStyle::SP_ComputerIcon, 0, this));
	HBITMAP hBitmap = pixmap.toWinHBITMAP(QPixmap::Alpha);
	AppendMenu(d->menu, MF_BITMAP, size, (LPCTSTR)hBitmap);

	d->registerHook(d->hWnd);
	HookWndProc(d->hWnd, WM_INITMENU, (WPARAM)d->menu, 0);
	d->parseNativeMenu(d->menu, this);
	d->unregisterHook(d->hWnd);
}

#include "moc_filecontextmenu.cpp"
