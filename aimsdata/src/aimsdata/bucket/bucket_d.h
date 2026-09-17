


template<class T> AimsBucket<T> & 
AimsBucket<T>::operator = ( const BucketMap<T> & b2 )
  {
    b1.setSizeXYZT( b2.sizeX(), b2.sizeY(), b2.sizeZ(), b2.sizeT() );
    b1.clear();
    BucketMap<T>::const_iterator		ib, eb = b2.end();
    BucketMap<T>::BucketMap<T>::const_iterator	ibi, ebi;
    AimsBucketItem<T>				bitem;

    for( ib=b2.begin(); ib!=eb; ++ib )
      {
	std::list<AimsBucketItem<T> >	& bk = b1[ ib->first ];
	for( ibi=ib->second.begin(), ebi=ib->second.end(); ibi!=ebi; ++ibi )
	  {
	    bitem.location() = ibi->first;
	    bitem.value() = ibi->second;
	    bk.push_back( bitem );
	  }
      }
  }


