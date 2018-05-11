#pragma once

#include <QtWidgets/QLineEdit>

class ShortcutEdit : public QLineEdit
{
Q_OBJECT

public:
    ShortcutEdit (QWidget *parent = 0);

    virtual ~ShortcutEdit ()
    {
    }

    void setShortcut (const QKeySequence &ks);

    QKeySequence shortcut ();

protected:
    bool event (QEvent *e);

private:
    void handleKeyEvent (QKeyEvent *e);

    int translateModifiers (Qt::KeyboardModifiers state, const QString &text);

private:
    int m_key[4], m_keyNum;

private Q_SLOTS:

    void textChange (const QString &text);
};

