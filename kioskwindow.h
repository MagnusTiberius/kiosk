#ifndef KIOSKWINDOW_H
#define KIOSKWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QVariantList>
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

#include "trimet.h"

namespace Ui {
class KioskWindow;
}

class KioskWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit KioskWindow(QWidget *parent = 0);
    ~KioskWindow();

    QVariantList getArrivals();
    QVBoxLayout * buildTrimetLayout(QVariantList);

private slots:
    void timerTimeout();
    
private:
    static const QString SettingsOrg;
    static const QString AppName;
    static const QString TMAppStr;

    static const unsigned int updateInterval;
    static const unsigned int msInASec;

    Ui::KioskWindow *ui;
    QTimer *timer;
    QSettings *settings;
    QString appId;

};

#endif // KIOSKWINDOW_H
