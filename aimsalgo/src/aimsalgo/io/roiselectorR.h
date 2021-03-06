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


#ifndef AIMS_IO_ROISELECTORR_H
#define AIMS_IO_ROISELECTORR_H

#include <aims/config/aimsalgo_config.h>
#include <aims/roi/roiselector.h>
#include <graph/tree/tree.h>
#include <graph/tree/treader.h>
#include <cartobase/object/syntax.h>
#include <fstream>
#include <string>

namespace aims
{

  class RoiSelectorReader;

  RoiSelectorReader& operator >> ( RoiSelectorReader&, Tree& );


  class AIMSALGO_API RoiSelectorReader
  {
  public:

    RoiSelectorReader( const std::string& name, const carto::SyntaxSet& stx );
    virtual    ~RoiSelectorReader();
    std::string     removeExtension( const std::string& name );

    void       read( RoiSelector& thing );

    friend RoiSelectorReader& operator >> ( RoiSelectorReader&, RoiSelector& );

  private:

    std::string   _name;
  };


  inline RoiSelectorReader& operator >> ( RoiSelectorReader& reader,
					  RoiSelector& thing  )
  {
    reader.read( thing );
    return reader;
  }

}


#endif
