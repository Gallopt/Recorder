#include "MainWindow.h"

#include <QApplication>



int main(int argc, char *argv[])
{

#ifdef QT_DEBUG
    qSetMessagePattern("[%{time hh:mm:ss.zzz}]%{file} %"
                       "{if-debug}D%{endif}%{if-warning}W%{endif}"
                       "%{if-critical}C%{endif}%{if-fatal}F%{endif}"
                       ":%{line}\r\n                                                ---->%{message}\r");
#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
