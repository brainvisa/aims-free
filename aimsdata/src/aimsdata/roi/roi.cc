/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/*
 *  Region Of Interest
 */
#include <aims/roi/roi.h>
#include <aims/roi/hie.h>
#include <aims/io/hieR.h>
#include <aims/def/path.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/datatypecode.h>
#include <cartobase/stream/fileutil.h>
#include <vector>

using namespace aims;
using namespace carto;
using namespace std;

AimsRoi::AimsRoi( Hierarchy *hierarchy ) : Graph( "RoiArg" )
{
  setProperty( "filename_base", string( "." ) );
  rc_ptr<GraphElementTable>	get( new GraphElementTable );

  GraphElementCode	& gec = (*get)[ "roi" ][ "roi" ];
  gec = GraphElementCode( "roi", "Bucket", "VOID", GraphElementCode::Global );
  gec.attribute = "aims_roi";
  gec.local_file_attribute = "roi_filename";
  gec.global_index_attribute = "roi_label";
  gec.global_filename = "roi_bucket.bck";
  gec.syntax = "roi";
  setProperty( "aims_objects_table", get );

  if (!hierarchy)
    {
      std::string syntax_file = Path::singleton().hierarchy()
        + FileUtil::separator() + "neuronames.hie";
      HierarchyReader hrd(syntax_file, hDefault.syntax());
      hrd >> hDefault;
      hRoi = &hDefault;
    }
  else  hRoi = hierarchy;
}


void AimsRoi::setBucketFilename( const string& bname )
{
  rc_ptr<GraphElementTable>	get;
  setProperty( "aims_objects_table", get );
  if( !get )
    {
      cerr << "no aims_objects_table in graph\n";
      return;
    }

  string	fname = bname, ext;
  string::size_type	pos = fname.rfind( '.' );

  if( pos != string::npos )
    ext = fname.substr( pos, fname.length() - pos );

  if ( ext == ".arg" || ext == ".bck" )
    fname = fname.substr( 0, fname.length() - pos );

  string tmpName = fname + ".bck";
  pos = tmpName.rfind( FileUtil::separator() );
  if( pos != string::npos )
    tmpName.erase( 0, pos + 1 );

  GraphElementCode	& gec = (*get)[ "roi" ][ "roi" ];
  gec.global_filename = tmpName;
}



set<Vertex*> AimsRoi::VertByNameAndDescendantName(string& name)
{
  set<Vertex*> outvertices;

  set<Vertex*> sv = vertices();
  for(VSet::const_iterator v = sv.begin(); v != sv.end(); ++v)
    {
      if ((*v)->hasProperty("name"))
	{
	  string tmp; (*v)->getProperty("name", tmp);
	  if (tmp == "unknown") 

	    {
	      int labelToMatch;
	      if ((*v)->hasProperty("roi_label"))
		{
		  (*v)->getProperty("roi_label", labelToMatch);
		  // Search a NAME in the tree
		  set<Tree*> sthead = hRoi->getElementsWith("name", name);
		  ASSERT (sthead.size() < 2); // a NAME appears more than twice

		  if (sthead.size() != 0)
		    {     // Check attr on the node
		      if ((*sthead.begin())->hasProperty("roi_label"))
			{
			  int l = 0;
			  if ((*sthead.begin())->getProperty("roi_label", l)
			      && (l == labelToMatch))
			    {
			      outvertices.insert(*v); 
			    } 
			}
		         // Nww recurse on the descendant nodes 
		      set<Tree*> st;
		      st=(*sthead.begin())->getElementsWith("roi_label",
							    labelToMatch);
		      for(set<Tree*>::const_iterator candidate = st.begin();
			                           candidate  != st.end();
			                           candidate++)
			{
			  outvertices.insert(*v);
			}
		    }
		}
	    }
	  else
	    {
	      if (tmp == name)  outvertices.insert(*v);
	    }
	}
    }

  return( outvertices );
}

void AimsRoi::data2bucket()
{
  // Call data2bucket with a temporary empty name list
  return data2bucket( map<int, std::string>() );
}


