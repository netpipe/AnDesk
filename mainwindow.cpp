#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QTime>
#include <QProcess>
#include <QTimer>
#include <QDir>
#include <QSystemTrayIcon>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QCloseEvent>
#include <QSound>
#include <QStandardItemModel>
#include <QPainter>

#ifndef __linux__
#include <Windows.h>
#endif

//todo
//animation of gifs or regular imagesets
QString pwd;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDir dir(".");


    pwd.append( dir.absolutePath() );
    QDateTime utc = QDateTime::currentDateTimeUtc();
   // qDebug() << utc.isDaylightTime();

   // AppDir = qApp->applicationDirPath();
   // QDir::setCurrent(AppDir);
   // qDebug() << "Files are saved at \'" << AppDir << "\'";

    compCity = NULL;
    compCountry = NULL;

    bool settingsexists=false;

    QFile Fout(pwd.toLatin1() +"/settings.txt");
    if(Fout.exists()) {
        settingsexists=true;
    }
    Fout.close();

    if (settingsexists) {
        QString searchString(":");
        QFile MyFile(pwd.toLatin1() +"/settings.txt");
        MyFile.open(QIODevice::ReadWrite);
        QTextStream in (&MyFile);
        QString line;
        QStringList list;
        QStringList nums;
        do {
            line = in.readLine();
            searchString=":";
            if (line.contains(searchString)) { //, Qt::CaseSensitive
                QRegExp rx("[:]");// match a comma or a space
                list = line.split(rx);
                nums.append(list.at(1).toLatin1());
            }
        } while (!line.isNull());
        ui->lattxt->setText(nums.at(0).toLatin1());
        ui->longtxt->setText(nums.at(1).toLatin1());
        ui->tztxt->setText(nums.at(2).toLatin1());
        ui->serialtxt->setText(nums.at(3).toLatin1());//email/registercode
        ui->cmbCountry->setEditText(nums.at(4).toLatin1());
        ui->cmbCity->setEditText(nums.at(5).toLatin1());
    }

    QString test =ui->serialtxt->text(); ui->serialtxt->text();
    QByteArray uncompressed_data = QByteArray::fromHex(test.toUtf8()) ;
    QString test2 = qUncompress(uncompressed_data);
    // ui->decodetxt->setText( test2.toLatin1() );

    //split line and verify serial to get email.
    QString line;
    QStringList list;
    QStringList nums;
    line =  test2.toLatin1();
    QString searchString("|");

    if (line.contains(searchString)) { //, Qt::CaseSensitive
        QRegExp rx("[|]");// match a comma or a space
        list = line.split(rx);
        qDebug() << list.at(0).toLatin1();

        if (list.at(0).toLatin1() == "testhre"){
            ui->lblregister->setText("Registered To: " + list.at(1).toLatin1());
            auth=true;
        } else{
               ui->lblregister->setText("Registered To: unregistered");
        }
    }

    GetSunriseAndset();

    connect(ui->cmbCountry,SIGNAL(editTextChanged(const QString&)), this, SLOT(on_cmbCountry_TextChanged(const QString&)));
    connect(ui->listHour, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(on_ListHourItemChanged(QListWidgetItem*)));
    connect(ui->listHour, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(on_ListHourItemChanged(QListWidgetItem*)));

    QPixmap oPixmap(32,32);
    oPixmap.load (pwd.toLatin1() +"/Resource/moon.png");

    QIcon oIcon( oPixmap );

    trayIcon = new QSystemTrayIcon(oIcon);

    QAction *quit_action = new QAction( "Exit", trayIcon );
    connect( quit_action, SIGNAL(triggered()), this, SLOT(on_exit()) );

    QAction *show_action = new QAction( "Show", trayIcon );
    connect( show_action, SIGNAL(triggered()), this, SLOT(on_show()) );

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction( quit_action );
    trayIconMenu->addAction( show_action );
    trayIcon->setContextMenu( trayIconMenu);
    trayIcon->setVisible(true);
    //trayIcon->showMessage("Test Message", "Text", QSystemTrayIcon::Information, 1000);
    //trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
