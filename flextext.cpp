#include "flextext.h"

#include <QtXml>
#include <QXmlQuery>
#include <QXmlResultItems>

#include "messagehandler.h"

FlexText::FlexText()
{
    bValid = false;
}


FlexText::ReadResult FlexText::readXml(QString filename)
{
    if( !pConfiguration->isValid() )
        return FlexText::InvalidConfiguration;

    mFilename = filename;

    bFileChanged = false;

    QFile file(mFilename);
    if (!file.open(QIODevice::ReadOnly))
        return FlexText::CannotOpenFile;
    mDocument = new QDomDocument;
    if (!mDocument->setContent(&file)) {
        file.close();
        return FlexText::CannotSetContent;
    }
    file.close();

    if( !isCompatibleWithConfiguration() )
        return FlexText::IncompatibleWithConfiguration;

    mPhrases = mDocument->elementsByTagName("phrase");

    bValid = true;
    return FlexText::Success;
}

bool FlexText::isValid() const
{
    return bValid;
}

void FlexText::setFileChanged(bool changed)
{
    bFileChanged = changed;
}

bool FlexText::fileChanged() const
{
    return bFileChanged;
}

QString FlexText::filename() const
{
    return mFilename;
}

QDomDocument* FlexText::document()
{
    return mDocument;
}

void FlexText::setConfiguration(Configuration *configuration)
{
    pConfiguration = configuration;
}

bool FlexText::isCompatibleWithConfiguration()
{
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setFocus(QUrl(mFilename));
    query.setMessageHandler(new MessageHandler(0));
    query.setQuery(QString("count(/document/interlinear-text/paragraphs/paragraph/phrases/phrase/words/word[count(item[@lang='%1']) = 0])").arg(pConfiguration->display()));
    QXmlResultItems result;
    query.evaluateTo(&result);
    QXmlItem item(result.next());
    if(!item.isNull() && item.isAtomicValue() && item.toAtomicValue().toInt() > 0 )
        return false;
    else
        return true;
}

int FlexText::phraseCount() const
{
    return mPhrases.count();
}

QString FlexText::displayAt(int i) const
{
    if( i < 0 || i >= phraseCount() )
        return "";

    QString string = "";
    QDomNodeList items = mPhrases.at(i).toElement().elementsByTagName("item");
    for(int i=0; i<items.count(); i++)
    {
        if( items.at(i).attributes().contains("lang") )
        {
            if( items.at(i).attributes().namedItem("lang").toAttr().value() == pConfiguration->display() )
            {
                string.append(items.at(i).toElement().text());
                string.append(" ");
            }
        }
    }

    return string;
}

QString FlexText::translationAt(int i, QString language) const
{
    if( i < 0 || i >= phraseCount() )
        return "";

    QDomNode gloss = getGlossItem(i, language);
    if( gloss.isNull() )
        return "";
    else
        return gloss.toElement().text();
}

QDomNode FlexText::getGlossItem(int i, QString language) const
{
    QDomNodeList glosses = mPhrases.at(i).childNodes();
    for(int i=0; i<glosses.count(); i++)
        if( glosses.at(i).isElement() && glosses.at(i).toElement().tagName() == "item" && glosses.at(i).attributes().contains("type") && glosses.at(i).attributes().contains("lang") )
            if( glosses.at(i).attributes().namedItem("type").toAttr().value() == "gls" && glosses.at(i).attributes().namedItem("lang").toAttr().value() == language )
                return glosses.at(i);
    return QDomNode();
}

void FlexText::setAnnotation(QString annotation, int i, QString language)
{
    if( i < 0 || i >= phraseCount() )
        return;

    QDomElement newItem = mDocument->createElement("item");
    QDomText newNodeText = mDocument->createTextNode(annotation);
    newItem.appendChild(newNodeText);
    newItem.setAttribute("type", "gls" );
    newItem.setAttribute("lang", language);

    QDomNode gloss = getGlossItem(i, language);
    if( gloss.isNull() )
    {
        mPhrases.at(i).appendChild(newItem);
    }
    else
    {
        if( gloss.toElement().text() != annotation )
            bFileChanged = true;
        mPhrases.at(i).replaceChild(newItem,gloss);
    }
}