void AimsRoi::data2bucket( const map<int, string> &nameList )
{
  GraphManip::volume2Buckets( *this );

  Graph::iterator			iv, ev = end();
  rc_ptr<BucketMap<Void> >		rb;
  map<int, string>::const_iterator	it, et = nameList.end();
  int					label, bSize;
  string				lname;

  for( iv=begin(); iv!=ev; ++iv )
    if( (*iv)->getProperty( "aims_roi", rb ) 
	&& (*iv)->getProperty( "roi_label", label ) )
      {
	it = nameList.find( label );
	if ( it != et )
	  lname = it->second;
	else
	  lname = "unknown";
	(*iv)->setProperty( "name", lname );

	if( rb->begin() != rb->end() )
	  {
	    const BucketMap<Void>	& b = *rb;
	    bSize = rb->begin()->second.size();

	    (*iv)->setProperty( "point_number", bSize );
	    (*iv)->setProperty( "size", (float)bSize * b.sizeX() * b.sizeY() 
				 * b.sizeZ() );
	  }
      }
}


AimsRoi* AimsRoi::clipBucket( int x, int y, int z, int lx, int ly, int lz)
{
  vector< int > m,M;
  vector< float > s;
  AimsRoi *outRoi = new AimsRoi;

  outRoi->copyProperties( Object::reference( this->value<Dictionary>() ) );
 
  getProperty( "voxel_size", s );
  m.push_back(0);
  m.push_back(0);
  m.push_back(0);
  M.push_back(lx);
  M.push_back(ly);
  M.push_back(lz);
  outRoi->setProperty( "boundingbox_min", m ); 
  outRoi->setProperty( "boundingbox_max", M ); 
  float pVox = s[0] * s[1] * s[2];    

  int X = x+lx;
  int Y = y+ly;
  int Z = z+lz;
  Point3d Off = Point3d(x, y, z);

  // deep copy GraphElementTable
  rc_ptr<GraphElementTable>	getorig;
  getProperty( "aims_objects_table", getorig );
  rc_ptr<GraphElementTable>	get( new GraphElementTable( *getorig ) );
  outRoi->setProperty( "aims_objects_table", get );
  GraphElementCode		& gec = (*get)[ "roi" ][ "roi" ];
  bool				buckets = true;

  if( gec.storageType == GraphElementCode::GlobalPacked 
      && gec.objectType == "Volume" )
    {
      rc_ptr<AimsData<int16_t> > vol;
      getProperty( gec.global_attribute, vol );
      rc_ptr<AimsData<int16_t> > newvol( new AimsData<int16_t>( lx, ly, lz ) );
      newvol->setSizeXYZT( vol->sizeX(), vol->sizeY(), vol->sizeZ(), 1 );
      if( vol->header() )
	newvol->setHeader( vol->header()->cloneHeader() );
      int ix, iy, iz;
      ForEach3d( (*newvol), ix, iy, iz )
	(*newvol)( ix, iy, iz ) = (*vol)( ix+x, iy+y, iz+z );
      outRoi->setProperty( gec.global_attribute, newvol );
    }

  // copy nodes
  Graph::const_iterator			iv, ev = end();
  Vertex				*v;
  rc_ptr<BucketMap<Void> >		bck, bck2;
  BucketMap<Void>::Bucket::iterator	ib, eb;

  for( iv=begin(); iv!=ev; ++iv )
    {
      v = outRoi->addVertex( "roi" );
      v->copyProperties( Object::reference( (*iv)->value<Dictionary>() ) );
      if( buckets )
	{
	  v->getProperty( gec.attribute, bck );
	  if( bck.get() )
	    {
	      bck2 = rc_ptr<BucketMap<Void> >( new BucketMap<Void> );
	      BucketMap<Void>::Bucket	& b2 = bck2->begin()->second;
	      for( ib=bck->begin()->second.begin(), 
		     eb=bck->begin()->second.end(); ib!=eb; ++ib )
		{
		  const Point3d & tmp = ib->first;
		  if ( (tmp[0] >= x) && (tmp[0] < X) &&
		       (tmp[1] >= y) && (tmp[1] < Y) && 
		       (tmp[2] >= z) && (tmp[2] < Z) )
		    b2[ tmp - Off ];
		}
	      v->setProperty( "point_number", (int) b2.size() );
	      v->setProperty( "size", (float) pVox * b2.size() );
	      v->setProperty( gec.attribute, bck2 );
	    }
	}
    }

  return( outRoi );
}