//  connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
//    connect(trayIcon, &QSystemTrayIcon::activated, [this](auto reason)
//    {
//        if(reason == QSystemTrayIcon::Trigger)
//        {
//            if(isVisible())
//            {
//                hide();
//            }
//            else
//            {
//                show();
//                activateWindow();
//            }
//        }
//    });

    closing = true;


    auto exitAction = new QAction(tr("&Exit"), this);
    connect(exitAction, &QAction::triggered, [this]()
    {
        closing = true;
        close();
    });

    QFile MyFile33(pwd.toLatin1() +"/themes2.txt");
    if(!MyFile33.exists()){
        QDirIterator it2(pwd.toLatin1() +"/themes/", QStringList() << "*.tss", QDir::Files, QDirIterator::Subdirectories);
        while (it2.hasNext()){
            // // QFileInfo fileInfo(f.fileName());
            ui->cmbwalls->addItem(it2.next().toLatin1());
        }

        loaded=true;
        on_cmbwalls_activated("test");
        loaded=false;
    }

    QDirIterator it2(pwd.toLatin1() +"/themes/", QStringList() << "*.tss", QDir::Files, QDirIterator::Subdirectories);
    while (it2.hasNext()){
        // //  QFileInfo fileInfo(f.fileName());
        ui->cmbwalls->addItem(it2.next().toLatin1());
    }

    QDirIterator it(pwd.toLatin1() +"/Resource/themes/", QStringList() << "*.qss", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        //  QFileInfo fileInfo(f.fileName());
        ui->cmbTheme->addItem(it.next().toLatin1());
    }

    QString walltheme;
    QFile MyFile(pwd.toLatin1() +"/themes.txt");
    if (MyFile.exists()) {
        MyFile.open(QIODevice::ReadWrite);
        QTextStream in (&MyFile);
        QString line;
        QStringList list;
        //   QList<QString> nums;
        QStringList nums;
        QRegExp rx("[|]");
        line = in.readLine();
        QString stylesheet;

        if (line.contains("|")) {
            list = line.split(rx);
            qDebug() << "theme" <<  list.at(1).toLatin1();
            stylesheet =  list.at(1).toLatin1();
            loadStyleSheet( list.at(1).toLatin1());
            MyFile.close();
        }

        fileName=stylesheet;
        QFile file(stylesheet);

        file.open(QIODevice::Text | QIODevice::ReadOnly);
        QString content;

        while(!file.atEnd())
            content.append(file.readLine());
     }

     QFile MyFile2(pwd.toLatin1() +"/themes2.txt"); //wallpaper selection save
     if(MyFile2.exists()){
        MyFile2.open(QIODevice::ReadWrite);
        QTextStream in (&MyFile2);
        QString line2;
        QStringList list2;
        QRegExp rx("[|]");
        line2 = in.readLine();

        if (line2.contains("|")) {
            list2 = line2.split(rx);
            qDebug() << "theme" <<  list2.at(1).toLatin1();
            walltheme =  list2.at(1).toLatin1();
            //    loadStyleSheet( list.at(1).toLatin1());
            MyFile2.close();
        }
    }
    on_cmbwalls_activated("test");
    loaded=true;



 //   show();

    sunrisehour = sunrisestart; // sunset
    QString str;
    str = QString("%1:00").arg(sunrisehour); //risehour +i

    for (int i = 0; i < wtcount; i ++)
    {
        int frames = 24*i/wtcount;

        if (sunrisehour+frames < 24)
            str = QString("%1:00").arg(sunrisehour+frames); //risehour +i
        else
           str = QString("%1:00 (+24h)").arg(sunrisehour+frames-24); //risehour +i

        ui->listHour->insertItem(i, str);
    }

//    if (ui->listHour->count() > 0)
//    {
//        str = QString("%1:00 (+24h)").arg(sunrisehour);
//        ui->listHour->insertItem(ui->listHour->count(), str);
//    }

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateWallpaper);
    timer->start(1000000);

    QString dbpath = pwd.toLatin1() + "/Database/worldcities.db";
    db.openSqliteDatabase(dbpath);
    db.getCountryList();

    initCountryCompleter();

    loaded=true;
    updateWallpaper();
    clear2();
}

