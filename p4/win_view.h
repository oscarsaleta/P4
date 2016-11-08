#ifndef WIN_VIEW_H
#define WIN_VIEW_H


#include "win_find.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>


class QViewDlg : public QWidget
{
    Q_OBJECT

public:
    QViewDlg(QWidget *);
    ~QViewDlg();
    bool GetDataFromDlg( void );
    void UpdateDlgData( void );
    void ExclusiveToggle( bool, QRadioButton *, ... );

private:
    bool changed;
    QFindDlg * parent;
    QBoxLayout * mainLayout;

    QRadioButton * btn_sphere;
    QRadioButton * btn_plane;
    QRadioButton * btn_U1;
    QRadioButton * btn_V1;
    QRadioButton * btn_U2;
    QRadioButton * btn_V2;
    QLineEdit * edt_projection;
    QLineEdit * edt_x0;
    QLineEdit * edt_x1;
    QLineEdit * edt_y0;
    QLineEdit * edt_y1;
    QPushButton * btn_square;

    QLabel * lbl_projection;
    QLabel * lbl_x0;
    QLabel * lbl_x1;
    QLabel * lbl_y0;
    QLabel * lbl_y1;

    QSpinBox * spin_numpoints;
    bool ReadFloatField( QLineEdit *, double *, double, double, double );

public slots:
    void MarkBad( QLineEdit * edt );
    void OnFieldChange( const QString & dummy );
    void btn_sphere_toggled( bool );
    void btn_plane_toggled( bool );
    void btn_square_clicked( void );
    void btn_U1_toggled( bool );
    void btn_U2_toggled( bool );
    void btn_V1_toggled( bool );
    void btn_V2_toggled( bool );
};


#endif /* WIN_VIEW_H */
