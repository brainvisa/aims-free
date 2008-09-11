
namespace aims {

//-----------------------------------------------------------------------------
class Interpolator : carto::RCObject /Abstract/
{
%TypeHeaderCode
#include <aims/resampling/linearInterpolator.h>
%End
public:

  virtual ~Interpolator();

  virtual bool isValid( float, float, float ) const = 0;
  bool isValid( const Point3df & ) const;
  
  double value( float, float, float ) const;
  double value( const Point3df & ) const;

  void values( float, float, float,
               vector_DOUBLE & /Out/ ) const;
  void values( const Point3df &,
               vector_DOUBLE & /Out/ ) const;
};


}; // namespace aims


//-----------------------------------------------------------------------------
%ModuleCode
#include <aims/resampling/linearInterpolator.h>
%End

namespace aims
{

rc_ptr_Interpolator getLinearInterpolator( const std::string & );
rc_ptr_Interpolator getLinearInterpolator( const AimsData_U8 & );
rc_ptr_Interpolator getLinearInterpolator( const AimsData_U16 & );
rc_ptr_Interpolator getLinearInterpolator( const AimsData_S16 & );
rc_ptr_Interpolator getLinearInterpolator( const AimsData_U32 & );
rc_ptr_Interpolator getLinearInterpolator( const AimsData_S32 & );
rc_ptr_Interpolator getLinearInterpolator( const AimsData_FLOAT & );
rc_ptr_Interpolator getLinearInterpolator( const AimsData_DOUBLE & );

};