#ifndef __linux__ || __apple__
int MainWindow::setwallpaper(QString filePath) {
  bool ret = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)filePath.utf16(), SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
  return ret;
}
#endif

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateWallpaper()
{
    QTime ct = QTime::currentTime();

    int hour = ct.hour();

    QString filename;
int set;

int picked;
QString str;
str = QString("%1:00").arg(sunrisehour); //risehour +i

for (int i = 0; i < wtcount; i ++)
{
    int frames = 24*i/wtcount;
  //  str = QString("%1:00").arg(sunrisehour+frames); //risehour +i
    if (hour==sunrisehour+frames-24||hour==sunrisehour+frames-23||hour==sunrisehour+frames-22||hour==sunrisehour+frames-21){
      picked=i;
    qDebug()<< "picked";
    }

}

//hour
//    for (int i = 1; i < ui->listHour->count(); i ++)
//    {
//        int prevhouritem = i-1;//sunrisehour
//        int nexthouritem = i;

            if (picked!=0)
            {
                    filename +=   QString( pwd.toLatin1() + "/themes/" + wtdir + "/"+wtnumname + "%1." + wtextension ).arg(picked+1);
//qDebug() << filename << "testing";
                    QPixmap pix;
                    pix.load(filename);
                    pix.scaled(ui->lblImg->size(), Qt::KeepAspectRatio);
                    ui->lblImg->setPixmap(pix);
             //       ui->listHour->setCurrentRow(picked);
            }
   //     }
  //  }
 ui->listHour->setCurrentRow(picked);

    //hourchime  // needs to be in own loop so that it can update per hour instead of skipping
    if (ui->hchimechk->isChecked()){
        QSound::play(AppDir.toLatin1() + "/Resource/chime.mp3");
    }

    showCurrentTime();
}

QString MainWindow::getTimeFromSunValue(double val)
{
    int hh = (int)(val / 60);
    int mm = (int)val - hh * 60;

    QString str = QString("%1:%2").arg(hh).arg(mm);

    return str;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
     case QSystemTrayIcon::Trigger:
         this->show();
         qDebug() << "test123";
         break;
     case QSystemTrayIcon::DoubleClick:
         this->show();
         qDebug() << "test123";
         break;
     default:
         ;
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if(event->type() == QEvent::WindowStateChange)
        if(isMinimized())
            this->hide();
}


//void MainWindow::showMessage()
//{
//    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon();
//    trayIcon->showMessage(tr("QGiffer"), tr("updating background..."), icon, 100);
//}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(closing)
    {
        event->accept();
    }
    else
    {
        this->hide();
        event->ignore();
    }
}

void MainWindow::on_wallpaperButton_clicked()
{
    int row = ui->listHour->currentRow();
    if (row == -1)
        return;

    QString filename;

    #ifndef __linux__
   filename =  pwd.toLatin1()+ "/themes/" + wtdir.toLatin1() + "/"+ wtnumname.toLatin1() + QString::number(wtcount-1-row+1) +"." + wtextension;
   // qDebug() << "set" << row << " test " << filename;
     //   filename = QString("C:\\Wallpaper\\mojave_dynamic_%1").arg(row + 1);
        setwallpaper(filename);
    //    setwallpaper("C:\\Wallpaper\\mojave_dynamic_1.jpeg");
    #elif __apple__
        QString Test;
        Test +=  "wallpaper ";
        Test +=   QString("\\\"" + pwd.toLatin1() + "/mojave_dynamic/mojave_dynamic_%1.jpeg\\\"").arg(wtcount-1-row + 1);
        qDebug() << Test.toLatin1();
        QProcess::execute("bash", QStringList() << "-c" << Test.toLatin1() );
    #else
        QString Test;
        Test +=  "dconf write /org/mate/desktop/background/picture-filename ";
//if (1){
    if (!ui->sunimatechk->isChecked()){
                if (wtcount-1-row +1  < wtcount){
            Test +=   QString("\\\"" +pwd.toLatin1() + "/themes/" + wtdir.toLatin1() + "/"+ wtnumname.toLatin1() + "%1."+ wtextension +"\\\"").arg(wtcount-1-row + 1);
        }else
        {
            Test +=   QString("\\\"" +pwd.toLatin1() + "/themes/" + wtdir.toLatin1() + "/"+ wtnumname.toLatin1() + "%1." + wtextension +"\\\"").arg(wtcount);
        }
}else{
        if (wtcount-1-row + 1 < wtcount){
            Test +=   QString("\\\"" +pwd.toLatin1() + "/" + "test.png" + "\\\"").arg(wtcount-1-row + 1);
        }else
        {
            Test +=   QString("\\\"" +pwd.toLatin1() + "/" + "test.png" + "\\\"").arg(wtcount);
        }
 }
        qDebug() << Test.toLatin1();
        QProcess::execute("bash", QStringList() << "-c" << Test.toLatin1() );
    #endif

}

