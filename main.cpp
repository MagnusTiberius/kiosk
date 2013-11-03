#include <QApplication>
#include <QBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QSettings>
#include <QDateTime>
#include <QInputDialog>
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
            qDebug() << settings.allKeys();
        }

    }
    Trimet tm(NULL, settings.value("trimet/appid").toString());

    // Parser for QJson
    QJson::Parser parser;
    bool ok;

    std::vector<unsigned int> stops;
    stops.push_back(10117);  // Goose Hollow Eastbound
    stops.push_back(10118);  // Goose Hollow Westbound

    // Get and parse the JSON data.
    QByteArray ba = tm.httpGet(stops, true);
    QVariantMap result = parser.parse(ba, &ok).toMap();
    if(!ok) {
        qFatal("An error occured during parsing the JSON!");
        exit(1);
    }
    result = result["resultSet"].toMap();
    qDebug() << result["queryTime"].toString();

    QVariantList arrivals = result["arrival"].toList();

    // Add a row for each of the arrivals
    QVBoxLayout * vl = new QVBoxLayout;
    foreach(QVariant arrival, arrivals)
    {
        // Convert this arrival set to a map.
        QVariantMap currArrival = arrival.toMap();

        // First, parse the time from the JSON data to determine minutes.
        QDateTime eTime = QDateTime::fromString(currArrival["estimated"].toString(), Qt::ISODate);
        QDateTime now = QDateTime::currentDateTime();

        int arrivalTimeInSec = now.secsTo(eTime);


        // Create a horizontal layout to put in the vertical layout
        QHBoxLayout * hl = new QHBoxLayout;

        // Create the sign name, estimated time of arrival, and put in horizontal layout.
        QLabel * icon = new QLabel;
        icon->setText("STuff");
        icon->setPixmap(QPixmap(":/new/train/25_railtransportation_inv.gif"));
        hl->addWidget(icon);

        QLabel * sign = new QLabel;
        sign->setText(currArrival["fullSign"].toString());
        hl->addWidget(sign);

        QLabel * est = new QLabel;
        est->setAlignment(Qt::AlignRight);
        est->setText(tm.prettyTime(arrivalTimeInSec));
        hl->addWidget(est);

        vl->addLayout(hl);
    }

    KioskWindow w;
    w.centralWidget()->setLayout(vl);

    w.show();
    
    return a.exec();
}
