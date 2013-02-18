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


#ifndef AIMS_IO_GIFTIHEADER_H
#define AIMS_IO_GIFTIHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
extern "C"
{
  #include <gifti_io.h>
}


template <typename T> class TimeTexture;

namespace carto
{
  class Mutex;
}

namespace aims
{

  /**   GIFTI Header class.
  */
  class GiftiHeader : public PythonHeader
  {
  public:
    GiftiHeader( const std::string & name );
    /*GiftiHeader( int dimx, int dimy, int dimz, int dimt, float sx, float sy,
                 float sz, float st, const std::string & name );*/
    virtual ~GiftiHeader();

    const std::string& name() const;
    void setName( const std::string & fname ) { _name = fname; }

    const carto::Object options() const { return _options; }
    void setOptions( carto::Object opt ) { _options = opt; }

    virtual std::string extension() const;

    bool read();
    static std::string niftiRefFromAimsString( const std::string & space );
    static carto::Mutex & giftiMutex();

  private:
    template <int D, typename T>
    friend class GiftiMeshFormat;
    template <typename T>
    friend class GiftiTextureFormat;

    gifti_image* giftiImageBase();
    void giftiAddExternalTextures( gifti_image *gim, int & hdrtexda,
                                   carto::Object da_info );
    void giftiAddLabelTable( gifti_image *gim );
    static carto::Object giftiFindHdrDA( int & nda, carto::Object dainfo,
                                         const std::string & intent );
    static void giftiCopyMetaToGii( carto::Object dainf, giiDataArray *da );
    static void giftiSetTransformations( carto::Object cs, giiDataArray *da );
    template <typename T>
    void giftiAddTexture( gifti_image* gim, const std::vector<T> & tex );
    template <typename T>
    void giftiAddTexture( gifti_image* gim, const TimeTexture<T> & texture );

    //void giftiAddTexture( gifti_image* gim, const std::vector<Void> & tex );
    template <typename T>
    void giftiAddTextureObject( gifti_image* gim, carto::Object texture );

    std::string _name;
    carto::Object _options;
   };

}

#endif