void MainWindow::on_ListHourItemChanged(QListWidgetItem* item)
{
    int row = ui->listHour->currentRow();
    if (row == -1)
        return;

    QString filename;

    filename +=   QString(pwd.toLatin1() + "/themes/" + wtdir.toLatin1() + "/"+ wtnumname.toLatin1() +"%1."+wtextension).arg(15-row + 1);

    QPixmap pix;
    pix.load(filename);
    pix.scaled(ui->lblImg->size(), Qt::KeepAspectRatio);
    if (ui->sunimatechk->isChecked()){
    QPixmap oPixmap(32,32);
    oPixmap.load (pwd.toLatin1() +"/Resource/moon.png");
oPixmap.scaled(10, Qt::KeepAspectRatio);
   QRect topPortion = QRect(QPoint(0, 0), QSize(width()/10, (height()/10)*1));
    QPainter painter(&pix);

    //https://itecnote.com/tecnote/qt-how-to-add-an-image-on-the-top-of-another-image/
//    QPixmap result(pix.width(), pix.height());
//    result.fill(Qt::transparent); // force alpha channel
//    {
//        QPainter painter(&result);
//        painter.drawPixmap(0, 0, pix);
//        painter.drawPixmap(1, 1, oPixmap);
//    }

        painter.drawPixmap(topPortion, oPixmap.scaled(topPortion.size(), Qt::KeepAspectRatio, Qt::FastTransformation));//Issue
   // painter.drawPixmap(10, 10, oPixmap); // moon position
pix.save("./test.png");
    }

    ui->lblImg->setPixmap(pix);
}

void MainWindow::on_saveButton_clicked()
{
    QFile file(pwd.toLatin1() +"/settings.txt");

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.seek(0);
        QTextStream stream(&file);
        stream << "lat:" << ui->lattxt->text().toLatin1() << endl;
        stream << "long:" << ui->longtxt->text().toLatin1()<< endl;
        stream << "tz:" << ui->tztxt->text().toLatin1()<< endl;
        stream << "serial:" << ui->serialtxt->text().toLatin1()<< endl;
        stream << "country:" << ui->cmbCountry->currentText().toLatin1() << endl;
        stream << "city:" << ui->cmbCity->currentText().toLatin1() << endl;
        file.close();
    }

    qDebug() << rot13(ui->serialtxt->text(),12) ;

    if (rot13(ui->serialtxt->text(),12) == "test") {
        qDebug() << "yes";
    }

    GetSunriseAndset();
}

void MainWindow::loadStyleSheet( QString sheet_name)
{
    QFile file(sheet_name);
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    qApp->setStyleSheet(styleSheet);
}

void MainWindow::on_cmbTheme_currentIndexChanged(const QString &arg1)
{
    if (loaded==true)
    {
        fileName=ui->cmbTheme->currentText();
        QFile file(fileName);

        file.open(QIODevice::Text | QIODevice::ReadOnly);
        QString content;
        while(!file.atEnd())
            content.append(file.readLine());
        file.close();

        loadStyleSheet(ui->cmbTheme->currentText());

        QFile file2(pwd.toLatin1() +"/themes.txt");
        if(file2.open(QIODevice::ReadWrite | QIODevice::Text))// QIODevice::Append |
        {
            QTextStream stream(&file2);
            file2.seek(0);
            stream << "theme:" << ui->cmbTheme->currentText().toLatin1()<< endl;
            for (int i = 0; i < ui->cmbTheme->count(); i++) {
                stream << "theme:" << ui->cmbTheme->itemText(i) << endl;
            }
            file2.close();
        }

        if (ui->cmbTheme->currentText().toLatin1() != ""){
          //   ui->cmbTheme->currentText().toLatin1();
        }
    }
}


QString MainWindow::rot13( const QString & input,int offset )
{//https://doc.qt.io/archives/3.3/rot-example.html
    QString r = input;
    int i = r.length();
    while( i-- ) {
        if ( r[i] >= QChar('A') && r[i] <= QChar('M') ||
             r[i] >= QChar('a') && r[i] <= QChar('m') )
            r[i] = (char)((int)QChar(r[i]).toLatin1() + offset);
        else if  ( r[i] >= QChar('N') && r[i] <= QChar('Z') ||
                   r[i] >= QChar('n') && r[i] <= QChar('z') )
            r[i] = (char)((int)QChar(r[i]).toLatin1() - offset);
    }
    return r;
}
void MainWindow::on_exit()
{
    this->close();
    QApplication::quit();
}
void MainWindow::on_show()
{
    this->show();
   // QApplication::quit();
}

