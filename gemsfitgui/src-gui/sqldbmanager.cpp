#include "sqldbmanager.h"

SqlDBManager::SqlDBManager()
{



}

QSqlError SqlDBManager::addConnection(const QString &driver, const QString &dbName, const QString &host,
                                      const QString &user, const QString &passwd, int port )
{
    static int cCount = 0;

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase(driver, QString("Browser%1").arg(++cCount));
    db.setDatabaseName(dbName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
    }

    return err;
}
