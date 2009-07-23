#include <QtCore/QDir>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QToolButton>
#include <QtGui/QAction>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QPixmap>
#include <QtGui/QFileIconProvider>
#include <QtGui/QBitmap>

#include "qtoolbuttonpreference.h"
#include "appsettings.h"
#include "pccommandsdialog.h"
#ifdef Q_WS_WIN
#  include <qt_windows.h>
#  include <private/qpixmapdata_p.h>
#endif

QToolButtonPreference::QToolButtonPreference(QWidget* parent)
		:QWidget(parent)
{
	createControls();
	setLayouts();
	setConnects();
}

void QToolButtonPreference::createControls()
{
	qlCommand = new QLabel(tr("Command"), this);
	qleCommand = new QLineEdit(this);
	qtbPCCommand = new QToolButton(this);
	qtbPCCommand->setText(QLatin1String("..."));
	qtbPCCommand->setToolButtonStyle(Qt::ToolButtonTextOnly);
	qtbCommand = new QToolButton(this);
	qtbCommand->setText(QLatin1String(">>"));
	qtbCommand->setToolButtonStyle(Qt::ToolButtonTextOnly);

	qlParams = new QLabel(tr("Parameters"), this);
	qleParams = new QLineEdit(this);

	qlWorkDir = new QLabel(tr("Work dir"), this);
	qleWorkDir = new QLineEdit(this);
	qtbWorkDir = new QToolButton(this);
	qtbWorkDir->setText(QLatin1String(">>"));
	qtbWorkDir->setToolButtonStyle(Qt::ToolButtonTextOnly);

	qlIconFile = new QLabel(tr("Icon file"), this);
	qleIconFile = new QLineEdit(this);
	qtbIconFile = new QToolButton(this);
	qtbIconFile->setText(QLatin1String(">>"));
	qtbIconFile->setToolButtonStyle(Qt::ToolButtonTextOnly);

	qlIcon = new QLabel(tr("Icon"), this);
	qlwIcons = new QListWidget(this);
	qlwIcons->setFlow(QListView::LeftToRight);
	qlwIcons->setViewMode(QListView::IconMode);
	qlwIcons->setIconSize(QSize(32, 32));
	qlwIcons->setWrapping(false);
	qlwIcons->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	qlCaption = new QLabel(tr("Caption"), this);
	qleCaption = new QLineEdit(this);
}

void QToolButtonPreference::setLayouts()
{
	QGridLayout* qglMainLayout = new QGridLayout();

	QHBoxLayout* qhblCommandLayout = new QHBoxLayout();
	qhblCommandLayout->addWidget(qleCommand);
	qhblCommandLayout->addWidget(qtbPCCommand);
	qhblCommandLayout->addWidget(qtbCommand);

	QHBoxLayout* qhblWorkDirLayout = new QHBoxLayout();
	qhblWorkDirLayout->addWidget(qleWorkDir);
	qhblWorkDirLayout->addWidget(qtbWorkDir);

	QHBoxLayout* qhblIconFileLayout = new QHBoxLayout();
	qhblIconFileLayout->addWidget(qleIconFile);
	qhblIconFileLayout->addWidget(qtbIconFile);

	qglMainLayout->addWidget(qlCommand, 0, 0);
	qglMainLayout->addLayout(qhblCommandLayout, 0, 1);
	qglMainLayout->addWidget(qlParams, 1, 0);
	qglMainLayout->addWidget(qleParams, 1, 1);
	qglMainLayout->addWidget(qlWorkDir, 2, 0);
	qglMainLayout->addLayout(qhblWorkDirLayout, 2, 1);
	qglMainLayout->addWidget(qlIconFile, 3, 0);
	qglMainLayout->addLayout(qhblIconFileLayout, 3, 1);
	qglMainLayout->addWidget(qlIcon, 4, 0);
	qglMainLayout->addWidget(qlwIcons, 4, 1);
	qglMainLayout->addWidget(qlCaption, 5, 0);
	qglMainLayout->addWidget(qleCaption, 5, 1);

	this->setLayout(qglMainLayout);
}

void QToolButtonPreference::setConnects()
{
	connect(qtbPCCommand, SIGNAL(clicked()),
			this, SLOT(slotChoosePCCommand()));
	connect(qtbCommand, SIGNAL(clicked()),
			this, SLOT(slotChooseCommandFile()));
	connect(qtbWorkDir, SIGNAL(clicked()),
			this, SLOT(slotChooseWorkDir()));
	connect(qtbIconFile, SIGNAL(clicked()),
			this, SLOT(slotChooseIconFile()));
	connect(qleIconFile, SIGNAL(textChanged(QString)),
			this, SLOT(slotGetIconList(QString)));
}

void QToolButtonPreference::slotChooseCommandFile()
{
	QString command;
	QFileDialog::Options options;
	if (!AppSettings::instance()->useNativeDialogs())
		options = QFileDialog::DontUseNativeDialog;
	command = QFileDialog::getOpenFileName(this,
										tr("Choose command file"),
										QDir(qleCommand->text()).absolutePath(),
										QString(),
										0,
										options);
	if (!command.isEmpty()) {
		qleCommand->setText(QDir::toNativeSeparators(command));
		qleWorkDir->setText(QDir::toNativeSeparators(QFileInfo(command).absolutePath()));
		qleIconFile->setText(QDir::toNativeSeparators(command));
		qleCaption->setText(QFileInfo(command).baseName());
	}
}

void QToolButtonPreference::slotChoosePCCommand()
{
	PCCommandsDialog *dialog = new PCCommandsDialog(this);
	if (dialog->exec())
		qleCommand->setText(dialog->getCurrentActionName());
	delete dialog;
}

