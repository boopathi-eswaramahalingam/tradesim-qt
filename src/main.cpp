#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLabel label("TradeSim - Hello World");
    label.show();
    return app.exec();
}