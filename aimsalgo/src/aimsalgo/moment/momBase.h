
#ifndef AIMS_MOMENT_MOMBASE_H
#define AIMS_MOMENT_MOMBASE_H


class MomentBase
{
  public:
  
    enum MomentType
    {
      Normal,
      Incremental,
      Volumic,
      Surfacic
    };
    
    MomentBase() { }
    virtual ~MomentBase() { }
};

#endif
