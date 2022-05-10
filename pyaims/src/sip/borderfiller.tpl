class BorderFiller_%Template1typecode%
{
%TypeHeaderCode
#include <aims/border/borderfiller.h>
%Template1typeinclude%

#ifndef PYAIMS_BORDERFILLER_%Template1typecode%_DEFINED
#define PYAIMS_BORDERFILLER_%Template1typecode%_DEFINED
typedef aims::BorderFiller<%Template1%> BorderFiller_%Template1typecode%;
#endif
%End

    public :
      static void fillConstant( rc_ptr_Volume_%Template1typecode% data, 
                                const %Template1% & value = 0, 
                                bool unread_only = false ) /ReleaseGIL/;
%Docstring
  static void fillConstant( rc_ptr_Volume_%Template1typecode% data, 
                                const %Template1% & value = 0, 
                                bool unread_only = false )

  Fills volume border with a constant value.
  The function modifies the input data, and does not return a new one.

  Parameters
  ----------
  data: Volume_%Template1typecode% instance to fill border.
  value: %Template1% value to fill border with (optional)
         Default is 0.
  unread_only: bool (optional)
         For partial reading, fill the data only where it has not been read.
         Default is False

%End

    static void fillMedian( rc_ptr_Volume_%Template1typecode% data,
                            Point4dl size = Point4dl(-1,-1,-1,-1), 
                            bool unreadOnly = false ) /ReleaseGIL/;
%Docstring
  static void fillMedian( rc_ptr_Volume_%Template1typecode% data,
                            Point4dl size = Point4dl(-1,-1,-1,-1), 
                            bool unreadOnly = false )

  Fills the border with a median value. The median value is processed in the 
  inside border.
  The function modifies the input data, and does not return a new one.

  Parameters
  ----------
  data: Volume_%Template1typecode% instance to fill border.
  size: Point4dl size of inside border to use to define median value
        [-1,-1,-1,-1] means that the median value is processed in the inside 
        border of equal outside border size. if the outside border is of size 
        (2, 2, 0) in dimensions x, y, z, the inside border is also of size 
        (2, 2, 0) (optional).
        Default is [-1,-1,-1,-1]. 

  unread_only: bool (optional)
         For partial reading, fill the data only where it has not been read.
         Default is False.

%End

    static void fillNearest( rc_ptr_Volume_%Template1typecode% data, 
                             bool unreadOnly = false ) /ReleaseGIL/;
%Docstring
  static void fillNearest( rc_ptr_Volume_%Template1typecode% data,
                           bool unreadOnly = false )

  Fills the border with the nearest inside voxel value.
  The function modifies the input data, and does not return a new one.

  Parameters
  ----------
  data: Volume_%Template1typecode% instance to fill border.

  unread_only: bool (optional)
         For partial reading, fill the data only where it has not been read.
         Default is False.

%End
    static void fillMirror( rc_ptr_Volume_%Template1typecode% data, 
                            bool unreadOnly = false ) /ReleaseGIL/;
%Docstring
  static void fillMirror( rc_ptr_Volume_%Template1typecode% data,
                            bool unreadOnly = false )

  Fills the border mirroring the inside border.
  The function modifies the input data, and does not return a new one.

  Parameters
  ----------
  data: Volume_%Template1typecode% instance to fill border.

  unread_only: bool (optional)
         For partial reading, fill the data only where it has not been read.
         Default is False.

%End
    /// Build a view that exposes data filled at reading time. Borders of this view
    /// correpond to data unfilled at reading time (i.e. borders that are outside of 
    /// the read image domain). This is useful to fill borders of partialy read \c carto::VolumeRef.
    //static rc_ptr_Volume_%Template1typecode% buildUnfilledBorderView(const rc_ptr_Volume_%Template1typecode% & vol);

};
