#include <qdialog.h>
#include <qpixmap.h>

class QBoxLayout;
class QPushButton;

class QP4AboutDlg : public QDialog
{
Q_OBJECT

private:
    QPushButton * btn_ok;
    QPushButton * btn_settings;

    QBoxLayout * mainLayout;
    QPixmap p4image;

public:
    QP4AboutDlg( QWidget * parent = 0, Qt::WindowFlags f = 0 );

public slots:
    void OnOk( void );
    void OnSettings( void );
};
