#include <QtCore/QDir>

#include <QtGui/QBitmap>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileIconProvider>

#include "src/AppSettings.h"
#include "CommandsDialog.h"

#ifdef Q_WS_WIN
#  include <qt_windows.h>
#  include <private/qpixmapdata_p.h>
#endif

#include "ToolButtonPreference.h"

ToolButtonPreference::ToolButtonPreference (QWidget *parent)
    : QWidget (parent)
{
    commandLabel_ = new QLabel (tr ("Command"), this);
    commandEdit_ = new QLineEdit (this);
    commandButton_ = new QToolButton (this);
    commandButton_->setText (QLatin1String ("..."));
    commandButton_->setToolButtonStyle (Qt::ToolButtonTextOnly);
    commandButton2_ = new QToolButton (this);
    commandButton2_->setText (QLatin1String (">>"));
    commandButton2_->setToolButtonStyle (Qt::ToolButtonTextOnly);

    parametersLabel_ = new QLabel (tr ("Parameters"), this);
    parametersEdit_ = new QLineEdit (this);

    workDirLabel_ = new QLabel (tr ("Work dir"), this);
    workDirEdit_ = new QLineEdit (this);
    workDirButton_ = new QToolButton (this);
    workDirButton_->setText (QLatin1String (">>"));
    workDirButton_->setToolButtonStyle (Qt::ToolButtonTextOnly);

    iconFileLabel_ = new QLabel (tr ("Icon file"), this);
    iconFileEdit_ = new QLineEdit (this);
    iconFileButton_ = new QToolButton (this);
    iconFileButton_->setText (QLatin1String (">>"));
    iconFileButton_->setToolButtonStyle (Qt::ToolButtonTextOnly);

    iconLabel_ = new QLabel (tr ("Icon"), this);
    iconList_ = new QListWidget (this);
    iconList_->setFlow (QListView::LeftToRight);
    iconList_->setViewMode (QListView::IconMode);
    iconList_->setIconSize (QSize (32, 32));
    iconList_->setWrapping (false);
    iconList_->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Maximum);

    captionLabel_ = new QLabel (tr ("Caption"), this);
    captionEdit_ = new QLineEdit (this);
    QGridLayout *mainLayout = new QGridLayout ();

    QHBoxLayout *commandLayout = new QHBoxLayout ();
    commandLayout->addWidget (commandEdit_);
    commandLayout->addWidget (commandButton_);
    commandLayout->addWidget (commandButton2_);

    QHBoxLayout *workDirLayout = new QHBoxLayout ();
    workDirLayout->addWidget (workDirEdit_);
    workDirLayout->addWidget (workDirButton_);

    QHBoxLayout *iconFileLayout = new QHBoxLayout ();
    iconFileLayout->addWidget (iconFileEdit_);
    iconFileLayout->addWidget (iconFileButton_);

    mainLayout->addWidget (commandLabel_, 0, 0);
    mainLayout->addLayout (commandLayout, 0, 1);
    mainLayout->addWidget (parametersLabel_, 1, 0);
    mainLayout->addWidget (parametersEdit_, 1, 1);
    mainLayout->addWidget (workDirLabel_, 2, 0);
    mainLayout->addLayout (workDirLayout, 2, 1);
    mainLayout->addWidget (iconFileLabel_, 3, 0);
    mainLayout->addLayout (iconFileLayout, 3, 1);
    mainLayout->addWidget (iconLabel_, 4, 0);
    mainLayout->addWidget (iconList_, 4, 1);
    mainLayout->addWidget (captionLabel_, 5, 0);
    mainLayout->addWidget (captionEdit_, 5, 1);
    setLayout (mainLayout);

    connect (commandButton_, &QToolButton::clicked, this, &ToolButtonPreference::slotChoosePCCommand);
    connect (commandButton2_, &QToolButton::clicked, this, &ToolButtonPreference::slotChooseCommandFile);
    connect (workDirButton_, &QToolButton::clicked, this, &ToolButtonPreference::slotChooseWorkDir);
    connect (iconFileButton_, &QToolButton::clicked, this, &ToolButtonPreference::slotChooseIconFile);
    connect (iconFileEdit_, &QLineEdit::textChanged, this, &ToolButtonPreference::slotGetIconList);
}

ToolButtonPreference::~ToolButtonPreference ()
{

}

void ToolButtonPreference::slotChooseCommandFile ()
{
    QFileDialog::Options options;
    if (!AppSettings::instance ()->useNativeDialogs ()) {
        options = QFileDialog::DontUseNativeDialog;
    }
    const QString &command = QFileDialog::getOpenFileName (this,
                                                           tr ("Choose command file"),
                                                           QDir (commandEdit_->text ()).absolutePath (),
                                                           QString (),
                                                           0,
                                                           options
    );
    if (!command.isEmpty ()) {
        commandEdit_->setText (QDir::toNativeSeparators (command));
        workDirEdit_->setText (QDir::toNativeSeparators (QFileInfo (command).absolutePath ()));
        iconFileEdit_->setText (QDir::toNativeSeparators (command));
        captionEdit_->setText (QFileInfo (command).baseName ());
    }
}

void ToolButtonPreference::slotChoosePCCommand ()
{
    CommandsDialog dialog (this);
    if (dialog.exec ()) {
        commandEdit_->setText (dialog.getCurrentActionName ());
    }
}

