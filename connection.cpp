#include "connection.h"
#include <QSqlError>
#include <QSqlDatabase>
#include <QDebug>
Connection* Connection::instance = nullptr;

Connection::Connection() {
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("projet2A31");
    db.setUserName("saim");//inserer nom de l'utilisateur
    db.setPassword("pass123");//inserer mot de passe de cet utilisateur


}

Connection* Connection::getInstance() {
    if (instance == nullptr)
        instance = new Connection();
    return instance;
}

QSqlDatabase& Connection::getDatabase() {
    return db;
}

bool Connection::openConnection() {
    if (!db.open()) {
        //qDebug() << "Database connection failed:" << db.lastError().text();
        return false;
    }
    //qDebug() << "Database connected successfully!";
    return true;
}

void Connection::closeConnection() {
    if (db.isOpen())
        db.close();
}