void zip(QString filename , QString zip_filename)
{
   QFile infile(filename);
   QFile outfile(zip_filename);
   infile.open(QIODevice::ReadOnly);
   outfile.open(QIODevice::WriteOnly);
   QByteArray uncompressed_data = infile.readAll();
   QByteArray compressed_data = qCompress(uncompressed_data, 9);
   outfile.write(compressed_data);
   infile.close();
   outfile.close();
}

void unZip(QString zip_filename , QString filename)
{
   QFile infile(zip_filename);
   QFile outfile(filename);
   infile.open(QIODevice::ReadOnly);
   outfile.open(QIODevice::WriteOnly);
   QByteArray uncompressed_data = infile.readAll();
   QByteArray compressed_data = qUncompress(uncompressed_data);
   outfile.write(compressed_data);
   infile.close();
   outfile.close();
}

QByteArray fileChecksum(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}



void MainWindow::on_listHour_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    on_wallpaperButton_clicked();
}


void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_cmbwalls_activated(const QString &arg1)
{
    if (loaded==true)
    {
        fileName=ui->cmbwalls->currentText();
        QFile file(fileName);

        file.open(QIODevice::Text | QIODevice::ReadOnly);
        QString content;
        while(!file.atEnd())
            content.append(file.readLine());
        file.close();

       // loadStyleSheet(ui->cmbwalls->currentText());

        QFile file2(pwd.toLatin1() +"/themes2.txt");
        if(file2.open(QIODevice::ReadWrite | QIODevice::Text))// QIODevice::Append |
        {
            QTextStream stream(&file2);
            file2.seek(0);
            stream << "theme|" << ui->cmbwalls->currentText().toLatin1()<< endl;
            for (int i = 0; i < ui->cmbwalls->count(); i++)
            {
                stream << "theme|" << ui->cmbwalls->itemText(i) << endl;
            }

            file2.close();
        }

        if (ui->cmbwalls->currentText().toLatin1() != ""){
          //   ui->cmbTheme->currentText().toLatin1();
        }



    }

    QFile MyFile3(ui->cmbwalls->currentText());

    if(MyFile3.exists()) {
        MyFile3.open(QIODevice::ReadWrite);
        QTextStream in (&MyFile3);
        QString line3;
        QStringList list3;
        QRegExp rx("[|]");

        do {
            line3 = in.readLine();
            QString stylesheet3;
            if (line3.contains("|")) {
                list3 = line3.split(rx);
                //  qDebug() << "count" <<  list3.at(1).toLatin1();
                if (list3.at(0).toLatin1() == "count"){
                    wtcount =  list3.at(1).toInt();
                } else if ( list3.at(0).toLatin1() == "numname"){
                    wtnumname =  list3.at(1).toLatin1();
                } else if ( list3.at(0).toLatin1() == "direction"){
                    wtbool3 =  list3.at(1).toInt();
                } else if ( list3.at(0).toLatin1() == "animated"){
                    wtanimated =  list3.at(1).toInt();
                } else if ( list3.at(0).toLatin1() == "author"){
                    wtauthor =  list3.at(1).toLatin1();
                } else if ( list3.at(0).toLatin1() == "sunrisestart"){
                    sunrisestart =  list3.at(1).toInt();
                } else if ( list3.at(0).toLatin1() == "dir"){
                    wtdir = list3.at(1).toLatin1();
                    //  qDebug() << "count" <<  list3.at(1).toLatin1();
                } else if ( list3.at(0).toLatin1() == "extension"){
                    wtextension =  list3.at(1).toLatin1();
                }else if ( list3.at(0).toLatin1() == "delay"){
                    wtdelay =  list3.at(1).toLatin1();
                }

                MyFile3.close();
            }
        } while (!line3.isNull());
    } else {
        wtcount = 16;
        wtnumname = "";
        wtbool3 = 0;
        wtanimated = 0;
        wtauthor = "";
        sunrisestart = 3;
        wtdir = "";
        wtextension = "";
    }

    clear2();
//  while( ui->listHour->count()>0)
//  {
//      ui->listHour->takeItem(0);
//  }

}

