#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "MainWindow.h"

int main(int argc, char** argv) {
    QApplication a(argc, argv);

    // Load QSS stylesheet
    QFile styleFile("styles/app.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&styleFile);
        a.setStyleSheet(ts.readAll());
    }

    MainWindow w;
    w.resize(780, 520);
    w.show();

    return a.exec();
}
