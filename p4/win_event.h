
class QP4Event : public QEvent
{
public:
    QP4Event( QEvent::Type, void * data );
    void * data() const;
    ~QP4Event();
      
protected:
    void * customData;
};
