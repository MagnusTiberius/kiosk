#include "kioskwindow.h"
#include "ui_kioskwindow.h"

const QString KioskWindow::SettingsOrg = "JoshMoles";
const QString KioskWindow::AppName     = "TransitKiosk";
const QString KioskWindow::TMAppStr    = "trimet/appid";

const unsigned int KioskWindow::updateInterval = 60;   // How many seconds to update screen
const unsigned int KioskWindow::msInASec       = 1000; // The number of seconds in a millisecond

KioskWindow::KioskWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KioskWindow)
{
    ui->setupUi(this);

    // Configure the settings
    settings = new QSettings(SettingsOrg, AppName);

    //  If user hasn't specified a Trimet App Id, prompt for it.
    if(!settings->contains(TMAppStr)) {
        bool ok;

        QString appId = QInputDialog::getText(NULL, "TriMet App ID",
                                              "Please provide TriMet AppID:", QLineEdit::Normal,
                                              "",&ok);
        if(ok && ~appId.isEmpty() && appId.size() > 10 ) {
            settings->setValue(TMAppStr, appId);
            settings->sync();
        } else {
            exit(1);
        }

    }

    appId = settings->value(TMAppStr).toString();

    // Set up a new timer to refresh the arrivals every defined interval.
    timer = new QTimer(NULL);
    timer->setInterval(updateInterval*msInASec);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    timer->start();
}

KioskWindow::~KioskWindow()
{
    delete ui;
}

QVariantList KioskWindow::getArrivals()
{
    Trimet tm(NULL, appId);

    // Parser for QJson
    QJson::Parser parser;
    bool ok;

    std::vector<unsigned int> stops;
    stops.push_back(10117);  // Goose Hollow Westbound
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

    return arrivals;
}

void KioskWindow::timerTimeout()
{
    QVariantList arrivals = getArrivals();
    QVBoxLayout *vl = buildTrimetLayout(arrivals);

    qDeleteAll(centralWidget()->children());

    this->centralWidget()->setLayout(vl);


}

QVBoxLayout * KioskWindow::buildTrimetLayout(QVariantList arrivals)
{
    QVBoxLayout * vl = new QVBoxLayout;

    Trimet tm(NULL, NULL);

    // Get current time for calculating time delta.
    QDateTime now = QDateTime::currentDateTime();

    foreach(QVariant arrival, arrivals)
    {
        // Convert this arrival set to a map.
        QVariantMap currArrival = arrival.toMap();

        QString timeString = "";

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

    QHBoxLayout * hl = new QHBoxLayout;

    QLabel * lastUpdate = new QLabel;
    QString lastText = "Last updated at ";
    lastText.append(now.toString());
    lastUpdate->setText(lastText);
    lastUpdate->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    hl->addWidget(lastUpdate);

    vl->addLayout(hl);



    return vl;
}
