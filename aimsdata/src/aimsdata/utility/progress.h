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

#ifndef AIMS_UTILITY_PROGRESS_H
#define AIMS_UTILITY_PROGRESS_H

#include <iostream>
#include <string>

namespace aims {

  /** The template class used to display progression in text mode.
  /// Here is an example of how to use the Progression class:
  ///
  /// \code
  #include <iostream>
  #include <unistd.h>
  #include <aims/utility/progress.h>

  int main( int argc, const char* argv[] ) {
    int start = 10, end = 1000;
    aims::Progression progress(start, end);
    std::cout << "Progress ";
    for (int i = start; i <= end; ++i, ++progress)
    {
      usleep(1000);
      std::cout << progress << std::flush;
    }

    std::cout << std::endl;
  }
  /// \endcode
  */
  template <class T1 = double, class T2 = double>
  class ProgressInfo {

    public:
      /**@name Constructor and Destructor*/
      //@{
      /// Constructor of the class
      /// \param min minimum value of the progression
      /// \param max maximum value of the progression
      /// \param progressmin minimum displayed value [default=0]
      /// \param progressmax maximum displayed value [default=100]
      /// \param unit unit of the displayed value
      /// \param width number of charecters used to display the value
      ProgressInfo( const T1 min,
                    const T1 max,
                    const T2 progressmin = (T2)0,
                    const T2 progressmax = (T2)100,
                    const std::string unit = "%",
                    const int width = 3 );

      /// Constructor of the class
      /// \param max maximum value of the progression
      ProgressInfo( const T1 max );
      //@}

      /**@name Methods*/
      //@{
      /// Current value
      T1& current();
      /// Reset the progession to minimum value
      void reset();
      /// Scale that is used between displayed progession range
      /// and values
      const double& scale() const;
      /// Rescale value to its progression value
      const double rescale(const T1& value) const;
      /// Current progression value
      const double progression() const;
      /// Erase the last displayed value
      virtual std::string erase() const;
      /// Render the current progression value if changed or forced.
      /// \param force force to render even the displayed value has not changed
      virtual std::string render(const bool force = false);
      /// Convert progression to the display string
      virtual std::string toString() const;
      /// Print to cout if value changed or forced
      virtual void print( const bool force = false );

      /// Prefix operator to increment internal value
      ProgressInfo<T1, T2>& operator++(); // Prefix operator
      /// Postfix operator to increment internal value
      ProgressInfo<T1, T2> operator++(int); // Postfix operator
      /// Compound assignement operator to increment internal value
      ProgressInfo<T1, T2>& operator+= (const T1& r); // Compound assignment
      //@}

    protected:
      int _displayed, _width;
      double _scale;
      T1 _current, _min, _max;
      T2 _progressprec, _progressmin, _progressmax;
      std::string _unit;
  };

  typedef ProgressInfo<double, double> Progression;

  template <class T1, class T2>
  inline std::ostream& operator<< (
    std::ostream &out, aims::ProgressInfo<T1, T2> &progression)
  {
    std::string p = progression.render();

    if ( !p.empty() )
      out << p;

    return out;
  }

}


// Comparison operators
template <class T1, class T2>
inline bool operator< (const aims::ProgressInfo<T1, T2>& l, const T1& r){return (l.current() < r);}

template <class T1, class T2>
inline bool operator< (const T1& l, const aims::ProgressInfo<T1, T2>& r){return (l < r.current());}

template <class T1, class T2>
inline bool operator> (const aims::ProgressInfo<T1, T2>& l, const T1& r){return (r < l);}

template <class T1, class T2>
inline bool operator> (const T1& l, const aims::ProgressInfo<T1, T2>& r){return (r < l);}

template <class T1, class T2>
inline bool operator<=(const aims::ProgressInfo<T1, T2>& l, const T1& r){return !(l > r);}

template <class T1, class T2>
inline bool operator<=(const T1& l, const aims::ProgressInfo<T1, T2>& r){return !(l > r);}

template <class T1, class T2>
inline bool operator>=(const aims::ProgressInfo<T1, T2>& l, const T1& r){return !(l < r);}

template <class T1, class T2>
inline bool operator>=(const T1& l, const aims::ProgressInfo<T1, T2>& r){return !(l < r);}

#endif
