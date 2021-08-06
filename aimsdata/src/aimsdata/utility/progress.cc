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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/utility/progress.h>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

namespace aims {

  template <class T1, class T2>
  ProgressInfo<T1, T2>::ProgressInfo(const T1 min,
                                     const T1 max,
                                     const T2 progressmin,
                                     const T2 progressmax,
                                     const std::string unit,
                                     const int width )
    : _displayed(0),
      _width(width),
      _scale(((double)(progressmax) - (double)(progressmin)) / ((double)(max) - (double)(min))),
      _current(min),
      _min(min),
      _max(max),
      _progressprec(progressmin),
      _progressmin(progressmin),
      _progressmax(progressmax),
      _unit(unit) {
  }

  template <class T1, class T2>
  ProgressInfo<T1, T2>::ProgressInfo( const T1 max )
    : _displayed(0),
      _width(3),
      _scale(100. / (double)(max)),
      _current(T1(0)),
      _min(T1(0)),
      _max(max),
      _progressprec(0),
      _progressmin(0),
      _progressmax(100),
      _unit("%") {
  }

  template <class T1, class T2>
  T1& ProgressInfo<T1, T2>::current(){
    return _current;
  }

  template <class T1, class T2>
  void ProgressInfo<T1, T2>::reset(){
    _progressprec = _progressmin;
    _current = _min;
  }

  template <class T1, class T2>
  const double& ProgressInfo<T1, T2>::scale() const {
    return _scale;
  }

  template <class T1, class T2>
  double ProgressInfo<T1, T2>::rescale(const T1& value) const {
    return round((((double)value) - _min) * _scale + _progressmin);
  }

  template <class T1, class T2>
  double ProgressInfo<T1, T2>::progression() const {
    return rescale(_current);
  }

  template <class T1, class T2>
  std::string ProgressInfo<T1, T2>::erase() const {
    std::string s;
    s.assign((size_t)_displayed, '\b');
    return s;
  }

  template <class T1, class T2>
  std::string ProgressInfo<T1, T2>::render(const bool force) {
    // Only render if displayed value changed
    double p = this->progression();
    if( p != this->_progressprec ||
        this->_current == this->_min || // This is necessary to start display
        force )
    {
      std::string s = toString(), e = erase();
      _displayed = s.length();
      _progressprec = p;
      return e + s;
    }
    else
      return "";
  }

  template <class T1, class T2>
  void ProgressInfo<T1, T2>::print(const bool force) {
    // Only render if displayed value changed
    double p = this->progression();
    if( p != this->_progressprec ||
        _displayed == 0 || // This is necessary to start display
        force )
    {
      std::string s = toString(), e = erase();
      _displayed = s.length();
      _progressprec = p;
      std::cout << e + s << std::flush;
    }
  }

  template <class T1, class T2>
  std::string ProgressInfo<T1, T2>::toString() const {
    std::stringstream s;
    s << std::setw(_width) << progression() << " " << _unit;
    return s.str();
  }

  template <class T1, class T2>
  ProgressInfo<T1, T2>& ProgressInfo<T1, T2>::operator++(){
    this->_current++;
    return *this;
  }

  template <class T1, class T2>
  ProgressInfo<T1, T2> ProgressInfo<T1, T2>::operator++ (int)
  {
    ProgressInfo<T1, T2> p = *this;
    ++(*this);
    return p;
  }

  template <class T1, class T2>
  ProgressInfo<T1, T2>& ProgressInfo<T1, T2>::operator+=(const T1& r)
  {
    this->_current = this->_current + r;
    return *this;
  }

  //============================================================================
  //   I N S T A N C I A T I O N
  //============================================================================
  template class ProgressInfo<double, double>;

  template std::ostream& operator<< (std::ostream &out, aims::ProgressInfo<double, double> &progression);
}
