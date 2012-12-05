#include "configuration.h"

#include <QtXml>
#include <QtGui>

Configuration::Configuration()
{
}

bool Configuration::readXml(const QString & filename)
{
    mFilename = filename;
    QFile file(mFilename);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QDomDocument *document = new QDomDocument;
    if (!document->setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    QDomNodeList unchangeableList = document->elementsByTagName("unchangeable-display");
    if( unchangeableList.count() != 1 )
        return false;
    mDisplay = unchangeableList.at(0).toElement().text();
    mDisplayName = unchangeableList.at(0).toElement().attribute("label");

    QDomNodeList languageList = document->elementsByTagName("translation");
    if( languageList.count() < 1 )
        return false;

    for(int i=0; i<languageList.count(); i++)
    {
        mTranslations << languageList.at(i).toElement().text();
        mLanguageNames << languageList.at(i).toElement().attribute("label");
    }

    return true;
}

QString Configuration::filename() const
{
    return mFilename;
}

QString Configuration::display() const
{
    return mDisplay;
}

QString Configuration::displayName() const
{
    return mDisplayName;
}

QStringList* Configuration::languages()
{
    return &mTranslations;
}

QStringList* Configuration::languageNames()
{
    return &mLanguageNames;
}

bool Configuration::isValid() const
{
    return !mDisplay.isEmpty() && mTranslations.count() > 0;
}
