#ifndef AIMS_UTILITY_CYCLIC_ITERATOR_H_
#define AIMS_UTILITY_CYCLIC_ITERATOR_H_

/* Originally from Catier's aims-til library
*/

namespace aims
{

  //---------------------------------------------------------------------------

  template < typename TContainer, typename TIterator >
  inline TIterator cyclic_advance(TIterator i, TContainer &c)
  {
    if (++i == c.end()) i = c.begin();
    return i;
  }
  
  //---------------------------------------------------------------------------
  
  namespace detail
  {

    //-------------------------------------------------------------------------
    

    /// TODO: when implementing the operator+=, it might be that we need to split
    /// the class in different parts according to TIterator::iterator_category -- unless
    /// we can simply specialize the operator+= outside the class, and use enable_if!
    // We don't derive from the iterator anymore, for several reasons. The first is that TIterator might
    // be a naked pointer, as it is the case for boost::array. Although, of course, one could specialize 
    // cyclic_iterator for pointers. The second reason is that this is not safe to expose functionality
    // of the old iterator, as some functions may be broken for the cyclic iterator. In other words, we
    // expose the part of what we know should work. The other functionality can be retrieve with explicit
    // getting of the underlying iterator. Of course this is not ideal as well, because in some situations, we
    // would like to use it without knowing we are dealing with a cyclic iterator.
    template < typename TIterator >
    class cyclic_iterator_base
    //    : public TIterator
    {
    public: // typedefs -------------------------------------------------------
    
      //typedef TIterator Base;
      
    public: // constructors ---------------------------------------------------
    
      cyclic_iterator_base() : m_iterator(), m_begin(), m_end() {}
      // TODO: it's not clear whether this should initialise the iterator as well
      template < typename TContainer >
      cyclic_iterator_base(TContainer & c) : m_iterator(), m_begin(c.begin()), m_end(c.end()) {}
      /// NB: it is assumed that i does point on one of the element of c (end() accepted)
      template < typename TContainer >
      cyclic_iterator_base(TContainer &c, const TIterator & i) : m_iterator(i), m_begin(c.begin()), m_end(c.end())
      {
        // we treat end as a special case because it is one very common case where
        // the user would provide an iterator lying outside the container.
        // TODO: I guess this won't work with reverse iterators because of the end()...
        if (m_iterator == m_end) m_iterator = m_begin;
      }
      
    public: // functions ------------------------------------------------------
    
      // TODO: operator* does not necessarily return a reference... use a kind of type_of instead.
  
      typename std::iterator_traits<TIterator>::reference
      operator*() { return *m_iterator; }
      
      const typename std::iterator_traits<TIterator>::reference
      operator*() const { return *m_iterator; }
    
      void operator++()
      {
        ++m_iterator;
        if (m_iterator == m_end) m_iterator = m_begin;
      }
      
      void operator--()
      {
        if (m_iterator == m_begin) m_iterator = m_end;
        --m_iterator;
      }
      
      bool operator!=(const TIterator &i)
      {
        // I am just adding an assert to track eventual problems when a cyclic
        // iterator was mistakingly put in a 'for' loop and compared to end()
        assert(i != m_end);
        return m_iterator != i;
      }
      
      // TODO: overload other functions as well. This is one of the danger of
      // deriving from a type: you are exposing all methods! E.g. operator+=
      // still works in the same (non-cyclic) way. This is of course quite a
      // proble, when we inherit from an unknown (template argument) type...
  
      
    private: // data ----------------------------------------------------------
        
      TIterator m_iterator;
      TIterator m_begin;
      TIterator m_end;
    };

    //-------------------------------------------------------------------------

  } // namespace detail
  
  //---------------------------------------------------------------------------

  /// A const cyclic iterator is a const iterator that goes back to the begining
  /// of the container when it reaches the end.
  template < typename TContainer >
  class const_cyclic_iterator 
    : public detail::cyclic_iterator_base<typename TContainer::const_iterator>
  {
  public: // typedefs ---------------------------------------------------------
    typedef detail::cyclic_iterator_base<typename TContainer::const_iterator> Base;
  public: // constructors -----------------------------------------------------
    const_cyclic_iterator() : Base() {}
    const_cyclic_iterator(const TContainer & c) : Base(c) {}
    const_cyclic_iterator(const TContainer & c, const typename TContainer::const_iterator & i) : Base(c,i) {}
  };
  
  //---------------------------------------------------------------------------
  
  /// A cyclic iterator is an iterator that goes back to the begining
  /// of the container when it reaches the end.
  template < typename TContainer >
  class cyclic_iterator
   : public detail::cyclic_iterator_base<typename TContainer::iterator>
  {
  public: // typedefs ---------------------------------------------------------
    typedef detail::cyclic_iterator_base<typename TContainer::iterator> Base;
  public: // constructors -----------------------------------------------------
    cyclic_iterator() : Base() {}
    cyclic_iterator(TContainer & c) : Base(c) {}
    cyclic_iterator(TContainer & c, const typename TContainer::iterator & i) : Base(c, i) {}
  };

  //---------------------------------------------------------------------------

} // namespace til



#endif /* AIMS_UTILITY_CYCLIC_ITERATOR_H_ */
