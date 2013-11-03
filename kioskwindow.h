#ifndef KIOSKWINDOW_H
#define KIOSKWINDOW_H

#include <QMainWindow>
#include <QTextEdit>

namespace Ui {
class KioskWindow;
}

class KioskWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit KioskWindow(QWidget *parent = 0);
    ~KioskWindow();
    
private:
    Ui::KioskWindow *ui;
};

#endif // KIOSKWINDOW_H
