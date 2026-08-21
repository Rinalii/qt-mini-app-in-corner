#include <QApplication>
#include "mrmeow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MrMeow w;
    w.show();
    return a.exec();
}
