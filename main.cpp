#include <QApplication>
#include <QTextCodec>

#include "WFrame.hpp"
#include "MainController.hpp"
#include "Data.hpp"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);
    
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    MainController mainC;
    Data repository;
    
    WFrame mainWindow(&mainC, &repository);
    mainWindow.show();
    
    
    return app.exec();
}
