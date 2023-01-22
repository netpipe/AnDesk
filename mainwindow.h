#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <downloadmanager.h>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCompleter>
#include "dbmanager.h"
#include "sunset.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getTimeFromSunValue(double val);

#ifndef __linux__
    int  setwallpaper(QString filePath);
#endif

    void showCurrentTime();
    void updateWallpaper();

    bool loaded=false;
    QString fileName;
    QString rot13( const QString & input,int );

    int sunrisehour;
    bool closing;

#if DOWNLOAD
    void Download(QString URL);
    DownloadManager manager;
#endif

    bool auth;

    int wtcount;
    int wtbool3;
    int wtanimated;

    QString AppDir;
    QString wtnumname;
    QString wtauthor;
    QString walltheme;
    QString wtdir;
    QString wtextension;

    int sunrisestart;

    QCompleter*     compCountry;
    QCompleter*     compCity;

private:
    void GetSunriseAndset();
    void initCountryCompleter();
    void initCityCompleter();

private slots:
    void on_wallpaperButton_clicked();
    void on_ListHourItemChanged(QListWidgetItem* item);
    void on_cmbCountry_TextChanged(const QString& text);

    void on_saveButton_clicked();
    void on_geobutton_clicked();
    void loadStyleSheet(QString sheet_name);
    void on_cmbTheme_currentIndexChanged(const QString &arg1);

    void on_listHour_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_actionExit_triggered();
    void on_exit();
    void on_show();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

    void on_cmbwalls_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    SunSet sun;

    DbManager  db;
};
#endif // MAINWINDOW_H
