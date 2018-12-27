/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#ifndef AIMS_ROI_CLUSTERARG_D_H
#define AIMS_ROI_CLUSTERARG_D_H

#include <aims/roi/clusterArg.h>
#include <aims/utility/threshold.h>
#include <aims/connectivity/component.h>
#include <aims/utility/converter_volume.h>
#include <aims/utility/converter_bucket.h>
#include <aims/resampling/motion.h>
#include <aims/graph/graphmanip.h>
#include <aims/bucket/bucket.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/mesh/mesher.h>
#include <graph/graph/graph.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/exception/file.h>

namespace aims
{

  template<typename T>
  void ClusterArgMaker::make( Graph & gr, const AimsData<T> & data )
  {
    // graph and global attributes

    std::string		graphsyntax = gr.getSyntax();
    std::string		vertexsyntax = "cluster";
    GraphElementCode	bec, mec;

    enum GraphType
      {
        Cluster, 
        Fold
      };

    GraphType		gtype = Cluster;

    //	bucket element code
    bec.objectType = "Bucket";
    bec.dataType = "VOID";
    bec.storageType = GraphElementCode::Global;

    //	mesh element code
    mec.objectType = "Mesh";
    mec.dataType = "VOID";
    mec.storageType = GraphElementCode::Global;

    if( graphsyntax == "CorticalFoldArg" )
      {
        gtype = Fold;
        vertexsyntax = "fold";

        bec.attribute = "aims_ss";
        bec.global_index_attribute = "ss_label";
        bec.local_file_attribute = "ss_filename";
        bec.global_filename = "ss.bck";
        bec.id = "ss";

        mec.attribute = "aims_Tmtktri";
        mec.global_index_attribute = "Tmtktri_label";
        mec.local_file_attribute = "Tmtktri_filename";
        mec.global_filename = "tmtktri.mesh";
        mec.id = "Tmtktri";
      }
    else
      {
        bec.attribute = "aims_bucket";
        bec.global_index_attribute = "bucket_label";
        bec.local_file_attribute = "bucket_filename";
        bec.global_filename = "cluster.bck";
        bec.id = "cluster";

        mec.attribute = "aims_Tmtktri";
        mec.global_index_attribute = "Tmtktri_label";
        mec.local_file_attribute = "Tmtktri_filename";
        mec.global_filename = "Tmtktri.mesh";
        mec.id = "Tmtktri";
      }
    bec.syntax = vertexsyntax;
    mec.syntax = vertexsyntax;

    Vertex	*v;

    gr.setProperty( graphsyntax + "_VERSION", std::string( "1.0" ) );
    std::vector<float>	vs;
    vs.push_back( data.sizeX() );
    vs.push_back( data.sizeY() );
    vs.push_back( data.sizeZ() );
    vs.push_back( data.sizeT() );
    gr.setProperty( "voxel_size", vs );
    std::vector<int>	bb;
    bb.push_back( 0 );
    bb.push_back( 0 );
    bb.push_back( 0 );
    gr.setProperty( "boundingbox_min", bb );
    bb[0] = data.dimX() - 1;
    bb[1] = data.dimY() - 1;
    bb[2] = data.dimZ() - 1;
    gr.setProperty( "boundingbox_max", bb );
    std::string	outdir = _fileout;
    std::string::size_type	pos = outdir.rfind( '.' );
    if( pos != std::string::npos )
      outdir.erase( pos, outdir.length() - pos );
    pos = outdir.rfind( '/' );
    if( pos != std::string::npos )
      outdir.erase( 0, pos + 1 );
    outdir += ".data";
    gr.setProperty( "filename_base", outdir );
    std::vector<int>	colors;
    colors.push_back( 0 );
    colors.push_back( 255 );
    colors.push_back( 0 );
    gr.setProperty( bec.local_file_attribute, colors );
    gr.setProperty( bec.global_index_attribute, colors );

    carto::rc_ptr<GraphElementTable>
      objtable( new GraphElementTable );
    std::map<std::string,GraphElementCode> 
      & cltab = (*objtable)[ vertexsyntax ];
    cltab[ bec.id ] = bec;
    if( _domesh )
      {
        cltab[ mec.id ] = mec;
        gr.setProperty( mec.local_file_attribute, colors );
        gr.setProperty( mec.global_index_attribute, colors );
      }
    gr.setProperty( "aims_objects_table", objtable );

    // thresholding

    bool	thresh = false;
    threshold_t	thtype = AIMS_GREATER_OR_EQUAL_TO;

    if( _upth != FLT_MAX )
      {
        thresh = true;
        if( _lowth != -FLT_MAX )
          thtype = AIMS_BETWEEN;
        else
          {
            thtype = AIMS_LOWER_OR_EQUAL_TO;
            _lowth = _upth;
            _upth = 0;
          }
      }
    else if( _lowth != -FLT_MAX )
      {
        thresh = true;
        thtype = AIMS_GREATER_OR_EQUAL_TO;
        _upth = 0;
      }
    else if( _binarize )
      {
        thresh = true;
        if( _lowth == -FLT_MAX )
          {
            _lowth = 0;
            thtype = AIMS_GREATER_THAN;
          }
        //      _upth = 0;
      }

    AimsData<T>		datat;

    if( thresh )
      {
        AimsThreshold<T, T> th( thtype, (T) _lowth, 
                                        (T) _upth );
        std::cout << "threshold: " << _lowth << ", bin: " << _binarize 
                  << std::endl;
        if( _binarize )
          datat = th.bin( data );
        else
          datat = th( data );
      }
    else
      datat = data;

    // connected components

    std::cout << "extracting connected components...\n";

    AimsData<short> 
      datac( datat.dimX(), datat.dimY(), datat.dimZ(), datat.dimT(), 1 );
    carto::Converter<AimsData<T>, AimsData<short> >	conv2( true );
    conv2.convert( datat, datac );
    datac.fillBorder( -1 );
    /*
      const AttributedHeader 
      *ah = dynamic_cast<const AttributedHeader *>( data.header() );
      if( ah )
      datac.setHeader( new AttributedHeader( *ah ) );
      datac.setSizeXYZT( data.sizeX(), data.sizeY(), data.sizeZ(), 
      data.sizeT() );*/

    // find background level
    short	bg = 0;
    bool	bgfound = false;
    int	x, y, z;

    for( z=0; z<datat.dimZ() && !bgfound; ++z )
      for( y=0; y<datat.dimY() && !bgfound; ++y )
        for( x=0; x<datat.dimX(); ++x )
          if( datat( x, y, z ) == 0 )
            {
              bg = datac( x, y, z );
              bgfound = true;
              break;
            }

    AimsConnectedComponent( datac, _connectivity, bg, false, _minsize, 0, 0 );
    std::cout << "done\n";

    // transform

    Motion	motion;
    if( !_matrix.empty() )
      {
        std::cout << "reading transformation...\n";
        std::ifstream	ms( _matrix.c_str() );
        if( !ms )
          {
            std::cerr << "Can't open transformation file " << _matrix 
                      << std::endl;
            throw carto::file_error( _matrix );
          }
        std::vector<float>	transl( 3 ), rot( 9 );
        ms >> transl[0] >> transl[1] >> transl[2];
        ms >> rot[0] >> rot[1] >> rot[2];
        ms >> rot[3] >> rot[4] >> rot[5];
        ms >> rot[6] >> rot[7] >> rot[8];
        if( !ms )
          {
            std::cerr << "transformation file " << _matrix << " has wrong format\n";
            throw carto::file_error( _matrix );
          }
        std::cout << "done\n";
        // translation is in other form: t' = r^-1 * t
        AffineTransformation3d::Table<float> & rm = motion.matrix();
        rm(0,0) = rot[0];
        rm(0,1) = rot[1];
        rm(0,2) = rot[2];
        rm(1,0) = rot[3];
        rm(1,1) = rot[4];
        rm(1,2) = rot[5];
        rm(2,0) = rot[6];
        rm(2,1) = rot[7];
        rm(2,2) = rot[8];
        Motion	mot2 = motion.inverse();
        rm(0, 3) = transl[0];
        rm(1, 3) = transl[1];
        rm(2, 3) = transl[2];

        Point3df	t2 = mot2.transform( transl[0], transl[1], transl[2] );
        transl[0] = t2[0];
        transl[1] = t2[1];
        transl[2] = t2[2];
        gr.setProperty( "Talairach_translation", transl );
        gr.setProperty( "Talairach_rotation", rot );
        // Talairach_scale ?
      }

    // resampling
    // well, we shouldn't resample anymore!

    // buckets

    std::cout << "converting to buckets...\n";
    carto::RawConverter<AimsData<short>, BucketMap<Void> >	conv( true );
    BucketMap<Void>				bck;
    conv.convert( datac, bck );
    std::cout << "done, " << bck.size() << " buckets\n";

    // meshes

    std::map<size_t, AimsSurfaceTriangle>	mesh;

    if( _domesh )
      {
        std::cout << "meshing components...\n";
        std::map<size_t, std::list<AimsSurfaceTriangle> >	meshl;
        Mesher	mesher;
        mesher.setDecimation( 100.0, _deciMaxClearance, _deciMaxError, 120.0 );
        mesher.setMinFacetNumber( _minFacetNumber );
        mesher.setSmoothing( Mesher::LAPLACIAN, 5, 0.4 );
        mesher.setSmoothingLaplacian( 180. );
        mesher.doit( datac, meshl );
        std::cout << "done\n";
        //if( onenode )
        //{
        std::cout << "regrouping meshes...\n";
        std::map<size_t, std::list<AimsSurfaceTriangle> >::iterator 
          im, em = meshl.end();
        for( im=meshl.begin(); im!=em; ++im )
          {
            //std::cout << "* " << im->first << ", " << im->second.size();
            AimsSurfaceTriangle		& mesh2 = mesh[im->first];
            //im->second.erase( im->second.begin() );
            SurfaceManip::meshMerge( mesh2, im->second );
            //std::cout << " -> " << mesh2.vertex().size() << " vertices\n";
          }
        std::cout << "done\n";
        //}
      }

    // vertices

    BucketMap<Void>				*b;
    BucketMap<Void>::iterator			ib, eb = bck.end();
    char					num[50];
    float	voxvol = bck.sizeX() * bck.sizeY() * bck.sizeZ();
    int						index = 0;
    AimsSurfaceTriangle				*s;
    Point3d					pt;
    BucketMap<Void>::Bucket::iterator		ibb, ebb;
    std::vector<float>				pf(3);
    Point3df					pm;

    std::cout << "making cluster vertices...\n";
    for( ib=bck.begin(); ib!=eb; ++ib )
      {
        std::cout << "cluster " << ib->first << "..." << std::endl;
        v = gr.addVertex( vertexsyntax );
        b = new BucketMap<Void>;
        b->setSizeXYZT( bck.sizeX(), bck.sizeY(), bck.sizeZ(), bck.sizeT() );
        (*b)[0] = ib->second;
        v->setProperty( bec.attribute, carto::rc_ptr<BucketMap<Void> >( b ) );
        v->setProperty( "index", (int) ib->first );
        pt = ib->second.begin()->first;
        sprintf( num, "%d", (int) datat( pt[0], pt[1], pt[2] ) ); // label
        v->setProperty( "name", std::string( num ) );
        v->setProperty( "point_number", (int) ib->second.size() );
        v->setProperty( "size", voxvol * ib->second.size() );
        if( gtype == Fold )
          {
            v->setProperty( "skeleton_label", index++ );
            v->setProperty( "ss_point_number", (int) ib->second.size() );
            v->setProperty( "bottom_point_number", (int) 0 );
          }
        if( _domesh )
          {
            s = new AimsSurfaceTriangle( mesh[ ib->first ] );
            v->setProperty( mec.attribute, 
                             carto::rc_ptr<AimsSurfaceTriangle>( s ) );
          }
        // cluster center
        pf[0] = pf[1] = pf[2] = 0;
        BucketMap<Void>::Bucket	& bk = b->begin()->second;
        for( ibb=bk.begin(), ebb=bk.end(); ibb!=ebb; ++ibb )
          {
            pf[0] += ibb->first[0];
            pf[1] += ibb->first[1];
            pf[2] += ibb->first[2];
          }
        pf[0] = vs[0] * pf[0] / bk.size();
        pf[1] = vs[1] * pf[1] / bk.size();
        pf[2] = vs[2] * pf[2] / bk.size();
        v->setProperty( "gravity_center", pf );
        // in talairach space
        pm = motion.transform( pf[0], pf[1], pf[2] );
        pf[0] = pm[0];
        pf[1] = pm[1];
        pf[2] = pm[2];
        if( gtype == Fold )
          v->setProperty( "refgravity_center", pf );
        else
          v->setProperty( "Talairach(mm)", pf );
        sprintf( num, "%f %f %f", pf[0], pf[1], pf[2] );
        v->setProperty( "label", std::string( num ) );
      }
    std::cout << "clusters created\n";

    //	Relations ?
  }

}

#endif