void QToolButtonPreference::slotChooseWorkDir()
{
	QString dir;
	QFileDialog::Options options;
	if (!AppSettings::instance()->useNativeDialogs())
		options = QFileDialog::DontUseNativeDialog;
	dir = QFileDialog::getExistingDirectory(this,
										tr("Choose work dir"),
										QDir(qleWorkDir->text()).absolutePath(),
										options);
	if (!dir.isEmpty())
		qleWorkDir->setText(QDir::toNativeSeparators(dir));
}

void QToolButtonPreference::slotChooseIconFile()
{
	QString iconFileName;
	QStringList qslFilter;
	QFileDialog::Options options;
	if (!AppSettings::instance()->useNativeDialogs())
		options = QFileDialog::DontUseNativeDialog;
	iconFileName = QFileDialog::getOpenFileName(this,
										tr("Choose command file"),
										QDir(qleIconFile->text()).absolutePath(),
										QString(),
										0,
										options);
	if (!iconFileName.isEmpty())
		qleIconFile->setText(QDir::toNativeSeparators(iconFileName));
}

void QToolButtonPreference::slotGetIconList(const QString& iconFileName)
{
#ifdef Q_WS_WIN
	qlwIcons->clear();
	HICON smallIcon = 0, lageIcon = 0;
	UINT iconCount = ExtractIconEx((wchar_t*)iconFileName.utf16(), -1, 0, 0, 0);
	QListWidgetItem* item;
	for (int i = 0; i < int(iconCount); i++) {
		if (ExtractIconEx((wchar_t*)iconFileName.utf16(), i, &lageIcon, &smallIcon, 1)) {
			ICONINFO info;
			QPixmap pixmap;
			QIcon icon;
			//Lage
			GetIconInfo(lageIcon, &info);
			pixmap = QPixmap::fromWinHBITMAP(info.hbmColor, QPixmap::NoAlpha);
			pixmap.setMask(QPixmap::fromWinHBITMAP(info.hbmMask, QPixmap::NoAlpha));
			icon.addPixmap(pixmap);
			//Small
			GetIconInfo(smallIcon, &info);
			pixmap = QPixmap::fromWinHBITMAP(info.hbmColor, QPixmap::NoAlpha);
			pixmap.setMask(QPixmap::fromWinHBITMAP(info.hbmMask, QPixmap::NoAlpha));
			icon.addPixmap(pixmap);
			//
			item = new QListWidgetItem(icon, QString::number(i), qlwIcons);
			qlwIcons->addItem(item);
		}
	}
	if (qlwIcons->count() == 0) {
		item = new QListWidgetItem(QFileIconProvider().icon(QFileInfo(iconFileName)), QString::number(0), qlwIcons);
		qlwIcons->addItem(item);
	}
	qlwIcons->setCurrentItem(qlwIcons->item(0));
#endif
}

void QToolButtonPreference::setButton(const SToolBarButton& button)
{
	stbbButton = button;
	qleCommand->setText(button.qsCommand);
	qleParams->setText(button.qsParams);
	qleWorkDir->setText(button.qsWorkDir);
	qleIconFile->setText(button.qsIconFile);
	qleCaption->setText(button.qsCaption);
	qlwIcons->setCurrentItem(qlwIcons->item(button.iconNumber));
}

SToolBarButton QToolButtonPreference::getButton()
{
	if (qleCommand->text().isEmpty())
		return SToolBarButton();
	stbbButton.qsCommand = qleCommand->text();
	stbbButton.qsParams = qleParams->text();
	stbbButton.qsWorkDir = qleWorkDir->text();
	stbbButton.qsCaption = qleCaption->text();
	stbbButton.qsIconFile = qleIconFile->text();
	if (qlwIcons->count() > 0) {
		stbbButton.qiIcon = qlwIcons->currentItem()->icon();
		stbbButton.iconNumber = qlwIcons->currentIndex().row();
	}
	return stbbButton;
}

SToolBarButton QToolButtonPreference::getButton(const QString& command)
{
	SToolBarButton button;
	if (command.isEmpty())
		return button;
	button.qsCommand = QDir::toNativeSeparators(command);
	button.qsWorkDir = QDir::toNativeSeparators(QFileInfo(command).absolutePath());
	button.qsIconFile = button.qsCommand;
	button.qsCaption = QFileInfo(command).baseName();
	button.iconNumber = -1;
#ifdef Q_WS_WIN
	HICON smallIcon, lageIcon;
	UINT iconCount = ExtractIconEx((wchar_t*)button.qsIconFile.utf16(),-1,0,0,0);
	if ((iconCount > 0) && (ExtractIconEx((wchar_t*)button.qsIconFile.utf16(), 0, &lageIcon, &smallIcon, 1) > 0)) {
		ICONINFO info;
		QPixmap pixmap;
		QIcon icon;
		//Lage
		GetIconInfo(lageIcon, &info);
		pixmap = QPixmap::fromWinHBITMAP(info.hbmColor, QPixmap::NoAlpha);
		pixmap.setMask(QPixmap::fromWinHBITMAP(info.hbmMask, QPixmap::NoAlpha));
		icon.addPixmap(pixmap);
		//Small
		GetIconInfo(smallIcon, &info);
		pixmap = QPixmap::fromWinHBITMAP(info.hbmColor, QPixmap::NoAlpha);
		pixmap.setMask(QPixmap::fromWinHBITMAP(info.hbmMask, QPixmap::NoAlpha));
		icon.addPixmap(pixmap);
		//
		button.iconNumber = 0;
		button.qiIcon = icon;
	} else {
		button.iconNumber = 0;
		button.qiIcon = QFileIconProvider().icon(QFileInfo(button.qsIconFile));
	}
#endif
	return button;
}
