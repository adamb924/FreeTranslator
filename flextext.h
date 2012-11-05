#ifndef FLEXTEXT_H
#define FLEXTEXT_H

#include <QtXml>
#include <QString>

#include "configuration.h"

class FlexText
{
public:
    FlexText();

    bool readXml(QString filename);

    bool isValid() const;

    void setConfiguration(Configuration *configuration);

    void setFileChanged(bool changed);
    bool fileChanged() const;

    QString filename() const;

    QDomDocument* document();

    bool isCompatibleWithConfiguration();

    int phraseCount() const;

    QString displayAt(int i) const;
    QString translationAt(int i, QString language) const;

    void setAnnotation(QString annotation, int i, QString language);

private:
    QDomDocument *mDocument;
    QDomNodeList mPhrases;
    QString mFilename;

    QDomNode getGlossItem(int i, QString language) const;

    Configuration *pConfiguration;

    bool bFileChanged;
    bool bValid;
};

#endif // FLEXTEXT_H
