#include <QApplication>
#include "clientwnd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile hostfile("./data/host.txt");
    if (hostfile.exists()){
        hostfile.open(QFile::ReadOnly);
    }
    QString host = hostfile.readLine();
    ClientWnd wnd(host,2323);

    wnd.show();

    return a.exec();
}
