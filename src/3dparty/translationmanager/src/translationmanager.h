#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QtCore/QStringList>

#ifdef TRANSLATIONMANAGER_DLL
#	ifdef TRANSLATIONMANAGER_BUILD
#		define TRANSLATIONMANAGER_EXPORT Q_DECL_EXPORT
#	else
#		define TRANSLATIONMANAGER_EXPORT Q_DECL_IMPORT
#	endif
#endif
#ifndef TRANSLATIONMANAGER_EXPORT
#	define TRANSLATIONMANAGER_EXPORT
#endif

class TRANSLATIONMANAGER_EXPORT TranslationManager
{
public:
	static QString currentLanguage();
	static void setCurrentLanguage(const QString& language);

	static QString untranslatedLanguage();
	static void setUntranslatedLanguage(const QString& language);

	static QStringList languages();
	static bool isLanguageAvailable(const QString& language);

	static QStringList translationPaths();
	static void setTranslationPaths(const QStringList& paths);
	static void addTranslationPath(const QString& path);
	static void removeTranslationPath(const QString& path);

	static QStringList nameFilters();
	static void setNameFilter(const QString& filter);
	static void setNameFilters(const QStringList& filters);
	static void addNameFilter(const QString& filter);
	static void removeNameFilter(const QString& filter);
};

#endif // TRANSLATIONMANAGER_H

