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



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KioskWindow w;

    QVariantList arrivals = w.getArrivals();
    QVBoxLayout *vl = w.buildTrimetLayout(arrivals);
    w.centralWidget()->setLayout(vl);

    w.show();

    QStatusBar *status;
    QLabel *statusLabel = new QLabel(NULL);
    statusLabel->setText("Route and arrival data provided by permission of TriMet.");
    status = w.statusBar();
    status->addPermanentWidget(statusLabel,200);
    status->show();

    // Add the QTimer

    
    return a.exec();
}
