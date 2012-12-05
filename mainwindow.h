#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "flextext.h"
#include "configuration.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int argc, char *argv[], QWidget *parent = 0);
    ~MainWindow();

    bool isInitialized();

private:
    bool mInitialized;

    // GUI Setup
    void setupMenu();
    void setupUi();
    void setUiElements();

    // GUI Elements
    QLabel *mFilename, *mPosition;
    QPushButton *mNext, *mPrevious;

    QTextEdit *mDisplay;
    QList<QTextEdit*> aTranslations;

    QString mFontFamily;
    int mFontPointSize;

    int mCurrentAnnotation;

    // GUI Interaction
    void closeEvent(QCloseEvent *event);
    bool maybeSave();

    // Data
    FlexText mFlexText;
    Configuration mConfiguation;

    bool loadConfigurationFile();

private slots:
    void open();
    void open(const QString & filename);
    void save();

    void nextAnnotation();
    void previousAnnotation();
    void saveTranslations();
    void setAnnotation(int i);

    void changeFont();
    void updateStyleSheet();

    void first();
    void last();
    void goTo();
};

#endif // MAINWINDOW_H
