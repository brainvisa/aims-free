

#ifndef _AIMSCORRECTIONEPI_SMOOTHER_H_
#define _AIMSCORRECTIONEPI_SMOOTHER_H_



class Smoother
{

  public:

    Smoother( int levels ) : _levels( levels ) {}
    virtual ~Smoother() {}

    virtual void doIt( float* p12linear ) = 0;

  protected:

    int _levels;

};


#endif
