
class Converter_Volume_%Template1typecode%_Volume_%Template2typecode%
{
%TypeHeaderCode
#include <aims/utility/converter_volume.h>
#include <pyaims/data/data.h>
#include <aims/utility/converter_rgb.h>
typedef carto::Converter<AimsData<%Template1%>, AimsData<%Template2%> > 
  Converter_Volume_%Template1typecode%_Volume_%Template2typecode%;
%End

public:
  Converter_Volume_%Template1typecode%_Volume_%Template2typecode%( bool = false );
  ~Converter_Volume_%Template1typecode%_Volume_%Template2typecode%();

%If ( CARTODATA )

  Volume_%Template2typecode%* operator () ( const Volume_%Template1typecode% & ) const;
%MethodCode
  carto::rc_ptr<Volume_%Template1typecode%>	r( const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::rc_ptr<Volume_%Template2typecode%>	resvol;
  {
    AimsData_%Template1typecode% dat( r );
    AimsData_%Template2typecode% *d = (*sipCpp)( r );
    resvol = d->volume();
  }
  r.release();
  sipRes = resvol.get();
  resvol.release();
%End

  void convert( const Volume_%Template1typecode% &, Volume_%Template2typecode% & ) const;
%MethodCode
  carto::rc_ptr<Volume_%Template1typecode%> r1( const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::rc_ptr<Volume_%Template2typecode%> r2( a1 );
  AimsData_%Template1typecode%  dat1( r1 );
  AimsData_%Template2typecode%  dat2( r2 );
  sipCpp->convert( dat1, dat2 );
  r2.release();
  r1.release();
%End

%End

  AimsData_%Template2typecode%* operator ()
    ( const AimsData_%Template1typecode% & ) const;
  void convert( const AimsData_%Template1typecode% &,
    AimsData_%Template2typecode% & ) const;
};


class ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%
{
%TypeHeaderCode
#include <aims/utility/converter_volume.h>
#include <pyaims/data/data.h>
#include <aims/utility/converter_rgb.h>
typedef carto::ShallowConverter<AimsData<%Template1%>, AimsData<%Template2%> > 
  ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%;
%End

public:
  ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%( bool = false );
  ~ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%();

%If ( CARTODATA )

  Volume_%Template2typecode%* operator () ( const Volume_%Template1typecode% & ) const;
%MethodCode
  carto::rc_ptr<Volume_%Template1typecode%>	r( const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::rc_ptr<Volume_%Template2typecode%>	resvol;
  {
    AimsData_%Template1typecode% dat( r );
    AimsData_%Template2typecode% *d = (*sipCpp)( r );
    resvol = d->volume();
  }
  r.release();
  sipRes = resvol.get();
  resvol.release();
%End

  void convert( const Volume_%Template1typecode% &, Volume_%Template2typecode% & ) const;
%MethodCode
  carto::rc_ptr<Volume_%Template1typecode%> r1( const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::rc_ptr<Volume_%Template2typecode%> r2( a1 );
  AimsData_%Template1typecode%  dat1( r1 );
  AimsData_%Template2typecode%  dat2( r2 );
  sipCpp->convert( dat1, dat2 );
  r2.release();
  r1.release();
%End

%End

  AimsData_%Template2typecode%* operator ()
    ( const AimsData_%Template1typecode% & ) const;
  void convert( const AimsData_%Template1typecode% &,
    AimsData_%Template2typecode% & ) const;
};

