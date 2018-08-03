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

//--- plugin -----------------------------------------------------------------
#include <soma-io/image/gisimagereader.h>
#include <soma-io/image/gisimagewriter.h>
//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeformatreader.h>
#include <cartodata/io/volumeformatwriter.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/io/formatdictionary.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/types.h>
#include <cartobase/plugin/plugin.h>
//--- system -----------------------------------------------------------------
#include <vector>
#include <string>
//----------------------------------------------------------------------------
#include <aims/vector/vector.h>
// #include <aims/math/dtitensor.h>


using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;


namespace carto
{

  class GisPlugin2 : public carto::Plugin
  {
  public:
    GisPlugin2();
    virtual ~GisPlugin2();
    virtual std::string name() const;
    static bool noop();
  };

}

namespace
{

  bool initGis()
  {
    new GisPlugin2;
    return true;
  }

  bool gisinit __attribute__((unused)) = initGis();

}


GisPlugin2::GisPlugin2() : Plugin()
{
    vector<string>  exts(2);
    exts[0] = "ima";
    exts[1] = "dim";

    ///////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                             ////
    ///////////////////////////////////////////////////////////////////////////

    //=========================================================================
    //   V O L U M E
    //=========================================================================

    VolumeFormatReader<Point3df> *vfr8 = new VolumeFormatReader<Point3df>;
    vfr8->attach( rc_ptr<ImageReader<Point3df> >( new GisImageReader<Point3df> ) );
    FormatDictionary<Volume<Point3df> >::registerFormat( "GIS", vfr8, exts );

    VolumeFormatReader<Point3d> *vfr16 = new VolumeFormatReader<Point3d>;
    vfr16->attach( rc_ptr<ImageReader<Point3d> >( new GisImageReader<Point3d> ) );
    FormatDictionary<Volume<Point3d> >::registerFormat( "GIS", vfr16, exts );

    VolumeFormatReader<Point2d> *vfr32 = new VolumeFormatReader<Point2d>;
    vfr32->attach( rc_ptr<ImageReader<Point2d> >( new GisImageReader<Point2d> ) );
    FormatDictionary<Volume<Point2d> >::registerFormat( "GIS", vfr32, exts );

    typedef AimsVector<float,6> vectorf6;

    VolumeFormatReader<vectorf6> *vfr64 = new VolumeFormatReader<vectorf6>;
    vfr64->attach( rc_ptr<ImageReader<vectorf6> >( new GisImageReader<vectorf6> ) );
    FormatDictionary<Volume<vectorf6> >::registerFormat( "GIS", vfr64, exts );

//     VolumeFormatReader<DtiTensor *> *vfru8 = new VolumeFormatReader<DtiTensor>;
//     vfru8->attach( rc_ptr<ImageReader<DtiTensor> >( new GisImageReader<DtiTensor> ) );
//     FormatDictionary<Volume<DtiTensor> >::registerFormat( "GIS", vfru8, exts );


    //=========================================================================
    //   V O L U M E   R E F
    //=========================================================================

    VolumeRefFormatReader<Point3df> *rfr8 = new VolumeRefFormatReader<Point3df>;
    rfr8->attach( rc_ptr<ImageReader<Point3df> >( new GisImageReader<Point3df> ) );
    FormatDictionary<VolumeRef<Point3df> >::registerFormat( "GIS", rfr8, exts );

    VolumeRefFormatReader<Point3d> *rfr16 = new VolumeRefFormatReader<Point3d>;
    rfr16->attach( rc_ptr<ImageReader<Point3d> >( new GisImageReader<Point3d> ) );
    FormatDictionary<VolumeRef<Point3d> >::registerFormat( "GIS", rfr16, exts );

    VolumeRefFormatReader<Point2d> *rfr32 = new VolumeRefFormatReader<Point2d>;
    rfr32->attach( rc_ptr<ImageReader<Point2d> >( new GisImageReader<Point2d> ) );
    FormatDictionary<VolumeRef<Point2d> >::registerFormat( "GIS", rfr32, exts );

    VolumeRefFormatReader<vectorf6> *rfr64 = new VolumeRefFormatReader<vectorf6>;
    rfr64->attach( rc_ptr<ImageReader<vectorf6> >( new GisImageReader<vectorf6> ) );
    FormatDictionary<VolumeRef<vectorf6> >::registerFormat( "GIS", rfr64, exts );

//     VolumeRefFormatReader<DtiTensor> *rfru8 = new VolumeRefFormatReader<DtiTensor>;
//     rfru8->attach( rc_ptr<ImageReader<DtiTensor> >( new GisImageReader<DtiTensor> ) );
//     FormatDictionary<VolumeRef<DtiTensor> >::registerFormat( "GIS", rfru8, exts );

    ///////////////////////////////////////////////////////////////////////////
    ////                           W R I T E R                             ////
    ///////////////////////////////////////////////////////////////////////////

    //=========================================================================
    //   V O L U M E
    //=========================================================================

    //--- INT -----------------------------------------------------------------

    VolumeFormatWriter<Point3df> *vfw8 = new VolumeFormatWriter<Point3df>;
    vfw8->attach( rc_ptr<ImageWriter<Point3df> >( new GisImageWriter<Point3df> ) );
    FormatDictionary<Volume<Point3df> >::registerFormat( "GIS", vfw8, exts );

    VolumeFormatWriter<Point3d> *vfw16 = new VolumeFormatWriter<Point3d>;
    vfw16->attach( rc_ptr<ImageWriter<Point3d> >( new GisImageWriter<Point3d> ) );
    FormatDictionary<Volume<Point3d> >::registerFormat( "GIS", vfw16, exts );

    VolumeFormatWriter<Point2d> *vfw32 = new VolumeFormatWriter<Point2d>;
    vfw32->attach( rc_ptr<ImageWriter<Point2d> >( new GisImageWriter<Point2d> ) );
    FormatDictionary<Volume<Point2d> >::registerFormat( "GIS", vfw32, exts );

    VolumeFormatWriter<vectorf6> *vfw64 = new VolumeFormatWriter<vectorf6>;
    vfw64->attach( rc_ptr<ImageWriter<vectorf6> >( new GisImageWriter<vectorf6> ) );
    FormatDictionary<Volume<vectorf6> >::registerFormat( "GIS", vfw64, exts );

//     VolumeFormatWriter<DtiTensor> *vfwu8 = new VolumeFormatWriter<DtiTensor>;
//     vfwu8->attach( rc_ptr<ImageWriter<DtiTensor> >( new GisImageWriter<DtiTensor> ) );
//     FormatDictionary<Volume<DtiTensor> >::registerFormat( "GIS", vfwu8, exts );

    //=========================================================================
    //   V O L U M E   R E F
    //=========================================================================

    VolumeRefFormatWriter<Point3df> *rfw8 = new VolumeRefFormatWriter<Point3df>;
    rfw8->attach( rc_ptr<ImageWriter<Point3df> >( new GisImageWriter<Point3df> ) );
    FormatDictionary<VolumeRef<Point3df> >::registerFormat( "GIS", rfw8, exts );

    VolumeRefFormatWriter<Point3d> *rfw16 = new VolumeRefFormatWriter<Point3d>;
    rfw16->attach( rc_ptr<ImageWriter<Point3d> >( new GisImageWriter<Point3d> ) );
    FormatDictionary<VolumeRef<Point3d> >::registerFormat( "GIS", rfw16, exts );

    VolumeRefFormatWriter<Point2d> *rfw32 = new VolumeRefFormatWriter<Point2d>;
    rfw32->attach( rc_ptr<ImageWriter<Point2d> >( new GisImageWriter<Point2d> ) );
    FormatDictionary<VolumeRef<Point2d> >::registerFormat( "GIS", rfw32, exts );

    VolumeRefFormatWriter<vectorf6> *rfw64 = new VolumeRefFormatWriter<vectorf6>;
    rfw64->attach( rc_ptr<ImageWriter<vectorf6> >( new GisImageWriter<vectorf6> ) );
    FormatDictionary<VolumeRef<vectorf6> >::registerFormat( "GIS", rfw64, exts );

//     VolumeRefFormatWriter<DtiTensor> *rfwu8 = new VolumeRefFormatWriter<DtiTensor>;
//     rfwu8->attach( rc_ptr<ImageWriter<DtiTensor> >( new GisImageWriter<DtiTensor> ) );
//     FormatDictionary<VolumeRef<DtiTensor> >::registerFormat( "GIS", rfwu8, exts );
}


GisPlugin2::~GisPlugin2()
{
}


string GisPlugin2::name() const
{
  return string("GIS CARTO misc types");
}


bool GisPlugin2::noop()
{
  return true;
}

