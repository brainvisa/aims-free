/*
 *  Copyright (C) 1999-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 */

#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/connectivity/connectivity.h>
#include <aims/utility/converter_volume.h>

#include <iostream>
#include <cartobase/config/verbose.h>

using namespace aims;
using namespace carto;
using namespace std;

template<class T> 
static bool doit( Process &p, const string &filename, Finder & finder);

class LabelMapTexture : public Process
{
public:
  enum ValueMode
  {
    MAJORITY,
    AVERAGE,
    MAX,
    MIN,
    MEDIAN
  };

  LabelMapTexture( const string & meshfile, const string & brainfile,
                   const string & outexfile,
                   float radius, float height, float int_height, int mode,
                   int vmode, const string & projtexfile, float bg,
                   int16_t mbg, const string & apply_to );
  
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  template<class T> 
  bool labelMap( VolumeRef<T> data );

private:
  string       meshf, brainf, otexf, apply_tof;
  float        radius, height, int_height;
  int          mode;
  int          vmode;
  string       projtexfile;
  float        bg;
  int16_t      mbg;
};

LabelMapTexture::LabelMapTexture( const string & meshfile,
                                  const string & brainfile,
                                  const string & outexfile,
                                  float rad, float hei, float int_height,
                                  int mod, int vmode,
                                  const string & projtexfile, float bg,
                                  int16_t mbg, const string & apply_to )
    : Process(), meshf( meshfile ), brainf(brainfile), otexf( outexfile ),
      radius( rad ), height( hei ), int_height( int_height ), mode( mod ),
      vmode( vmode ), projtexfile( projtexfile ), bg( bg ), mbg( mbg ),
      apply_tof( apply_to )
{
    registerProcessType( "Volume", "S8", &doit<int8_t> );
    registerProcessType( "Volume", "U8", &doit<uint8_t> );
    registerProcessType( "Volume", "S16", &doit<int16_t> );
    registerProcessType( "Volume", "U16", &doit<uint16_t> );
    registerProcessType( "Volume", "S32", &doit<int32_t> );
    registerProcessType( "Volume", "U32", &doit<uint32_t> );
    registerProcessType( "Volume", "FLOAT", &doit<float> );
}


template<class T> 
bool doit( Process & p, const string & fname, Finder & f)
{
  if( verbose )
    cout << "Starting doit()" << endl;
  LabelMapTexture               & tp = (LabelMapTexture &) p;
  VolumeRef<T>		data;
  Reader<Volume<T> >	reader( fname );
  string		format = f.format();
  data.reset( reader.read( 0, &format) );
  return( tp.labelMap( data ) );
}


