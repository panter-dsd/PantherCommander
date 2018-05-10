#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>

#include "pcshortcutedit.h"

PCShortcutEdit::PCShortcutEdit (QWidget *parent)
    : QLineEdit (parent)
{
    m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
    connect (this, SIGNAL(textChanged (QString)),
             this, SLOT(textChange (QString)));
}

bool PCShortcutEdit::event (QEvent *e)
{
    if (e->type () == QEvent::KeyPress) {
        QKeyEvent *k = static_cast<QKeyEvent *>(e);
        handleKeyEvent (k);
        return true;
    }

    if (e->type () == QEvent::Shortcut ||
        e->type () == QEvent::ShortcutOverride ||
        e->type () == QEvent::KeyRelease) {
            return true;
    }
    return QLineEdit::event (e);
}

void PCShortcutEdit::handleKeyEvent (QKeyEvent *e)
{
    int nextKey = e->key ();
    if (m_keyNum > 3 ||
        nextKey == Qt::Key_Control ||
        nextKey == Qt::Key_Shift ||
        nextKey == Qt::Key_Meta ||
        nextKey == Qt::Key_Alt) {
            return;
    }

    nextKey |= translateModifiers (e->modifiers (), e->text ());
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
    QKeySequence ks (m_key[0], m_key[1], m_key[2], m_key[3]);
    setText (ks.toString ());
    e->accept ();
}

int PCShortcutEdit::translateModifiers (Qt::KeyboardModifiers state,
                                        const QString &text)
{
    int result = 0;
    // The shift modifier only counts when it is not used to type a symbol
    // that is only reachable using the shift key anyway
    if ((state & Qt::ShiftModifier) && (text.size () == 0
                                        || !text.at (0).isPrint ()
                                        || text.at (0).isLetter ()
                                        || text.at (0).isSpace ())) {
                                            result |= Qt::SHIFT;
    }
    if (state & Qt::ControlModifier) {
        result |= Qt::CTRL;
    }
    if (state & Qt::MetaModifier) {
        result |= Qt::META;
    }
    if (state & Qt::AltModifier) {
        result |= Qt::ALT;
    }
    return result;
}

void PCShortcutEdit::setShortcut (const QKeySequence &ks)
{
    m_keyNum = ks.count ();
    for (int i = 0; i < m_keyNum; ++i) {
        m_key[i] = ks[i];
    }
    this->setText (ks.toString ());
}

QKeySequence PCShortcutEdit::shortcut ()
{
    return QKeySequence::fromString (text ());
}

void PCShortcutEdit::textChange (const QString &text)
{
    if (text.isEmpty ()) {
        m_keyNum = m_key[0] = m_key[1] = m_key[2] = m_key[3] = 0;
    }
}
