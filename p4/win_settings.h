#include <qdialog.h>

class QPushButton;
class QLineEdit;
class QLabel;
class QBoxLayout;

class QSettingsDlg : public QDialog
{
Q_OBJECT

private:
    QPushButton * btn_base;
    QPushButton * btn_sum;
    QPushButton * btn_temp;
    QPushButton * btn_maple;
    QPushButton * btn_red;
    QPushButton * btn_ok;
    QPushButton * btn_reset;
    QPushButton * btn_cancel;
    QLineEdit * edt_base;
    QLineEdit * edt_sum;
    QLineEdit * edt_temp;
    QLineEdit * edt_maple;
    QLineEdit * edt_red;
    QLabel * lbl_base;
    QLabel * lbl_sum;
    QLabel * lbl_temp;
    QLabel * lbl_maple;
    QLabel * lbl_red;
    QBoxLayout * mainLayout;

public:
    QSettingsDlg( QWidget *, Qt::WindowFlags );
    void BrowseForExistingPathOrFile( QLineEdit *, QString, bool );

public slots:
    void OnOk( void );
    void OnCancel( void );
    void OnReset( void );
    void OnBrowseBase( void );
    void OnBrowseSum( void );
    void OnBrowseTemp( void );
    void OnBrowseMaple( void );
    void OnBrowseReduce( void );
};
