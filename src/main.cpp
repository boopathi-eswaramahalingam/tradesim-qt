#include <QApplication>

#include "db/Database.h"
#include "db/Schema.h"
#include "db/StockRepository.h"
#include "ui/LoginDialog.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!Database::instance().open())
    {
        return 1;
    }

    if (!Schema::createTables())
    {
        return 1;
    }

    StockRepository::seedStocks();

    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted)
    {
        return 0;
    }

    auto user = loginDialog.getLoggedInUser();
    if (!user)
    {
        return 0;
    }

    MainWindow mainWindow(*user);
    mainWindow.show();

    return app.exec();
}