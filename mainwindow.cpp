#include "mainwindow.h"

#include <QtGui>
#include <QtXml>

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent)
    : QMainWindow(parent)
{
    mInitialized = true;

    if(! loadConfigurationFile() )
    {
        mInitialized = false;
        return;
    }

    setupUi();
    setupMenu();

    if( argc == 2 )
    {
        if( !mFlexText.readXml(QString(argv[1])) )
            QMessageBox::critical(0,tr("Error"),tr("%1 could not be opened.").arg(QString(argv[1])));
        setUiElements();
        setAnnotation( 0 );
    }
}

MainWindow::~MainWindow()
{

}

bool MainWindow::loadConfigurationFile()
{
    QString configurationFilename = QDir::current().absoluteFilePath("configuration.xml");
    if( QFile::exists(configurationFilename) )
    {
        if( !mConfiguation.readXml(configurationFilename) )
        {
            if( mConfiguation.display().isEmpty() )
            {
                QMessageBox::critical(0,tr("Error"),tr("There is an error with %1. There is no <unchangeable-display> tag, or there is more than one.").arg(configurationFilename));
                return false;
            }
            else if ( mConfiguation.languages()->isEmpty() )
            {
                QMessageBox::critical(0,tr("Error"),tr("There is an error with %1. There are no <translation> tags.").arg(configurationFilename));
                return false;
            }
        }
    }
    else
    {
        QMessageBox::critical(0,tr("Error"),tr("The file %1 does not appear to exist.").arg(configurationFilename));
        return false;
    }
    mFlexText.setConfiguration(&mConfiguation);
    return true;
}

void MainWindow::setUiElements()
{
    QFileInfo info(mFlexText.filename());
    mFilename->setText( info.baseName() );

    mDisplay->setEnabled(true);
    for(int i=0; i<aTranslations.count(); i++)
        aTranslations.at(i)->setEnabled(true);
}

void MainWindow::setupMenu()
{
    QMenu *file = new QMenu(tr("File"));
    file->addAction(tr("Open"),this,SLOT(open()),QKeySequence("Ctrl+O"));
    file->addAction(tr("Save"),this,SLOT(save()),QKeySequence("Ctrl+S"));
    file->addSeparator();
    file->addAction(tr("Exit"),this,SLOT(close()),QKeySequence("Ctrl+Q"));

    QMenu *goTo = new QMenu(tr("Go to"));
    goTo->addAction(tr("First"),this,SLOT(first()),QKeySequence("Ctrl+F"));
    goTo->addAction(tr("Last"),this,SLOT(last()),QKeySequence("Ctrl+L"));
    goTo->addAction(tr("Go to..."),this,SLOT(goTo()),QKeySequence("Ctrl+G"));

    QMenu *options = new QMenu(tr("Options"));
    options->addAction(tr("Font..."),this,SLOT(changeFont()));

    QMenuBar *menubar = menuBar();
    menubar->addMenu(file);
    menubar->addMenu(goTo);
    menubar->addMenu(options);
}

void MainWindow::setupUi()
{
    mFilename = new QLabel(tr("No file selected"));
    mPosition = new QLabel(tr("--"));
    mNext = new QPushButton(tr("Next"));
    mPrevious = new QPushButton(tr("Previous"));

    connect(mNext,SIGNAL(clicked()),this,SLOT(nextAnnotation()));
    connect(mPrevious,SIGNAL(clicked()),this,SLOT(previousAnnotation()));

    QHBoxLayout* infolayout = new QHBoxLayout;
    infolayout->addWidget(mFilename);
    infolayout->addStretch(1);
    infolayout->addWidget(mPosition);

    QHBoxLayout* hlayout = new QHBoxLayout;
    hlayout->addWidget(mPrevious);
    hlayout->addStretch(1);
    hlayout->addWidget(mNext);

    QVBoxLayout* vlayout = new QVBoxLayout;
    vlayout->addLayout(infolayout);
    vlayout->addLayout(hlayout);

    mDisplay = new QTextEdit();
    mDisplay->setEnabled(false);
    mDisplay->setReadOnly(true);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(mConfiguation.displayName()), 0 , 0 );
    gridLayout->addWidget(mDisplay, 0 , 1 );

    for(int i=0; i<mConfiguation.languages()->count(); i++)
    {
        aTranslations << new QTextEdit();
        aTranslations.last()->setEnabled(false);

        gridLayout->addWidget(new QLabel(mConfiguation.languageNames()->at(i)), i+1 , 0 );
        gridLayout->addWidget(aTranslations.last(), i+1 , 1 );

        connect( aTranslations.last(), SIGNAL(textChanged()), this, SLOT(setModified()) );
    }

    vlayout->addLayout(gridLayout);

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(vlayout);
    this->setCentralWidget(centralWidget);

    // not great
    mFontFamily = "Times New Roman";
    mFontPointSize = 28;

    updateStyleSheet();

    // initially disable these
    mNext->setEnabled(false);
    mPrevious->setEnabled(false);
}

