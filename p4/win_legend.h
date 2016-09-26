#include <qwidget.h>

class QBoxLayout;

class QLegendWnd : public QWidget
{
    Q_OBJECT
        
public:
    QLegendWnd();
    void paintEvent( QPaintEvent * );
    void calculateGeometry( void );

private:
    QBoxLayout * mainLayout;

    int orbitwidth;
    int sepwidth;

    int hmargin1;
    int hmargin2;
    int hmargin3;
    int hmargin4;
    int hmargin5;
    int hmargin6;
    int interline;
    int xheight;

    int vmargin1;
    int vmargin2;
    int vmargin3;
    int vmargin4;
    int vmargin5;
    int vmargin6;
    int vmargin7;
    int vmargin8;
};
