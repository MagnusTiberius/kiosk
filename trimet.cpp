#include "trimet.h"

const QString Trimet::baseArrivals = "http://developer.trimet.org/ws/V1/arrivals";
const QString Trimet::locIdk  = "locIDs";
const QString Trimet::appIdk  = "appID";
const QString Trimet::jsonk   = "json";

Trimet::Trimet(QObject *parent, const QString appId) :
    QObject(parent), myAppId(appId)
{
}

QByteArray Trimet::httpGet(unsigned int stopID, bool json) {
    std::vector<unsigned int> myStops;
    myStops.push_back(stopID);

    return httpGet(myStops, json);
}

QByteArray Trimet::httpGet(std::vector<unsigned int> stopIDs, bool json) {

    // Prepare the Query URL and necessary items.
    QUrl myUrl(baseArrivals);
    myUrl.addQueryItem(appIdk, myAppId);
    if(json) {
        myUrl.addQueryItem(jsonk, "true");
    }

    // Build the list of stops and remove the final comma on the string.
    QString stops = "";
    for(std::vector<unsigned int>::iterator it = stopIDs.begin(); it != stopIDs.end(); ++it) {

        QString s = QString::number(*it);
        stops.append(s);
        stops.append(",");
    }
    stops.truncate(stops.size() - 1);
    myUrl.addQueryItem(locIdk, stops);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(myUrl);

    QNetworkReply* reply = manager->get(request);

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QByteArray ret = reply->readAll();

    qDebug() <<  myUrl.toString();

    return ret;
}

QString Trimet::prettyTime(int seconds) {

    int minLeft = seconds / 60;
    int secLeft = seconds % 60;

    if(seconds < 0) {
        minLeft = 0;
        secLeft = 0;
    } else {
        minLeft = seconds / 60;
        secLeft = seconds % 60;
    }

    QString time;
    time.sprintf("%2d:%02d", minLeft, secLeft);

    return time;
}


