#ifndef SQLDBMANAGER_H
#define SQLDBMANAGER_H

#include <QtSql>


class SqlDBManager
{

public:
    SqlDBManager();


    QSqlTableModel *model;

    QSqlError addConnection(const QString &driver, const QString &dbName, const QString &host,
                  const QString &user, const QString &passwd, int port = 5432);

    QString driver;
    QString dbName;
    QString host;
    QString user;
    QString passwd;
    int port;

};

#endif // SQLDBMANAGER_H
