#include <QApplication>
#include <QBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QSettings>
#include <QDateTime>
#include <QInputDialog>
#include <QStatusBar>
#include <QTimer>
#include <vector>

// Used to parse JSON reply
#include <qjson/parser.h>

#include "kioskwindow.h"
#include "trimet.h"

static const QString SettingsOrg = "JoshMoles";
static const QString AppName     = "TransitKiosk";
static const QString TMAppStr    = "trimet/appid";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Configure settings first
    QSettings settings(SettingsOrg, AppName);

    //  If user hasn't specified a Trimet App Id, prompt for it.
    if(!settings.contains(TMAppStr)) {
        bool ok;

        QString appId = QInputDialog::getText(NULL, "TriMet App ID",
                                              "Please provide TriMet AppID:", QLineEdit::Normal,
                                              "",&ok);
        if(ok && ~appId.isEmpty()) {
            settings.setValue(TMAppStr, appId);
            settings.sync();
        }

    }
    Trimet tm(NULL, settings.value("trimet/appid").toString());

    // Parser for QJson
    QJson::Parser parser;
    bool ok;

    std::vector<unsigned int> stops;
    stops.push_back(10117);  // Goose Hollow Eastbound
    stops.push_back(10118);  // Goose Hollow Westbound
    stops.push_back(1114);   // #6 bus to Jantzen Beach at Goose Hollow

    // Get and parse the JSON data.
    QByteArray ba = tm.httpGet(stops, true);
    QVariantMap result = parser.parse(ba, &ok).toMap();
    if(!ok) {
        qFatal("An error occured during parsing the JSON!");
        exit(1);
    }
    result = result["resultSet"].toMap();

    QVariantList arrivals = result["arrival"].toList();

    // Add a row for each of the arrivals
    QVBoxLayout * vl = new QVBoxLayout;
    foreach(QVariant arrival, arrivals)
    {
        // Convert this arrival set to a map.
        QVariantMap currArrival = arrival.toMap();

        // Get current time for calculating time delta.
        QDateTime now = QDateTime::currentDateTime();
        QString timeString;

        QString status = currArrival["status"].toString();
        int arrivalTimeInSec;

        if(status.compare("estimated") == 0) {

            // Estimated time is provided. Calculate delta and prepare string.
            QDateTime eTime = QDateTime::fromString(currArrival["estimated"].toString(), Qt::ISODate);
            arrivalTimeInSec = now.secsTo(eTime);
            timeString = tm.prettyTime(arrivalTimeInSec);

        } else if(status.compare("scheduled") == 0) {

            // Scheduled time so just provide time up to that time.
            timeString = "Scheduled - ";
            QDateTime eTime = QDateTime::fromString(currArrival["scheduled"].toString(), Qt::ISODate);
            arrivalTimeInSec = now.secsTo(eTime);
            timeString.append(tm.prettyTime(arrivalTimeInSec));
        } else if(status.compare("delayed") == 0) {
            timeString = "Delayed";
        } else if(status.compare("cancelled") == 0) {
            timeString = "Cancelled";
        } else {
            timeString = "Unknown";
        }

        // Create a horizontal layout to put in the vertical layout
        QHBoxLayout * hl = new QHBoxLayout;

        // Create the sign icon by determining which icon to use and then set it.
        QLabel * icon = new QLabel;
        icon->setFixedSize(50,50);
        icon->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        QPixmap pm;
        QString arrStr = currArrival["fullSign"].toString();


        if(arrStr.contains("MAX")) {
            pm = QPixmap(":/new/train/25_railtransportation.gif");
            if(arrStr.contains("blue",Qt::CaseInsensitive)) {
                icon->setStyleSheet("border: 5px solid blue;");
            } else if(arrStr.contains("red",Qt::CaseInsensitive)) {
                icon->setStyleSheet("border: 5px solid red;");
            }
        } else {
            pm = QPixmap(":/new/train/23_bus.gif");
        }
        icon->setPixmap(pm.scaled(50, 50, Qt::KeepAspectRatio));

        hl->addWidget(icon);

        QLabel * sign = new QLabel;
        sign->setText(currArrival["fullSign"].toString());
        hl->addWidget(sign);

        QLabel * est = new QLabel;
        est->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        est->setText(timeString);
        hl->addWidget(est);

        vl->addLayout(hl);
    }

    KioskWindow w;
    w.centralWidget()->setLayout(vl);

    w.show();

    QStatusBar *status;
    QLabel *statusLabel = new QLabel(NULL);
    statusLabel->setText("Route and arrival data provided by permission of TriMet.");
    status = w.statusBar();
    status->addPermanentWidget(statusLabel,200);
    status->show();
    
    return a.exec();
}
