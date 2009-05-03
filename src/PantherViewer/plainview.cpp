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
#include <QtGui>
#include "plainview.h"
//
PlainView::PlainView(const QString& fileName,QWidget* parent)
		:AbstractView(fileName,parent)
{
	qsSplitSymbol=QChar(0x0D);
	qsSplitSymbol+=QChar(0x0A);
	createControls();
	qtcCodec=QTextCodec::codecForLocale();
	qfFile=new QFile(fileName);
	if (!qfFile->open(QFile::ReadOnly))
		QMessageBox::critical(this,tr("Error open"),qfFile->errorString());
	stringCount=qfFile->size();
}
//
PlainView::~PlainView()
{
	qfFile->close();
	delete qfFile;
}
//
void PlainView::createControls()
{
	m_frame=new Frame(this);
	m_frame->setFrameShape(QFrame::Panel);
	m_frame->setFrameShadow(QFrame::Sunken);
	m_frame->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	qsbScroll=new QScrollArea(this);
	qsbScroll->setWidget(m_frame);
#ifndef Q_CC_MSVC
	#warning "This is only white, but if other color?"
#endif
	qsbScroll->setBackgroundRole(QPalette::Light);
	qsbScroll->verticalScrollBar()->setSingleStep(1);

	QVBoxLayout* layout=new QVBoxLayout();
	layout->addWidget(qsbScroll);
	this->setLayout(layout);
	connect(qsbScroll->verticalScrollBar(),
			SIGNAL(valueChanged(int)),
			this,
			SLOT(slotScroll()));
}
//
void PlainView::slotScroll()
{
	qfFile->seek(qsbScroll->verticalScrollBar()->value());

	QByteArray buf=qfFile->read(256);
	int pos=buf.indexOf(qsSplitSymbol,0);
	qsbScroll->verticalScrollBar()->blockSignals(true);
	if (pos>0)
		qsbScroll->verticalScrollBar()->setValue(qsbScroll->verticalScrollBar()->value()+pos);
	else
		qsbScroll->verticalScrollBar()->setValue(qsbScroll->verticalScrollBar()->value()+256);
	qsbScroll->verticalScrollBar()->blockSignals(false);
	slotReadFile();
}
//
bool PlainView::event(QEvent* event )
{
	if (event->type()==QEvent::Resize)
	{
		m_frame->resize(m_frame->width(),stringCount*m_frame->fontMetrics().height()+m_frame->fontMetrics().height());
		if (m_frame->height()<qsbScroll->height())
			m_frame->resize(m_frame->width(),qsbScroll->heightMM());
		slotReadFile();
	}
	return QWidget::event(event);
}
//
void PlainView::slotReadFile()
{
	qfFile->seek(qsbScroll->verticalScrollBar()->value());
	m_frame->qslText.clear();
	m_frame->setRect(QRect(10,qsbScroll->verticalScrollBar()->value(),0,0));
	m_frame->resize(m_frame->fontMetrics().width("W")*256,m_frame->height());
	while(m_frame->qslText.count()<qsbScroll->height()/m_frame->fontMetrics().height())
	{
		QByteArray buf=qfFile->read(256);//.simplified();
		for (int i=0; i<buf.count(); i++)
			if ((unsigned char)(buf[i])<32)
				if (!qsSplitSymbol.contains(QChar(buf[i])))
					buf[i]='*';
		int pos=buf.lastIndexOf(qsSplitSymbol);
		if (pos>0)
		{
			qfFile->seek(qfFile->pos()-buf.size()+pos+qsSplitSymbol.count());
			buf.resize(pos);
		}
		QString str=qtcCodec->toUnicode(buf);
		m_frame->qslText.append(str.split(qsSplitSymbol,QString::KeepEmptyParts));
	}
	m_frame->update();
}
//
void PlainView::setTextCodec(const QString& codecName)
{
	qtcCodec=QTextCodec::codecForName(codecName.toAscii());
	slotReadFile();
}
//
QString PlainView::textCodec()
{
	return QString(qtcCodec->name());
}
//
Frame::Frame(QWidget* parent)
		:QFrame(parent)
{
}
//
void Frame::paintEvent(QPaintEvent * ev)
{
	QPainter painter(this);
#ifndef Q_CC_MSVC
	#warning "This is only white, but if other color?"
#endif
	painter.fillRect(this->rect(),Qt::white);
	for (int i=0; i<qslText.count(); i++)
		painter.drawText(qrRect.left(),this->fontMetrics().height()*(i+1)+qrRect.top(),qslText.at(i));
	ev->accept();
}
//
