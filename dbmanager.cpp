#include "dbmanager.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>

DbManager::DbManager()
{

}

DbManager::DbManager(const QString& path)
{
    openSqliteDatabase(path);
}

bool DbManager::openSqliteDatabase(const QString& path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open())
    {
       qDebug() << "Error: connection with database failed";
    }
    else
    {
       qDebug() << "Database: connection ok";
    }
}

bool DbManager::getGeoInfo(const QString& country, const QString& city, GeoInfo& info)
{
    bool success = false;

    QString strSql = QString("SELECT lat, lng FROM worlddata WHERE country = \'%1\' and city_ascii = \'%2\'").arg(country).arg(city);
    QSqlQuery query;
    if (query.exec(strSql))
    {
        if (query.next())
        {
            info.lat = query.value("lat").toString();
            info.lng = query.value("lng").toString();
            success = true;
        }
    }

    qDebug() << query.lastError().text();

    return success;
}

void DbManager::getCountryList()
{
    vCountry.clear();

    QSqlQuery query("SELECT distinct country FROM worlddata");
    while (query.next())
    {
        QString country = query.value("country").toString();
        vCountry.push_back(country);
    }
}

void DbManager::getCityList(const QString& country)
{
    vCity.clear();

    QString strSql = QString("SELECT distinct city_ascii FROM worlddata where country = \'%1\'").arg(country);
    QSqlQuery query(strSql);
    if (query.exec(strSql))
    {
        while (query.next())
        {
            QString city = query.value("city_ascii").toString();
            vCity.push_back(city);
        }
    }
}
