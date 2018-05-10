#ifndef QPREFERENCEGLOBAL_H
#define QPREFERENCEGLOBAL_H
//
#include "qabstractpreferencespage.h"

//
class QLabel;

class QLineEdit;

class QCheckBox;

//
class QPreferenceGlobal : public QAbstractPreferencesPage
{
Q_OBJECT
private:
    QCheckBox *qchbUseNativeDialogs;
public:
    QPreferenceGlobal (QWidget *parent = 0);

    ~QPreferenceGlobal ();

    void saveSettings ();

    void loadSettings ();

    void setDefaults ();

//	QString preferenceGroup() const
//	{ return tr("Global"); }
    static QString preferenceGroup ()
    {
        return tr ("Global");
    }

private:
    void createControls ();

    void setLayouts ();

    void setConnects ();
};

#endif // QPREFERENCEGLOBAL_H
