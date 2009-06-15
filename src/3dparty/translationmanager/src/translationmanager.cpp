#include "translationmanager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QMutex>
#include <QtCore/QPointer>
#include <QtCore/QRegExp>
#include <QtCore/QTranslator>

/*
	Makes a list of filters from space-separated text.
*/
static QStringList make_filter_list(const QString& filter)
{
	QString f(filter);
	if(f.isEmpty())
		return QStringList();

	QString sep(QLatin1Char(' '));
	if(f.indexOf(sep, 0) == -1 && f.indexOf(QLatin1Char('\n'), 0) != -1)
		sep = QLatin1Char('\n');

	return f.split(sep, QString::SkipEmptyParts);
}


typedef QList<QPointer<QTranslator> > TranslatorList;

struct TranslationsData
{
	TranslationsData()
	{
		if(!QCoreApplication::instance())
			qCritical("TranslationManager: an instance of QCoreApplication is required for proper functionality.");

		dirty = true;
		initSearchPaths();
		initNameFilters();
	}
	~TranslationsData()
	{
		qDeleteAll(translators.begin(), translators.end());
		translators.clear();
	}

	void initSearchPaths()
	{
		QStringList defaultPaths;

		if(QCoreApplication::instance())
		{
			// Application's default translations path
			QString appPathTranslations = QCoreApplication::applicationDirPath();
#ifndef Q_OS_WIN
			appPathTranslations.append(QLatin1String("/../share/")).append(qAppName());
#endif
			appPathTranslations.append(QDir::separator()).append(QLatin1String("translations"));
			defaultPaths.append(appPathTranslations);
		}

		// Qt's translations path
		defaultPaths.append(QLibraryInfo::location(QLibraryInfo::TranslationsPath));

		foreach(const QString path, defaultPaths)
		{
			QDir dir(path);
			if(dir.exists() && dir.isReadable())
			{
				// Make sure we convert from backslashes to slashes.
				searchPaths.append(dir.canonicalPath());
			}
		}
	}

	void initNameFilters()
	{
		const QString suffix("_*.qm");
		if(QCoreApplication::instance())
			nameFilters.append(qAppName() + suffix);
		nameFilters.append(QLatin1String("qt") + suffix);
	}

	void refreshLanguages()
	{
		if(!dirty)
			return;

		dirty = false;
		languages.clear();

		QString prefix;
		QString suffix;
		const QString pattern(nameFilters.first());
		int indexOfStar = pattern.indexOf(QLatin1Char('*'));
		if(indexOfStar >= 0)
		{
			prefix = pattern.left(indexOfStar);
			suffix = pattern.right(pattern.size() - indexOfStar - 1);
		}
		const QRegExp regexp(pattern, Qt::CaseInsensitive, QRegExp::Wildcard);
		const QStringList nameFilter(pattern);
		foreach(const QString path, searchPaths)
		{
			QDir dir(path);
			if(!dir.exists() || !dir.isReadable())
				continue;

			QFileInfo fileInfo;
			const QStringList fileNameList = dir.entryList(nameFilter);
			foreach(const QString fname, fileNameList)
			{
				fileInfo.setFile(fname);
				if(fileInfo.isReadable())
					continue;

				QString language = fileInfo.fileName();
				if(!regexp.exactMatch(language))
					continue;

				if(!suffix.isEmpty())
					language = language.left(language.size() - suffix.size());
				if(!prefix.isEmpty())
					language = language.right(language.size() - prefix.size());
				if(!language.isEmpty())
				{
					QLocale locale(language);
					if(locale != QLocale::c())
						language = locale.name(); // let's shift 'ru' to 'ru_RU' to enable QTranslation::load()'s fallback
					else
						language = QLatin1String("untranslated");
					if(!languages.contains(language))
						languages.append(language);
				}
			}
		}
	}

	void refreshTranslations()
	{
		qDeleteAll(translators.begin(), translators.end());
		translators.clear();

		if(currentLanguage.isEmpty())
			return;

		QList<QString> keys;
		for(int i = nameFilters.size() - 1; i >= 0; --i)
		{
			QString translationFileName = nameFilters[i];
			translationFileName.replace(QLatin1Char('*'), currentLanguage);

			foreach(const QString path, searchPaths)
			{
				const QString key = QString(nameFilters[i]).append(QLatin1Char('?')).append(path);
				if(keys.contains(key))
					continue;

				QTranslator* translator = new QTranslator(QCoreApplication::instance());
				if(translator->load(translationFileName, path))
				{
					keys.append(key);
					translators.append(translator);

					// A LanguageChange event is sent to all objects in the application
					// They need to catch it and re-translate themselves
					QCoreApplication::installTranslator(translator);
					break;
				}
				delete translator;
			}
		}
	}

