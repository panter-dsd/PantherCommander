#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>

#include "ShortcutEdit.h"

ShortcutEdit::ShortcutEdit (QWidget *parent)
    : QLineEdit (parent)
    , key_ {0}
    , keyNumber_ {0}
{
    connect (this, &ShortcutEdit::textChanged, this, &ShortcutEdit::textChange);
}

ShortcutEdit::~ShortcutEdit ()
{
}

bool ShortcutEdit::event (QEvent *e)
{
    if (e->type () == QEvent::KeyPress) {
        QKeyEvent *k = static_cast<QKeyEvent *>(e);
        handleKeyEvent (k);
        return true;
    }

    static const auto events = {QEvent::Shortcut, QEvent::ShortcutOverride, QEvent::KeyRelease};
    if (std::find (std::begin (events), std::end (events), e->type ()) != std::end (events)) {
        return true;
    }
    return QLineEdit::event (e);
}

void ShortcutEdit::handleKeyEvent (QKeyEvent *e)
{
    int nextKey = e->key ();

    if (keyNumber_ > 3 ||
        nextKey == Qt::Key_Control ||
        nextKey == Qt::Key_Shift ||
        nextKey == Qt::Key_Meta ||
        nextKey == Qt::Key_Alt) {
        return;
    }

    nextKey |= translateModifiers (e->modifiers (), e->text ());
    key_.at (keyNumber_) = nextKey;
    keyNumber_++;
    QKeySequence ks (key_[0], key_[1], key_[2], key_[3]);
    setText (ks.toString ());
    e->accept ();
}

int ShortcutEdit::translateModifiers (Qt::KeyboardModifiers state, const QString &text)
{
    int result = 0;
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

void ShortcutEdit::setShortcut (const QKeySequence &ks)
{
    keyNumber_ = ks.count ();
    for (int i = 0; i < keyNumber_; ++i) {
        key_.at (i) = ks[i];
    }
    setText (ks.toString ());
}

QKeySequence ShortcutEdit::shortcut ()
{
    return QKeySequence::fromString (text ());
}

void ShortcutEdit::textChange (const QString &text)
{
    if (text.isEmpty ()) {
        keyNumber_ = 0;
        key_.fill (0);
    }
}

