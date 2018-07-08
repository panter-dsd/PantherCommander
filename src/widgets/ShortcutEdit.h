#pragma once

#include <QtWidgets/QLineEdit>

#include <array>

class ShortcutEdit : public QLineEdit
{
Q_OBJECT

public:
    explicit ShortcutEdit (QWidget *parent = nullptr);

    virtual ~ShortcutEdit ();

    void setShortcut (const QKeySequence &ks);

    QKeySequence shortcut ();

protected:
    virtual bool event (QEvent *e) override;

private:
    void handleKeyEvent (QKeyEvent *e);

    int translateModifiers (Qt::KeyboardModifiers state, const QString &text);

private Q_SLOTS:

    void textChange (const QString &text);

private:
    std::array<int, 4> key_;
    int keyNumber_;

};

