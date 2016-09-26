#include <qdialog.h>

class QPushButton;
class QBoxLayout;
class QCheckBox;
class QLineEdit;

class QPrintDlg : public QDialog
{
Q_OBJECT

private:
    QPushButton * btn_default;
    QPushButton * btn_epsimage;
    QPushButton * btn_xfigimage;
    QPushButton * btn_cancel;
    QPushButton * btn_jpeg;
    QBoxLayout * mainLayout;
    QCheckBox * btn_blackwhite;
    QLineEdit * edt_resolution;
    QLineEdit * edt_linewidth;
    QLineEdit * edt_symbolsize;

public:
    QPrintDlg( QWidget * parent, Qt::WindowFlags f );

    static bool LastBlackWhite;
    static double LastLineWidth;
    static double LastSymbolSize;
    static int LastResolution;

public slots:
    void OnDefaultPrinter( void );
    void OnEpsImagePrinter( void );
    void OnXfigImagePrinter( void );
    void OnJpegImagePrinter( void );
    void OnCancel( void );

    bool ReadDialog( void );
    bool ReadFloatField( QLineEdit * edt, double * presult,
                                   double defvalue, double minvalue, double maxvalue );
    void MarkBad( QLineEdit * edt );

    int GetChosenResolution( void );
    double GetChosenLineWidth( void );
    double GetChosenSymbolSize( void );
};

#define P4PRINT_NONE        0
#define P4PRINT_DEFAULT     1
#define P4PRINT_EPSIMAGE    2
#define P4PRINT_XFIGIMAGE   3
#define P4PRINT_JPEGIMAGE   4
