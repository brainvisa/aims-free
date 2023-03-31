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

#ifndef CARTODATA_TRANSFORMATION_REFERENTIAL_H
#define CARTODATA_TRANSFORMATION_REFERENTIAL_H

#include <soma-io/transformation/affinetransformation3d_base.h>
#include <cartobase/smart/rcobject.h>


namespace carto
{

  class Referential: RCObject
  {
  public:
    enum Orientation
    {
      Undefined,
      L,
      R,
      A,
      P,
      S,
      I
    };

    Referential();
    Referential( const Referential & ref );
    Referential( Object ref );
    ~Referential();

    Referential & operator = ( const Referential & ref );

    std::string uuid() const { return _uuid; }
    std::string orientationStr() const;
    std::vector<int> axesOrientation() const { return _orientation; }
    rc_ptr<Transformation>
    toOrientation(
      const std::string & orient,
      const std::vector<float> & transl = std::vector<float>() ) const;
    rc_ptr<Transformation>
    toOrientation(
      const std::vector<int> & orient,
      const std::vector<float> & transl = std::vector<float>() ) const;
    std::string lpiReferentialUuid() const { return _lpi_uuid; }

    void setUuid( const std::string & uuid );
    void setOrientation( const std::string & orient );
    void setOrientation( const std::vector<int> & orient );
    void setLpiReferential( const std::string & lpi_uuid );

    static std::string orientationStr( Orientation orient );
    static Orientation orientationCode( const std::string & orient );
    static std::vector<int> orientationVector( const std::string & orient );
    static void setAxisTransform( AffineTransformation3dBase & tr,
                                  Orientation from_code, Orientation code,
                                  const std::vector<float> & transl
                                  = std::vector<float>() );
    Object header() const { return _header; }

  private:
    std::string _uuid;
    std::string _lpi_uuid;
    std::vector<int> _orientation;
    Object _header;
  };

}

#endif

