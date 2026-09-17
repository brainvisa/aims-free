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
                                     const int width,
                                     ostream & stream )
    : _displayed(0),
      _width(width),
      _scale(((double)(progressmax) - (double)(progressmin)) / ((double)(max) - (double)(min))),
      _current(min),
      _min(min),
      _max(max),
      _progressprec(progressmin),
      _progressmin(progressmin),
      _progressmax(progressmax),
      _unit(unit),
      _stream( stream )
  {
  }

  template <class T1, class T2>
  ProgressInfo<T1, T2>::ProgressInfo( const T1 max,
                                      ostream & stream )
    : _displayed(0),
      _width(3),
      _scale(100. / (double)(max)),
      _current(T1(0)),
      _min(T1(0)),
      _max(max),
      _progressprec(0),
      _progressmin(0),
      _progressmax(100),
      _unit("%"),
      _stream( stream )
  {
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
    if( &_stream == &std::cout )
      s.assign((size_t)_displayed, '\b');
    else
      s = "";
    return s;
  }

  template <class T1, class T2>
  std::string ProgressInfo<T1, T2>::endline() const
  {
    std::string s;
    if( &_stream == &std::cout )
      s = "";
    else
      s = "\n";
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
      return e + s + endline();
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
      _stream << e + s + endline() << std::flush;
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