void buildCylinder( set<Point3d, BucketMapLess> & cylinder, const Point3df & u,
                    const Point3df & v, const Point3df & w, float min_cyl,
                    float max_cyl, float radius, const Point3df & p,
                    const rc_ptr<Volume<int16_t> > & brainData,
                    const vector<float> & vs, const Connectivity & connect,
                    int16_t label, int16_t mbg )
{
  float max_radius = max( -min_cyl, max_cyl );
  max_radius = max( max_radius, radius );
  int x, y, z, nx, ny, nz, i;
  float radius2 = radius * radius;

  nx = ceil( max_radius / vs[0] );
  ny = ceil( max_radius / vs[1] );
  nz = ceil( max_radius / vs[2] );

  set<Point3d, BucketMapLess> todo;
  cylinder.clear();
  Point3d pint( short( rint( p[0] / vs[0] ) ), short( rint( p[1] / vs[1] ) ),
                short( rint( p[2] / vs[2] ) ) );
  Point3d pn( nx + 1, ny + 1, nz + 1 );

  VolumeRef<short> mask;
  if( brainData.get() )
  {
    mask.reset( new Volume<short>(
      nx * 2 + 3, ny * 2 + 3, nz * 2 + 3, 1,
      AllocatorContext( &MemoryAllocator::singleton() ) ) );
    // (include a border)
    mask.fill( 0 );
    mask->at( nx + 1, ny + 1, nz + 1 ) = 1; // central point is valid
  }

  for( z=-nz; z<= nz; ++z )
    for( y=-ny; y<= ny; ++y )
      for( x=-nx; x<= nx; ++x )
      {
        Point3df pos( x * vs[0], y * vs[1], z * vs[2] );
        float proj = u.dot( pos );
        if( proj < min_cyl || proj > max_cyl )
          // out of normal projection
          continue;
        float r2 = square( v.dot( pos ) ) + square( w.dot( pos ) );
        if( r2 > radius2 )
          // out of radius to normal axis
          continue;
        Point3d pd = Point3d( x, y, z );
        Point3d pint2 = pint + pd;
        if( pint2[0] < 0 || pint2[1] < 0 || pint2[2] < 0 )
          continue;
        if( brainData.get() )
        {
          if( pint2[0] >= brainData->getSizeX()
              || pint2[1] >= brainData->getSizeY()
              || pint2[2] >= brainData->getSizeZ() )
            continue;

          int16_t m = brainData->at( pint2 );
          if( m == mbg || ( label != mbg && m != label ) )
            // out of brain or label mask
            continue;
          if( x != 0 || y != 0 || z != 0 )
          {
            // check connectivity to central point in mask
            for( i=0; i<connect.nbNeighbors(); ++i )
              if( mask->at( pn + pd + connect.xyzOffset( i ) ) )
              {
                cylinder.insert( pint2 );
                mask->at( pn + pd ) = 1;
                break;
              }
            if( i == connect.nbNeighbors() ) // no connection
            {
              todo.insert( pd );
            }
          }
          else
          {
            cylinder.insert( pint2 );
            mask->at( pn + pd ) = 1;
          }
        }
        else
          cylinder.insert( pint2 );
      }

  // process mask points which did not have a proven connection to the center
  bool removed = true;
  set<Point3d, BucketMapLess>::iterator it, jt;

  /* cout << "point: " << p << endl;
  cout << "todo: " << todo.size() << endl; */
  while( removed && !todo.empty() ) // exit when no point has been updated
  {
    removed = false;
    it = todo.begin();
    while( it!=todo.end() )
    {
      // check connectivity to central point in mask
      for( i=0; i<connect.nbNeighbors(); ++i )
      {
        if( mask->at( pn + *it + connect.xyzOffset( i ) ) != 0 )
        {
          cylinder.insert( *it + pint );
          mask->at( *it + pn ) = 1;
          jt = it;
          ++it;
          todo.erase( jt );
          removed = true;
          break;
        }
      }
      if( i == connect.nbNeighbors() ) // no connection
        ++it;
    }
  }
  if( !todo.empty() )
    cout << "non-connected: " << todo.size() << endl;
}


class BrainMaskReader : public Process
{
public:
  BrainMaskReader() : Process()
  {
      registerProcessType( "Volume", "S8", &doit<int8_t> );
      registerProcessType( "Volume", "U8", &doit<uint8_t> );
      registerProcessType( "Volume", "S16", &doit<int16_t> );
      registerProcessType( "Volume", "U16", &doit<uint16_t> );
      registerProcessType( "Volume", "S32", &doit<int32_t> );
      registerProcessType( "Volume", "U32", &doit<uint32_t> );
      registerProcessType( "Volume", "FLOAT", &doit<float> );
  }

  template <typename T>
  static bool doit( Process & p, const string & fname, Finder & f );

  VolumeRef<int16_t> brainmask;
};


template <typename T>
bool BrainMaskReader::doit( Process & p, const string & fname, Finder & f )
{
  BrainMaskReader & proc = static_cast<BrainMaskReader &>( p );
  Reader<Volume<T> > r( fname );
  VolumeRef<T> vol( r.read() );
  ShallowConverter<VolumeRef<T>, VolumeRef<int16_t> > conv;
  VolumeRef<int16_t> *vp = conv( vol );
  proc.brainmask = *vp;
  delete vp;
}


// --


template <typename U>
class ConvReader : public Process
{
public:
  ConvReader() : Process()
  {
      registerProcessType( "Volume", "S8", &doit<int8_t> );
      registerProcessType( "Volume", "U8", &doit<uint8_t> );
      registerProcessType( "Volume", "S16", &doit<int16_t> );
      registerProcessType( "Volume", "U16", &doit<uint16_t> );
      registerProcessType( "Volume", "S32", &doit<int32_t> );
      registerProcessType( "Volume", "U32", &doit<uint32_t> );
      registerProcessType( "Volume", "FLOAT", &doit<float> );
  }

