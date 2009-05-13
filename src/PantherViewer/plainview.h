#ifndef PLAINVIEW_H
#define PLAINVIEW_H
/********************************************************************
* Copyright (C) PanteR
*-------------------------------------------------------------------
*
* Panther Commander is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* Panther Commander is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Panther Commander; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor,
* Boston, MA 02110-1301 USA
*-------------------------------------------------------------------
* Project:		Panther Commander
* Author:		PanteR
* Contact:	panter.dsd@gmail.com
*******************************************************************/

//
class QFile;
class QByteArray;
class QScrollArea;
class QTextCodec;
//
#include "abstractview.h"
#include <QFrame>
//
class Frame : public QFrame
{
Q_OBJECT
public:
	QStringList qslText;
	QRect			qrRect;
public:
	Frame(QWidget* parent=0);
	virtual ~Frame() {;}
	void setRect(const QRect& rect) {qrRect=rect;}
protected:
	void paintEvent(QPaintEvent * /* event */);
};
//
class PlainView : public AbstractView
{
Q_OBJECT
private:
	Frame*				m_frame;
	QScrollArea*		qsbScroll;
	QFile*					qfFile;
	QTextCodec*		qtcCodec;
	int						stringCount;
	QString				qsSplitSymbol;
public:
	PlainView(const QString& fileName,QWidget* parent=0);
	~PlainView();
	static bool isOpen(const QString& /*fileName*/) {return true;}
	void setTextCodec(const QString& codecName);
	QString textCodec();
private:
	void createControls();
protected:
	bool event ( QEvent * event );
private slots:
	void slotReadFile();
	void slotScroll();
};

#endif // PLAINVIEW_H
