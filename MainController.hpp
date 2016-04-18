#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <QObject>
#include <QDebug>
#include <QString>

class MainController : public QObject {
    Q_OBJECT
    
public:
    MainController();
private:

   public slots:
   void exitClicked();
   void FileLoadedSlot(int pPercentage);
   void closeMapClicked();
   void loadFile(QString pFilename);
   void loadMapSlot();
   void noNodesSlot();
   void noOSMSlot();
   void multipleDropSlot();
    
   signals:
    void exitProgram();
    void FileLoadedSignal(int pPercentage);
    void closeMap();
    void droppedFile(QString pFileName);
    void loadMapSignal();
    void noNodesSignal();
    void noOSMSignal();
    void multipleDropSignal();
};

#endif	/* MAINCONTROLLER_HPP */