  template <typename T>
  static bool doit( Process & p, const string & fname, Finder & f );

  VolumeRef<U> values;
};


template <typename U> template <typename T>
bool ConvReader<U>::doit( Process & p, const string & fname, Finder & f )
{
  ConvReader<U> & proc = static_cast<ConvReader<U> &>( p );
  Reader<Volume<T> > r( fname );
  VolumeRef<T> vol( r.read() );
  ShallowConverter<VolumeRef<T>, VolumeRef<U> > conv;
  VolumeRef<U> *vp = conv( vol );
  proc.values = *vp;
  delete vp;
}

namespace
{

  template <typename T>
  inline
  bool first_comparator ( const pair<T, T> & l, const pair<T, T> & r )
  {
    return l.first < r.first;
  }

}

// --

template<class T>
bool LabelMapTexture::labelMap( VolumeRef<T> data )
{
  /* Loading input */
  if(verbose) {
    cout << "Starting LabelMapTexture()" << endl;
  	 cout << "reading mesh...";
  }
  Reader<AimsSurfaceTriangle>	rmesh( meshf );
  AimsSurfaceTriangle		mesh;
  if( !rmesh.read( mesh ) )
    return( false );
  if(verbose)
  	cout << "done.\n";

  rc_ptr<Volume<short> > brainData;
  if( !brainf.empty() )
  {
    if(verbose)
      cout << "reading brain mask...";
    BrainMaskReader br;
    br.execute( brainf );
    brainData = br.brainmask;
    if(verbose)
      cout << "done.\n";
  }

  rc_ptr<Volume<T> > apply_to;
  if( !apply_tof.empty() )
  {
    if( verbose )
      cout << "reading values to apply to...";
    ConvReader<T> fr;
    fr.execute( apply_tof );
    apply_to = fr.values;
    if(verbose)
      cout << "done.\n";
  }

  rc_ptr<TimeTexture<int16_t> > tex_mask;
  if( !projtexfile.empty() )
  {
    Reader<TimeTexture<int16_t> > rt( projtexfile );
    tex_mask.reset( rt.read() );
  }

  TimeTexture<T>             otext;

  /***
  Algorithm
  ***/

  set<size_t>                           instants;
  AimsSurfaceTriangle::iterator         itm, etm = mesh.end();

  //    count mesh times
  for( itm=mesh.begin(); itm!=etm; ++itm )
    instants.insert( (*itm).first );
  //    add remaining vomule times
  size_t                                vt, evt = data->getSizeT();
  for( vt = *instants.rbegin() + 1; vt<evt; ++vt )
    instants.insert( vt );

  set<size_t>::iterator             ins, endt = instants.end();
  unsigned                          p;
  int                               min_cyl, max_cyl;
  T                                 val=0, nlabel, nlabel_max, label_max,
                                    cur_label, apply_val = 0;
  typename vector< pair<T, T> >::iterator    lab, endlab;
  vector< pair<T, T> >              label;
  vector< float >                   vs = data->getVoxelSize();

  vs.resize( 3, 1. );

  set<Point3d, BucketMapLess>                 cylinder;
  set<Point3d, BucketMapLess>::const_iterator ic, ec = cylinder.end();

  // cylinder (radius, alpha*height+1)
  switch ( mode )
  {
    case 1:
      min_cyl = -height;
      max_cyl = 0;
      break;
    case 2:
      min_cyl = 0;
      max_cyl = height;
      break;
    case 3:
      min_cyl = -height;
      max_cyl = height;
      break;
    case 4:
      min_cyl = -int_height;
      max_cyl = height;
      break;
    default:
      min_cyl = -height;
      max_cyl = height;
  }

  Connectivity connect( 0, 0, Connectivity::CONNECTIVITY_26_XYZ );

  for( ins=instants.begin(); ins!=endt; ++ins )
    {
      size_t                             t;

      itm = mesh.lower_bound( *ins );
      if( itm == etm )
        t = (*mesh.rbegin()).first;
      else
        t = (*itm).first;

      t = *ins;
      if( t >= (size_t) data->getSizeT() )
        t = (size_t) ( data->getSizeT() - 1 );

      Texture<T> & otex = otext[t];

      const AimsSurface<3,Void>         & surface = mesh[t];
      const vector<Point3df>            & vert = surface.vertex();
      const vector<Point3df>            & u = surface.normal();
      unsigned                          n = vert.size();
      vector<Point3df>                  v = vector<Point3df>( n );
      vector<Point3df>                  w = vector<Point3df>( n );
      int16_t                           tex_label = mbg;

      if( verbose )
        cout << "Computing sets of normal vectors at each vertex" << endl;

      for( p=0; p<n; ++p )
        {
          if( u[p][0]!=1 && u[p][0]!=-1 ) /* main case */
            {
                w[p][0] = (float) sqrt( 1 - u[p][0]*u[p][0] );
                w[p][1] = -u[p][0] * u[p][1] / w[p][0];
                w[p][2] = -u[p][0] * u[p][2] / w[p][0];

                v[p][0] = 0;
                v[p][1] = -u[p][2] / w[p][0];
                v[p][2] =  u[p][1] / w[p][0];
            }
          else /* u[p][1] = u[p][2] = 0 */
            {
                v[p][1] = 1;
                v[p][0] = v[p][2] = 0;
                w[p][2] = 1;
                w[p][0] = w[p][1] = 0;
            }
        }

      if( verbose ) {
          cout << "Computing texture, radius " << radius << " height " << height  << " mode " << mode << endl;
      }

      otex.reserve( n );

      for( p=0; p<n; p++ )
      {
        // label initialization
        label.clear();

        if( tex_mask.get() )
        {
          // label on texture mask
          tex_label = tex_mask->begin()->second[p];
          if( tex_label == mbg )
          {
            // out of tex mask
            otex.push_back( bg );
            continue;
          }
        }

        buildCylinder( cylinder, u[p], v[p], w[p], min_cyl, max_cyl, radius,
                       vert[p], brainData, vs, connect, tex_label, mbg );

        for( ic=cylinder.begin(); ic!=ec; ++ic )
        {
          val = data->at( (*ic)[0], (*ic)[1], (*ic)[2], t );
          if( val != bg )
          {
            if( apply_to.get() )
              apply_val = apply_to->at( (*ic)[0], (*ic)[1], (*ic)[2], t );
            label.push_back( make_pair( val, apply_val ) );
          }
        }

        if( !label.empty() )
        {
          if( vmode == AVERAGE )
          {
            float sum = 0;
            endlab = label.end();
            for( lab=label.begin(); lab!=endlab; ++lab )
              if( apply_to.get() )
                sum += lab->second;
              else
                sum += lab->first; // labels are actually values here
            otex.push_back( sum / label.size() );
          }
          else
          {
            sort( label.begin(), label.end(), first_comparator<T> );

            if( vmode == MAJORITY )
            {
              cur_label=-1;
              nlabel=0;
              nlabel_max=0;
              endlab = label.end();
              label_max = 0;
              for(lab=label.begin(); lab!=endlab; ++lab )
              {
                if( lab->first != cur_label )
                {
                    if( nlabel > nlabel_max )
                    {  // the largest label
                      nlabel_max = nlabel;
                      label_max = cur_label;
                    }
                    // new label
                    cur_label = lab->first;
                    nlabel = 1;
                }
                else
                  nlabel ++;
              }

              if( nlabel > nlabel_max )
              {  // the largest label
                  nlabel_max = nlabel;
                  label_max = cur_label;
              }
              otex.push_back( label_max );
            }
            else if( vmode == MAX )
            {
              if( apply_to.get() )
                otex.push_back( label.back().second );
              else
                otex.push_back( label.back().first );
            }
            else if( vmode == MIN )
            {
              if( apply_to.get() )
                otex.push_back( label.front().second );
              else
                otex.push_back( label.front().first );
            }
            else if( vmode == MEDIAN )
            {
              if( apply_to.get() )
                otex.push_back( label[ ( label.size() + 1 ) / 2 ].second );
              else
                otex.push_back( label[ ( label.size() + 1 ) / 2 ].first );
            }
          }
        }
        else
           otex.push_back( bg );
      }
    }

  /* Writing ouput */
  if(verbose)
    cout << "writing texture..." << endl;
  aims::write( otext, otexf );

  if(verbose)
    cout << "End of the process." << endl;
  return( true );
}


