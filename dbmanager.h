#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>

struct GeoInfo
{
    QString  lat;
    QString  lng;
};

struct CityInfo
{
    QString  country;
    QString  city;
};



class DbManager
{
public:
    DbManager();
    DbManager(const QString& path);

    bool getGeoInfo(const QString& country, const QString& city, GeoInfo& info);
    bool openSqliteDatabase(const QString& path);

    void getCountryList();
    void getCityList(const QString& country);

    QVector<QString>   vCountry;
    QVector<QString>   vCity;

private:
    QSqlDatabase        db;

};

#endif // DBMANAGER_H
