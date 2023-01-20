#ifdef DOWNLOAD
#include "downloadmanager.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
//DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
//{

//}


void DownloadManager::replaceFile(QString Src, QString Dest)
{
//"/home/user/dst.txt")
if (QFile::exists(Dest))
{
    QFile::remove(Dest);
}

QFile::copy(Src, Dest);
}



DownloadManager::DownloadManager()
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

void DownloadManager::doDownload(const QUrl &url)
{

    QNetworkRequest request(url);


    QNetworkReply *reply = manager.get(request);

//#if QT_CONFIG(ssl)
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
            SLOT(sslErrors(QList<QSslError>)));
//#endif

    currentDownloads.append(reply);
}

QString DownloadManager::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

bool DownloadManager::isHttpRedirect(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void DownloadManager::execute()
{
   // QStringList args = QCoreApplication::instance()->arguments();
   // args.takeFirst();           // skip the first argument, which is the program's name
  //  if (args.isEmpty()) {
   //     printf("Qt Download example - downloads all URLs in parallel\n"
      //         "Usage: download url1 [url2... urlN]\n"
      //         "\n"
      //         "Downloads the URLs passed in the command-line to the local directory\n"
      //         "If the target file already exists, a .0, .1, .2, etc. is appended to\n"
      //         "differentiate.\n");
     //   QCoreApplication::instance()->quit();
     //   return;
   // }

  //  for (const QString &arg : qAsConst(args)) {
  //      QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());
  //      doDownload(url);
  //  }
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
        QMessageBox::information(NULL, "Notice", "Download incompleted.");
    } else {
        if (isHttpRedirect(reply)) {
            fputs("Request was redirected.\n", stderr);
        } else {
            QString filename = saveFileName(url);
            if (saveToDisk(filename, reply)) {
                printf("Download of %s succeeded (saved to %s)\n",
                       url.toEncoded().constData(), qPrintable(filename));
                QMessageBox::information(NULL, "Notice", "Download completed.");
            }
        }
    }

 //   currentDownloads.removeAll(reply);
        currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty()) {
        // all downloads finished
    //    QCoreApplication::instance()->quit();
    }
}

int DownloadManager::Upload(QString URL,QString User,QString Password,QString port,QString filePath){

    QFile *file = new QFile(filePath);
    file->open(QIODevice::ReadOnly);
    QByteArray byte_file = file->readAll();
    QFileInfo fileInfo(file->fileName());
        QString filename(fileInfo.fileName());

    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    QUrl url(URL.toLatin1() + filename.toLatin1());
    url.setPort(port.toInt());
    url.setUserName(User.toLatin1());
    url.setPassword(Password.toLatin1());

    QNetworkRequest request(url );
  //  QNetworkReply* reply = accessManager->put(request, byte_file);
accessManager->put(request, byte_file);


}
#endif

void DownloadManager::Download(QString URL){
    // QUrl url{"https://www.google.com/images/branding/googlelogo/1x/googlelogo_color_272x92dp.png"};
#if DOWNLOAD
    QUrl url{URL};
    //  URL = "\"https://www.google.com/images/branding/googlelogo/1x/googlelogo_color_272x92dp.png\"";
    // QUrl url;
    // url.setPath(URL);

    //    url.setPassword("12345");
    //   url.setUserName("qt");

    // QString URL2 = "\""+url+"\"";  //url.toString();
    // fromStdString()

        doDownload(url);

    //    manager.replaceFile("./new.exe","./S.exe");
#endif
}
