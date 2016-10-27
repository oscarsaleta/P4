#ifndef WIN_EVENT_H
#define WIN_EVENT_H


#include <QEvent>


class QP4Event : public QEvent
{
public:
    QP4Event( QEvent::Type, void * data );
    void * data() const;
    ~QP4Event();
      
protected:
    void * customData;
};


#endif /* WIN_EVENT_H */
