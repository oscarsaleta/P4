#include "win_event.h"


QP4Event::QP4Event( QEvent::Type t, void * data )
                    : QEvent(t)
{
    customData = data;
}

void * QP4Event::data() const { return customData; }

QP4Event::~QP4Event()
{
    // data is freeed by caller
}
