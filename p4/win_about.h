#ifndef WIN_ABOUT_H
#define WIN_ABOUT_H


#include <QBoxLayout>
#include <QDialog>
#include <QPixmap>
#include <QPushButton>


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


#endif /* WIN_ABOUT_H */