void AimsRoi::bucket2data( int borderWidth )
{
  GraphManip::buckets2Volume( *this );
  if( borderWidth > 0 )
    {
      rc_ptr<GraphElementTable>	get;
      getProperty( "aims_objects_table", get );
      GraphElementCode		& gec = (*get)[ "roi" ][ "roi" ];
      rc_ptr<AimsData<int16_t> >	vol;
      getProperty( gec.global_attribute, vol );
      rc_ptr<AimsData<int16_t> > 
	vol2( new AimsData<int16_t>( vol->dimX(), vol->dimX(), vol->dimX(), 1, 
				   borderWidth ) );
      vol2->setSizeXYZT( vol->sizeX(), vol->sizeY(), vol->sizeZ(), 1 );
      if( vol->header() )
	vol2->setHeader( vol->header()->cloneHeader() );

      int	x, y, z;
      ForEach3d( (*vol), x, y, z )
	(*vol2)( x, y, z ) = (*vol)( x, y, z );
      setProperty( gec.global_attribute, vol2 );
    }
}


void AimsRoi::setLabel( AimsData<int16_t> &label)
{
  vector<int> bmax(3), bmin(3, 0) ;
  bmax[0] = label.dimX() - 1 ;
  bmax[1] = label.dimY() - 1 ;
  bmax[2] = label.dimZ() - 1 ;
  
  vector<float> voxSize(3) ;
  voxSize[0] = label.sizeX() ;
  voxSize[1] = label.sizeY() ;
  voxSize[2] = label.sizeZ() ;
  
  setProperty( "boundingbox_max", bmax) ;
  setProperty( "boundingbox_min", bmin) ;
  setProperty( "voxel_size", voxSize) ;
  GraphManip::buckets2Volume( *this );
  rc_ptr<AimsData<int16_t> >	vol( new AimsData<int16_t>( label ) );
  
  if( label.header() )
    vol->setHeader( label.header()->cloneHeader() );
/*   cerr << "!!!! Cloning " << label.header() << endl ;
  label.header()->cloneHeader() ;
  cerr << "!!!! Cloned " << endl ;
  

  vol->setHeader( label.header()->cloneHeader() );
 */
  setProperty( "aims_roi", vol );

  // check nodes and add new ones
  int			x, y, z;
  map<int, Vertex *>	nodes;
  map<int, int>		counts;
  int			l;
  iterator		iv, ev = end();

  // map existing nodes
  for( iv=begin(); iv!=ev; ++iv )
    if( (*iv)->getProperty( "roi_label", l ) )
      nodes[ l ] = *iv;

  // look for new labels
  ForEach3d( label, x, y, z )
    {
      l = label( x, y, z );
      if( l != 0 )
	{
	  Vertex	*& v = nodes[ l ];
	  if( !v )
	    {
	      v = addVertex( "roi" );
	      v->setProperty( "name", string( "unknown" ) );
	      v->setProperty( "roi_label", l );
	    }
	    ++counts[ l ];
	}
    }
  
  for( map<int, int>::iterator it = counts.begin() ; it != counts.end() ; ++it )
  	cout << "Counts " << it->first << " : " << it->second << endl ;
  cout << "Ndes sise : " << nodes.size() << endl ; 
//  map<int, Vertex *>::const_iterator	in, en = nodes.end();
  iterator	in, en = end();
  float	vs = label.sizeX() * label.sizeY() * label.sizeZ();
  int	c;

  for( in=begin(); in!=en; ++in )
    {
      (*in)->getProperty("roi_label", l) ;
      c = counts[ l ];
      (*in)->setProperty( "point_number", c );
      (*in)->setProperty( "size", vs * c );
    }
}
