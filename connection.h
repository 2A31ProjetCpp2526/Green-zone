#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>
#include <QDebug>

class Connection
{
private:
    static Connection *instance;
    QSqlDatabase db;
    Connection();  // Private constructor for Singleton

public:
    static Connection* getInstance();
    QSqlDatabase& getDatabase();
    bool openConnection();
    void closeConnection();
};

#endif // CONNECTION_H
