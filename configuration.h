#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QStringList>

class Configuration
{
public:
    Configuration();

    bool readXml(QString filename);

    QString display() const;
    QString displayName() const;
    QStringList* languages();
    QStringList* languageNames();

    bool isValid() const;

private:
    QString mDisplay;
    QString mDisplayName;
    QStringList mTranslations;
    QStringList mLanguageNames;
};

#endif // CONFIGURATION_H