void MainWindow::nextAnnotation()
{
    saveTranslations();
    setAnnotation( mCurrentAnnotation + 1 );
}

void MainWindow::previousAnnotation()
{
    saveTranslations();
    setAnnotation( mCurrentAnnotation - 1 );
}

void MainWindow::setAnnotation(int i)
{
    if( i == -1 || mFlexText.phraseCount() == 0 )
        return;

    mCurrentAnnotation = i;

    if( mCurrentAnnotation == 0 )
        mPrevious->setEnabled(false);
    else
        mPrevious->setEnabled(true);

    if( mCurrentAnnotation == ( mFlexText.phraseCount() - 1 ) )
        mNext->setEnabled(false);
    else
        mNext->setEnabled(true);

    mPosition->setText( tr("%1/%2").arg(mCurrentAnnotation+1).arg(mFlexText.phraseCount()));

    mDisplay->setText(mFlexText.displayAt(mCurrentAnnotation));
    for(int i=0; i<aTranslations.count(); i++)
        aTranslations.at(i)->setText( mFlexText.translationAt(mCurrentAnnotation, mConfiguation.languages()->at(i)) );
}

void MainWindow::saveTranslations()
{
    if( !mFlexText.isValid() )
        return;

    for(int i=0; i<mConfiguation.languages()->count(); i++)
        mFlexText.setAnnotation( aTranslations.at(i)->toPlainText(), mCurrentAnnotation, mConfiguation.languages()->at(i));
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open"),QString(),"*.flextext");
    if( filename.isEmpty() )
        return;

    if( !mFlexText.readXml(filename) )
        return;

    setUiElements();
    setAnnotation( 0 );
}

void MainWindow::save()
{
    if( mFlexText.filename().isEmpty() )
        return;

    saveTranslations();

    QString xml = mFlexText.document()->toString(4);
    QFile outfile(mFlexText.filename());
    if (!outfile.open(QIODevice::WriteOnly))
        return;
    outfile.write(xml.toUtf8());
    outfile.close();

    mFlexText.setFileChanged(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveTranslations();

    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::maybeSave()
{
    if( mFlexText.filename().length() == 0 )
        return true;
    if( !mFlexText.fileChanged() )
        return true;

    QMessageBox::StandardButton response;
    response = QMessageBox::question( this, tr("Save before closing?"), tr("Do you wish to save your work before closing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel , QMessageBox::Yes );
    if( response == QMessageBox::Cancel )
        return false;
    if ( response == QMessageBox::Yes )
        save();
    return true;
}

bool MainWindow::isInitialized()
{
    return mInitialized;
}

void MainWindow::changeFont()
{
    QFont currentFont(mFontFamily,mFontPointSize);
    bool isOk;
    QFont newFont = QFontDialog::getFont(&isOk,currentFont,this);
    if( isOk )
    {
        mFontFamily = newFont.family();
        mFontPointSize = newFont.pointSize();
        updateStyleSheet();
    }
}

void MainWindow::updateStyleSheet()
{
    QString styleSheet = QString("font: %1pt \"%2\";").arg(mFontPointSize).arg(mFontFamily);
    mDisplay->setStyleSheet(styleSheet);
    for(int i=0; i<aTranslations.count(); i++) {
        aTranslations.at(i)->setStyleSheet(styleSheet);
    }
}

void MainWindow::first()
{
    saveTranslations();
    setAnnotation( 0 );
}

void MainWindow::last()
{
    saveTranslations();
    setAnnotation( mFlexText.phraseCount() - 1 );
}

void MainWindow::goTo()
{
    bool ok;
    int i = QInputDialog::getInt(this, tr("Elan Check"),
                                 tr("Which?"), 1, 1, mFlexText.phraseCount(), 1, &ok);
    if (ok)
    {
        saveTranslations();
        setAnnotation( i-1 );
    }
}

void MainWindow::setModified()
{
    mFlexText.setFileChanged(true);
}
