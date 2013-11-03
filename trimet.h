#ifndef TRIMET_H
#define TRIMET_H

#include <QObject>
#include <vector>

// Used to get network stuff
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QUrl>

class Trimet : public QObject
{


    Q_OBJECT
public:
    explicit Trimet(QObject *parent = 0, const QString ="");
    QByteArray httpGet(unsigned int, bool =true);
    QByteArray httpGet(std::vector<unsigned int>, bool =true);

    QString prettyTime(int );

    
signals:
    
public slots:

private:
    static const QString baseArrivals;
    static const QString locIdk;
    static const QString appIdk;
    static const QString jsonk;
    const QString myAppId;
    
};

#endif // TRIMET_H
