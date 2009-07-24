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

#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>

#include "pcshortcutedit.h"

PCShortcutEdit::PCShortcutEdit(QWidget *parent)
		:QLineEdit(parent)
{
	m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
	connect(this, SIGNAL(textChanged(QString)),
			this, SLOT(textChange(QString)));
}

bool PCShortcutEdit::event(QEvent *e)
{
	if ( e->type() == QEvent::KeyPress ) {
		QKeyEvent *k = static_cast<QKeyEvent*>(e);
		handleKeyEvent(k);
		return true;
	}

	if ( e->type() == QEvent::Shortcut ||
		 e->type() == QEvent::ShortcutOverride  ||
		 e->type() == QEvent::KeyRelease )
		return true;
	return QLineEdit::event(e);
}

void PCShortcutEdit::handleKeyEvent(QKeyEvent *e)
{
	int nextKey = e->key();
	if ( m_keyNum > 3 ||
		 nextKey == Qt::Key_Control ||
		 nextKey == Qt::Key_Shift ||
		 nextKey == Qt::Key_Meta ||
		 nextKey == Qt::Key_Alt )
		return;

	nextKey |= translateModifiers(e->modifiers(), e->text());
	switch (m_keyNum) {
	case 0:
		m_key[0] = nextKey;
		break;
	case 1:
		m_key[1] = nextKey;
		break;
	case 2:
		m_key[2] = nextKey;
		break;
	case 3:
		m_key[3] = nextKey;
		break;
	default:
		break;
	}
	m_keyNum++;
	QKeySequence ks(m_key[0], m_key[1], m_key[2], m_key[3]);
	setText(ks);
	e->accept();
}

int PCShortcutEdit::translateModifiers(Qt::KeyboardModifiers state,
										 const QString &text)
{
	int result = 0;
	// The shift modifier only counts when it is not used to type a symbol
	// that is only reachable using the shift key anyway
	if ((state & Qt::ShiftModifier) && (text.size() == 0
										|| !text.at(0).isPrint()
		|| text.at(0).isLetter()
		|| text.at(0).isSpace()))
		result |= Qt::SHIFT;
	if (state & Qt::ControlModifier)
		result |= Qt::CTRL;
	if (state & Qt::MetaModifier)
		result |= Qt::META;
	if (state & Qt::AltModifier)
		result |= Qt::ALT;
	return result;
}

void PCShortcutEdit::setShortcut(const QKeySequence& ks)
{
	m_keyNum = ks.count();
	for (int i = 0; i < m_keyNum; ++i) {
		m_key[i] = ks[i];
	}
	this->setText(ks);
}

QKeySequence PCShortcutEdit::shortcut()
{
	return QKeySequence::fromString(text());
}

void PCShortcutEdit::textChange(const QString& text)
{
	if (text.isEmpty())
		m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
}
