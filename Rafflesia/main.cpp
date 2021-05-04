#include <QtWidgets>

#include "MainWindow.h"

#include <istream>
#include <fstream>
int main(int argc, char *argv[])
{

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    MainWindow window;
    window.show();
    window.setWindowTitle(QApplication::translate("toplevel", "Top-level widget"));

    return app.exec();
}