	void refreshAll()
	{
		dirty = true;
		refreshLanguages();
		refreshTranslations();
	}

	QStringList searchPaths;
	QStringList nameFilters;
	TranslatorList translators;

	QStringList languages;
	QString currentLanguage;
	QString untranslatedLanguage;

	bool dirty;
};

Q_GLOBAL_STATIC(TranslationsData, trdata)

Q_GLOBAL_STATIC_WITH_ARGS(QMutex, translationMutex, (QMutex::Recursive))


QString TranslationManager::currentLanguage()
{
	QMutexLocker lock(translationMutex());
	const TranslationsData* d = trdata();

	if(!d->currentLanguage.isEmpty())
		return d->currentLanguage;

	if(!d->untranslatedLanguage.isEmpty())
		return d->untranslatedLanguage;

	const QString sysLanguage = QLocale::system().name();
	if(isLanguageAvailable(sysLanguage))
		return sysLanguage;

	return QString();
}

void TranslationManager::setCurrentLanguage(const QString& language)
{
	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	if(!isLanguageAvailable(language) || d->currentLanguage == language)
		return;

	d->currentLanguage = language;
	d->refreshTranslations();
}

QString TranslationManager::untranslatedLanguage()
{
	QMutexLocker lock(translationMutex());

	return trdata()->untranslatedLanguage;
}

void TranslationManager::setUntranslatedLanguage(const QString& language)
{
	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	if(d->untranslatedLanguage == language)
		return;

	d->untranslatedLanguage.clear();
	QLocale locale(language);
	if(locale != QLocale::c())
		d->untranslatedLanguage = locale.name();
}

QStringList TranslationManager::languages()
{
	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	d->refreshLanguages();
	QStringList languages(d->languages);
	if(!d->untranslatedLanguage.isEmpty())
		languages.prepend(d->untranslatedLanguage);

	return languages;
}

bool TranslationManager::isLanguageAvailable(const QString& language)
{
	return language.isEmpty() || languages().contains(language);
}

QStringList TranslationManager::translationPaths()
{
	QMutexLocker lock(translationMutex());

	return trdata()->searchPaths;
}

void TranslationManager::setTranslationPaths(const QStringList& paths)
{
	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	if(d->searchPaths == paths)
		return;

	d->searchPaths = paths;
	d->refreshAll();
}

void TranslationManager::addTranslationPath(const QString& path)
{
	QString canonicalPath = QDir(path).canonicalPath();
	if(canonicalPath.isEmpty())
		return;

	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	d->searchPaths.append(canonicalPath);
	d->refreshAll();
}

void TranslationManager::removeTranslationPath(const QString& path)
{
	QString canonicalPath = QDir(path).canonicalPath();
	if(canonicalPath.isEmpty())
		return;

	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	int i = d->searchPaths.lastIndexOf(canonicalPath);
	if(i >= 0)
	{
		d->searchPaths.removeAt(i);
		d->refreshAll();
	}
}

QStringList TranslationManager::nameFilters()
{
	QMutexLocker lock(translationMutex());

	return trdata()->nameFilters;
}

void TranslationManager::setNameFilter(const QString& filter)
{
	setNameFilters(make_filter_list(filter));
}

void TranslationManager::setNameFilters(const QStringList& filters)
{
	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	if(d->nameFilters == filters)
		return;

	d->nameFilters = filters;
	d->refreshAll();
}

void TranslationManager::addNameFilter(const QString& filter)
{
	if(filter.isEmpty())
		return;

	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	d->nameFilters.prepend(filter);
	d->refreshAll();
}

void TranslationManager::removeNameFilter(const QString& filter)
{
	if(filter.isEmpty())
		return;

	QMutexLocker lock(translationMutex());
	TranslationsData* d = trdata();

	int i = d->nameFilters.indexOf(filter);
	if(i >= 0)
	{
		d->nameFilters.removeAt(i);
		d->refreshAll();
	}
}