void ToolButtonPreference::slotChooseWorkDir ()
{
    QFileDialog::Options options {};
    if (!AppSettings::instance ()->useNativeDialogs ()) {
        options = QFileDialog::DontUseNativeDialog;
    }
    const QString &dir = QFileDialog::getExistingDirectory (this,
                                                            tr ("Choose work dir"),
                                                            QDir (workDirEdit_->text ()).absolutePath (),
                                                            options
    );
    if (!dir.isEmpty ()) {
        workDirEdit_->setText (QDir::toNativeSeparators (dir));
    }
}

void ToolButtonPreference::slotChooseIconFile ()
{
    QFileDialog::Options options {};
    if (!AppSettings::instance ()->useNativeDialogs ()) {
        options = QFileDialog::DontUseNativeDialog;
    }
    const QString &iconFileName = QFileDialog::getOpenFileName (this,
                                                                tr ("Choose command file"),
                                                                QDir (iconFileEdit_->text ()).absolutePath (),
                                                                QString (),
                                                                0,
                                                                options
    );
    if (!iconFileName.isEmpty ()) {
        iconFileEdit_->setText (QDir::toNativeSeparators (iconFileName));
    }
}

QIcon ToolButtonPreference::getIcon (const QString &fileName, int number)
{
#ifdef Q_WS_WIN
    QIcon icon;
    HICON smallIcon, lageIcon;
    if (ExtractIconEx((wchar_t*)fileName.utf16(), number, &lageIcon, &smallIcon, 1) > 0) {
        ICONINFO info;
        QPixmap pixmap;
        //Lage
        if (GetIconInfo(lageIcon, &info)) {
            pixmap = QPixmap::fromWinHBITMAP(info.hbmColor, QPixmap::NoAlpha);
            pixmap.setMask(QPixmap::fromWinHBITMAP(info.hbmMask, QPixmap::NoAlpha));
            icon.addPixmap(pixmap);
        }
        //Small
        if (GetIconInfo(smallIcon, &info)) {
            pixmap = QPixmap::fromWinHBITMAP(info.hbmColor, QPixmap::NoAlpha);
            pixmap.setMask(QPixmap::fromWinHBITMAP(info.hbmMask, QPixmap::NoAlpha));
            icon.addPixmap(pixmap);
        }
        DestroyIcon(smallIcon);
        DestroyIcon(lageIcon);
    }
    return icon;
#else
    return QIcon (fileName);
#endif
}

void ToolButtonPreference::slotGetIconList (const QString &iconFileName)
{
#ifdef Q_WS_WIN
    iconList_->clear();
    int iconCount = (int)ExtractIconEx((wchar_t*)iconFileName.utf16(), -1, 0, 0, 0);
    QListWidgetItem* item;
    for (int i = 0; i < iconCount; i++) {
            item = new QListWidgetItem(getIcon(iconFileName, i), QString::number(i), iconList_);
            iconList_->addItem(item);
    }
    if (iconList_->count() == 0) {
        item = new QListWidgetItem(QFileIconProvider().icon(QFileInfo(iconFileName)), QString::number(0), iconList_);
        iconList_->addItem(item);
    }
    iconList_->setCurrentItem(iconList_->item(0));
#else
    iconList_->clear ();
    QListWidgetItem *item = new QListWidgetItem (getIcon (iconFileName, 0), QString::number (0), iconList_);
    iconList_->addItem (item);
    if (iconList_->count () == 0) {
        item = new QListWidgetItem (QFileIconProvider ().icon (QFileInfo (iconFileName)), QString::number (0), iconList_
        );
        iconList_->addItem (item);
    }
    iconList_->setCurrentItem (iconList_->item (0));
#endif
}

void ToolButtonPreference::setButton (const ToolBarButton &button)
{
    buttons_ = button;
    commandEdit_->setText (button.command_);
    parametersEdit_->setText (button.parameters_);
    workDirEdit_->setText (button.workDir_);
    iconFileEdit_->setText (button.iconFile_);
    captionEdit_->setText (button.caption_);
    iconList_->setCurrentItem (iconList_->item (button.iconNumber_));
}

ToolBarButton ToolButtonPreference::getButton ()
{
    if (commandEdit_->text ().isEmpty ()) {
        return ToolBarButton ();
    }
    buttons_.command_ = commandEdit_->text ();
    buttons_.parameters_ = parametersEdit_->text ();
    buttons_.workDir_ = workDirEdit_->text ();
    buttons_.caption_ = captionEdit_->text ();
    buttons_.iconFile_ = iconFileEdit_->text ();
    if (iconList_->count () > 0) {
        buttons_.icon_ = iconList_->currentItem ()->icon ();
        buttons_.iconNumber_ = iconList_->currentIndex ().row ();
    }
    return buttons_;
}

ToolBarButton ToolButtonPreference::getButton (const QString &command)
{
    ToolBarButton button {};
    if (command.isEmpty ()) {
        return button;
    }
    button.command_ = QDir::toNativeSeparators (command);
    button.workDir_ = QDir::toNativeSeparators (QFileInfo (command).absolutePath ());
    button.iconFile_ = button.command_;
    button.caption_ = QFileInfo (command).baseName ();
    button.iconNumber_ = -1;
#ifdef Q_WS_WIN
    QIcon icon = getIcon(button.iconFile_, 0);
    if (!icon.isNull()) {
        button.iconNumber_ = 0;
        button.icon_ = icon;
    } else {
        button.iconNumber_ = 0;
        button.icon_ = QFileIconProvider().icon(QFileInfo(button.iconFile_));
    }
#endif
    return button;
}
