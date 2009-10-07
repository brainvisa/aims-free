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
#ifndef AIMS_ANYTYPE_READER_H
#define AIMS_ANYTYPE_READER_H

// includes from STL
#include <iostream>
#include <string>

// includes from AIMS
#include "aims/data/data_g.h"
#include "aims/io/process.h"
#include "aims/utility/converter_g.h"

namespace aims
{

  namespace
  {
    /**   A structure used within RawConverterWithAllocation to allocate data.
      */
    // TODO: Add specializations for type couples we wish to use.
    // NB: The reason why it is not a member class of RawConverterWithAllocation is that
    // it then cannot be partially specialized :(
    template < typename T1, typename T2 >
    struct Allocator;

    /**   Specialization for AimsData.
      */
    template < typename T1, typename T2 >
    struct Allocator< AimsData<T1>, AimsData<T2> >
    {
      /// Allocate image im2 so that it matches im1 in dimension and voxel size.
      void operator()(const AimsData<T1> & im1, AimsData<T2> & im2) const
      {
        im2 = AimsData<T2>(im1.dimX(), im1.dimY(), im1.dimZ());
        im2.setSizeX(im1.sizeX());
        im2.setSizeY(im1.sizeY());
        im2.setSizeZ(im1.sizeZ());
      }
    };
  }

  namespace
  {
    /**   A class that adds output allocation to carto::RawConvertor through the Allocator class.
      */
    template < typename T1, typename T2 >
    class RawConverterWithAllocation
    {
    public: // constructors

      /**   Default constructor
        */
      RawConverterWithAllocation()
        : m_cartoConverter()
        , m_allocator()
      {}

    public: // functions

      /**   Converts input 'in' into output 'out', where 'out' is not necessarily allocated.
        */
      void convert(const T1 & in, T2 & out) const
      {
        m_allocator(in, out);
        m_cartoConverter.convert(in, out);
      }

    private: // data

      carto::RawConverter<T1, T2>   m_cartoConverter;
      Allocator<T1,T2>              m_allocator;
    };
  }


  namespace
  {
    /**   A kind of template typedef, for templated class with two template arguments.
      */
    // TODO: If found useful elsewhere, could be de-anonymized and put in a separate file.
    template < template <typename, typename> class TArg2 >
    struct TArgWrapper2
    {
      template <typename T1, typename T2 >
      struct Type
      {
        typedef TArg2<T1, T2> type;
      };
    };
  }

  //----------------------------------------------------------------------------------

    //------------------------//
   //  AnyTypeReaderProcess  //
  //------------------------//

  /**   A process to read an object of type T, even if the type on file is different.
    *
    *   This is the low-level layer: it has a strong Process flavor.
    *   If you don't need precise Process control and if your type T has a default constructor,
    *   use the AnyTypeReader class instead, that has a more Reader-like interface.
    */
  template < typename T, typename TConvertorFamily = TArgWrapper2<RawConverterWithAllocation> >
  class AnyTypeReaderProcess : public Process
  {
  public: // typedefs
    typedef T                 return_type;
    typedef TConvertorFamily  convertor_type;

  public: // constructors

    /**   Initialize result with init.
      *
      *   NB: there is no default constructor for AnyTypeReader so that it compiles even if T itself
      *   does not have a default constructor.
      */
    explicit AnyTypeReaderProcess(const T & init);

  public: // set & get

    /** Get data loaded from file.  */
    T       & get()       { return m_res; }
    /** Get data loaded from file.  */
    T const & get() const { return m_res; }

  private: // functions

    void registerProcesses();

  private: // data

    T m_res;
  };


  //----------------------------------------------------------------------------------

    //-----------------//
   //  AnyTypeReader  //
  //-----------------//
  
  /**   A class to read an object of type T, even if the type on file is different.
    *
    *   Conversion is taken care of by convertor classes, indexed by the TConvertorFamily "template typename".
    *   These convertors have to be able to allocate the output if necessary.
    *   NB: error checking during conversion is left to the convertors -- no tests are
    *   done inside AnyTypeReader to check that the conversion succeeded, or even that a convertion
    *   is possible at all. Therefore, it is as safe as the Convertors you provide.
    *
    * Example:
    *
    *   If you want to read an image as an AimsData<float>, whatever the type on file:
    *
    *   std::string filename = "toto.ima";
    *   AnyTypeReader<AimsData<float> > reader(filename);
    *   AimsData<float> im;
    *   reader.read(im);
    */

  template < typename T, typename TConvertorFamily = TArgWrapper2<RawConverterWithAllocation> >
  // private inheritance to hide Process members.
  class AnyTypeReader : private AnyTypeReaderProcess<T, TConvertorFamily>
  {
  public: // typedefs

    typedef AnyTypeReaderProcess<T, TConvertorFamily> Base;

  public: // constructors

    /**   initialize with the name of the file to be read.
      */
    explicit AnyTypeReader(std::string filename);

  public: // functions

    /**   read file and get result.
      */
    void read(T & out);

  private: // data

    std::string m_filename;
  };

} // namespace aims


// implementation
#include "aims/utility/anytype_reader_details.h"

#endif

