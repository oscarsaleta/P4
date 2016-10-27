#ifndef WIN_PARAMS_H
#define WIN_PARAMS_H


#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QWidget>

#include "win_find.h"


class QParamsDlg : public QWidget
{
    Q_OBJECT

public:
    QParamsDlg( QFindDlg * startwindow );
    ~QParamsDlg();
    void GetDataFromDlg( void );
    void UpdateDlgData( void );
    void ExclusiveToggle( bool, QRadioButton *, ... );

private:
    QFindDlg * parent;
    QBoxLayout * mainLayout;

    QRadioButton * btn_sepyes;
    QRadioButton * btn_sepno;
    QRadioButton * btn_num;
    QRadioButton * btn_alg;
    QSpinBox * spin_precis;
    QSpinBox * spin_precis0;
    QSpinBox * spin_level;
    QSpinBox * spin_numlevel;
    QSpinBox * spin_maxlevel;
    QSpinBox * spin_weakness;
    QSpinBox * spin_p;
    QSpinBox * spin_q;
    QLineEdit * edt_epsilon;
    QLineEdit * edt_x0;
    QLineEdit * edt_y0;

    QLabel * lbl_x0;
    QLabel * lbl_y0;
    QLabel * lbl_p;
    QLabel * lbl_q;

public slots:
    void btn_num_toggled( bool );
    void btn_alg_toggled( bool );
    void btn_sepyes_toggled( bool );
    void btn_sepno_toggled( bool );
    void OnLevelChange( int );
};


#endif /* WIN_PARAMS_H */
