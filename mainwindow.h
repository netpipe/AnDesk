#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <downloadmanager.h>
#include <QSystemTrayIcon>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateWallpaper();
        bool loaded=false;
    QString fileName;
    QString rot13( const QString & input,int );
    void showCurrentTime();
        int sunrisehour;
         bool closing;

#if DOWNLOAD
    void Download(QString URL);
    DownloadManager manager;
#endif
    int wtcount;
    QString wtnumname;
    int wtbool3;
    int wtanimated;
    QString wtauthor;
    QString walltheme;
    QString wtdir;
    bool auth;
        QString wtextension;
void changeEvent(QEvent *event);
    int sunrisestart;
private slots:
    void on_wallpaperButton_clicked();
    void on_ListHourItemChanged(QListWidgetItem* item);

    void on_pushButton_clicked();
    void loadStyleSheet(QString sheet_name);
    void on_cmbTheme_currentIndexChanged(const QString &arg1);

    void on_listHour_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_actionExit_triggered();
    void on_exit();
    void on_show();
    void on_cmbwalls_currentIndexChanged(const QString &arg1);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
void closeEvent(QCloseEvent *event);

void on_cmbwalls_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};
#endif // MAINWINDOW_H