int main( int argc, const char** argv )
{
  string                       volumefile, outexfile, apply_to;
  Reader<AimsSurfaceTriangle>  meshfile;
  float                        radius = 1., height = 1., int_height = 0;
  int                          mode = 3;
  int                          vmode = 0;
  Reader<Volume<int16_t> >     projtexfile;
  Reader<Volume<int16_t> >     brainfile;
  float                        bg = 0;
  int16_t                      mbg = 0;

  AimsApplication	app( argc, argv, "Compute label close to mesh and build texture file" );
  app.addOption( volumefile, "-i", "input Label volume" );
  app.addOption( meshfile, "-m", "input mesh" );
  app.addOption( outexfile, "-o", "texture output");
  app.addOption( height, "-height", "cylinder half height (mm) [default=1]", 1 );
  app.addOption( int_height, "-int_height", "cylinder half height on interior side of the mesh (mm). Only used if mode=4. [default=0]", 1 );
  app.addOption( radius, "-radius", "cylinder radius (mm) [default=1]", 1 );
  app.addOption( mode, "-mode", "cylinder direction (1:interior, 2:exterior, "
                 "3:both, 4: both with differing sizes) [default=3]", true );
  app.addOption( brainfile, "-b",
                 "brain mask file [default=no brain mask]. May be either a "
                 "binary volume, or a labels volume (regions): used together "
                 "with the -p option, allows to separate projected values in "
                 "regions they belong to.",
                 true );
  app.addOption( vmode, "-v", "value mode (0: majority (labels), 1: average, "
                 "2: max, 3: min, 4: median)", true );
  app.addOption( projtexfile, "-p",
                 "projection texture. They can be used in two ways:\n- binary "
                 "mask on vertices: only vertices with a non-zero value will "
                 "be processed.\n- labels map: together with a volume labels "
                 "mask (-b option): values from the input (-i) volume will be "
                 "processed only if the label of the voxel (-b) and of the "
                 "projection texture (-p) match. A projection texture can be "
                 "obtained using a first run of AimsLabels2Tex, then used as "
                 "input to a second run using float data: it will thus mask "
                 "each region to avoid mixing values from different labelled "
                 "regions.", true );
  app.addOption( bg, "--bg",
                 "background value in the output texture (default: 0)",
                 true );
  app.addOption( mbg, "--maskbg",
                 "background value in the input masks (brain and projection "
                 "texture, if any (default: 0)", true );
  app.addOption( apply_to, "-a",
                 "apply to values from this other input volume.\n"
                 "In min/max/median modes, take the min/max/median position "
                 "for each cylinder from the input volume, but take the "
                 "output texture value in another volume specified here. This "
                 "option is not useful in average mode since in that case it "
                 "does not differ from using an optional mask and the input "
                 "volume. To be clearer we get for each cylinder the max "
                 "position (for instance) of input volume (-i) values, then "
                 "get the output texture from this volume (-a) at the same "
                 "position.\nThe \"majority\" value mode does not use this "
                 "additional volume.\n"
                 "The apply_to volume is converted into the same data type as "
                 "the main input volume." );
  app.alias( "--mesh", "-m" );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--apply_to", "-a" );

  try
  {
    app.initialize();
    if( verbose )
      cout << "Init program" << endl;
    LabelMapTexture	proc( meshfile.fileName(), brainfile.fileName(),
                            outexfile,
                            radius, height, int_height, mode, vmode,
                            projtexfile.fileName(), bg, mbg, apply_to );
    if( verbose )
      cout << "Starting program.." << endl;
    if( !proc.execute( volumefile ) )
    {
      cout << "Couldn't process file - aborted\n";
      return( 1 );
    }
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