void MainWindow::clear2(){
if (loaded){
    while( ui->listHour->count()>0)
    {
        ui->listHour->takeItem(0);
    }

//loaded=false;
    //        for (int i = 1; i < wtcount; i ++)
    //        { ui->listHour->takeItem(0);
//}

        QString str;
        str = QString("%1:00").arg(sunrisehour); //risehour +i

        for (int i = 0; i < wtcount; i ++)
        {
            int frames = 24*i/wtcount;

            if (sunrisehour+frames < 24)
                str = QString("%1:00").arg(sunrisehour+frames); //risehour +i
            else
               str = QString("%1:00 (+24h)").arg(sunrisehour+frames-24); //risehour +i

            ui->listHour->insertItem(i, str);
        }
    //    loaded=true;
}
}
void MainWindow::on_geobutton_clicked()
{
    QString country = ui->cmbCountry->currentText();
    QString city = ui->cmbCity->currentText();

    GeoInfo geo;
    if (db.getGeoInfo(country, city, geo))
    {
        ui->lattxt->setText(geo.lat);
          ui->longtxt->setText(geo.lng);
        if (geo.lng.toFloat() < 0 ) {
           float tester = geo.lng.toFloat()*-1;
            ui->longtxt->setText(QString::number(tester));
        }
    }
}

void MainWindow::initCountryCompleter()
{
    compCountry = new QCompleter(this);

    int nCount = db.vCountry.count();


    QStandardItemModel *model = new QStandardItemModel(nCount, 1, compCountry);

    for (int i = 0; i < nCount; i ++)
    {
        QModelIndex idx = model->index(i, 0);
        model->setData(idx, db.vCountry.at(i));
    }

    compCountry->setModel(model);
    compCountry->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cmbCountry->setCompleter(compCountry);
}

void MainWindow::initCityCompleter()
{
    if (compCity)
        delete compCity;

    compCity = new QCompleter(this);

    int nCount = db.vCity.count();

    QStandardItemModel *model = new QStandardItemModel(nCount, 1, compCountry);

    for (int i = 0; i < nCount; i ++)
    {
        QModelIndex idx = model->index(i, 0);
        model->setData(idx, db.vCity.at(i));
    }

    compCity->setModel(model);
    compCity->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cmbCity->setCompleter(compCity);
}

void MainWindow::on_cmbCountry_TextChanged(const QString& text)
{
    QString country = ui->cmbCountry->currentText();
    if (country.length() < 3)
        return;

    db.getCityList(country);
    initCityCompleter();
}

void MainWindow::GetSunriseAndset()
{
    auto rightnow = std::time(nullptr);
    struct tm *tad = std::localtime(&rightnow);

    sun.setPosition(ui->lattxt->text().toDouble(), ui->longtxt->text().toDouble(), ui->tztxt->text().toInt()); // lat , long , timezone offset
    sun.setCurrentDate(tad->tm_year + 1900, tad->tm_mon + 1, tad->tm_mday);

    ui->lblTodayDate->setText(QString("%1/%2/%3").arg(tad->tm_year + 1900).arg(tad->tm_mon + 1).arg(tad->tm_mday));

    double sunrise = sun.calcSunrise();
    ui->lblRiseTime->setText(getTimeFromSunValue(sunrise));

    double sunset = sun.calcSunset();
    ui->lblSetTime->setText(getTimeFromSunValue(sunset));

    // sunrisehour = (int)sunrise / 60;
    if (QString::number(sun.moonPhase())==14) {
        ui->moon->setText("full moon"); // 14 is full moon 0 is hidden .  0 - 29 .   >14 is waning < is waxing
    }
    else if (QString::number(sun.moonPhase()) > 14){
        ui->moon->setText(QString::number(sun.moonPhase())+" waning");
    }
    else {
        ui->moon->setText(QString::number(sun.moonPhase()) + " waxing");
    }
}

void MainWindow::showCurrentTime()
{
    ui->lblCurTime->show();
    QTime ct = QTime::currentTime();
    QString str = QString("Current Time: %1:%2:%3").arg(ct.hour()).arg(ct.minute()).arg(ct.second());
    ui->lblCurTime->setText(str);


    //str = QString("Current Time: %1:%2:%3").arg(utc.time().hour()).arg(utc.time().minute()).arg(utc.time().second());
    //ui->lblCurTime->setText(str);
}
