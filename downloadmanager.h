#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H
#ifdef DOWNLOAD
#include <QObject>

#include <QtCore>
#include <QtNetwork>

#include <cstdio>

class QSslError;

using namespace std;


class DownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;
    QVector<QNetworkReply *> currentDownloads;

public:
    DownloadManager();
    void doDownload(const QUrl &url);
    static QString saveFileName(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    static bool isHttpRedirect(QNetworkReply *reply);
    void replaceFile(QString Src, QString Dest);
        int Upload(QString URL,QString User,QString Password,QString port,QString filePath);
        void Download(QString URL);

public slots:
    void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);
};


#endif // DOWNLOADMANAGER_H
#endif
