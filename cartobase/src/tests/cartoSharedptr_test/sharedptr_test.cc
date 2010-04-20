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

#include <cartobase/smart/rcptrtrick.h>
#include <cartobase/exception/assert.h>
#include <list>

using namespace carto;
using namespace std;

namespace
{
  class Toto : public SharedObject
  {
  public:
    Toto() : SharedObject() { cout << "Toto " << this << " created.\n"; }
    virtual ~Toto() { cout << "Toto " << this << " deleting.\n"; }
  };

}

int main( int, char** )
{
  {
    Toto  *obj = new Toto;
    cout << "no ref. canDel: " << obj->testDeletable() << " (should be false)"
        << endl;
    ASSERT( !obj->testDeletable() );
    rc_ptr<Toto> *sp = new rc_ptr<Toto>( obj );
    cout << "strong ref. testDeletable: " << obj->testDeletable() << endl;
    ASSERT( !obj->testDeletable() );
    weak_shared_ptr<Toto> *wsp = new weak_shared_ptr<Toto>( obj );
    cout << "+weak shared ref. testDeletable: " << obj->testDeletable()
        << endl;
    ASSERT( !obj->testDeletable() );
    delete sp;
    cout << "-strong ref. testDeletable: " << obj->testDeletable() << endl;
    ASSERT( obj->testDeletable() );
    delete wsp;
    cout << "-weak shared ref. obj should be destroyed." << endl;

    cout << "------\n";
    obj = new Toto;
    weak_ptr<Toto> *wp = new weak_ptr<Toto>( obj );
    cout << "weak ref.\n";
    delete wp;
    cout << "-weak ref. obj should not be destroyed.\n";
    wp = new weak_ptr<Toto>( obj );
    wsp = new weak_shared_ptr<Toto>( obj );
    weak_shared_ptr<Toto> *wsp2 = new weak_shared_ptr<Toto>( obj );
    cout << "+weak ref + 2 shared ref.\n";
    delete wsp;
    cout << "-weak shared.\n";
    cout << "weak ptr:" << wp->get() << endl;
    cout << "shared ptr:" << wsp2->get() << endl;
    sp = new rc_ptr<Toto>( obj );
    cout << "+strong.\n";
    ASSERT( !obj->tryDelete() );
    // cout << "deletion forced.\n";
    cout << "weak ptr:" << wp->get() << endl;
    cout << "shared ptr:" << wsp2->get() << endl;
    delete wsp2;
    cout << "-shared.\n";
    delete sp;
    cout << "-strong. obj should be destroyed.\n";
    cout << "weak ptr:" << wp->get() << endl;
    delete wp;
    cout << "-weak\n";

    cout << "------\n";
    obj = new Toto;
    wsp2 = new weak_shared_ptr<Toto>( obj );
    cout << "+weak_shared\n";
    ASSERT( obj->tryDelete() );
    cout << "tryDelete done - obj should be destroyed.\n";
    cout << "weak_shared ptr (sould be null): " << wsp2->get() << endl;
    ASSERT( !wsp2->get() );
    delete wsp2;
    cout << "-weak_shared\n";
  }

  cout << "---- / redoing all tests using the generic shared_ptr / ----\n";
  {
    Toto  *obj = new Toto;
    shared_ptr<Toto> *sp = new shared_ptr<Toto>( shared_ptr<Toto>::Strong,
        obj );
    cout << "strong ref. testDeletable: " << obj->testDeletable() << endl;
    ASSERT( !obj->testDeletable() );
    shared_ptr<Toto> *wsp = new shared_ptr<Toto>( shared_ptr<Toto>::WeakShared,
        obj );
    cout << "+weak shared ref. testDeletable: " << obj->testDeletable()
        << endl;
    ASSERT( !obj->testDeletable() );
    delete sp;
    cout << "-strong ref. testDeletable: " << obj->testDeletable() << endl;
    ASSERT( obj->testDeletable() );
    delete wsp;
    cout << "-weak shared ref. obj should be destroyed." << endl;

    cout << "------\n";
    obj = new Toto;
    shared_ptr<Toto> *wp = new shared_ptr<Toto>( shared_ptr<Toto>::Weak, obj );
    cout << "weak ref.\n";
    delete wp;
    cout << "-weak ref. obj should not be destroyed.\n";
    wp = new shared_ptr<Toto>( shared_ptr<Toto>::Weak, obj );
    wsp = new shared_ptr<Toto>( shared_ptr<Toto>::WeakShared, obj );
    shared_ptr<Toto> *wsp2
        = new shared_ptr<Toto>( shared_ptr<Toto>::WeakShared, obj );
    cout << "+weak ref + 2 shared ref.\n";
    delete wsp;
    cout << "-weak shared.\n";
    cout << "weak ptr:" << wp->get() << endl;
    cout << "shared ptr:" << wsp2->get() << endl;
    sp = new shared_ptr<Toto>( shared_ptr<Toto>::Strong, obj );
    cout << "+strong.\n";
    ASSERT( !obj->tryDelete() );
    // cout << "deletion forced.\n";
    cout << "weak ptr:" << wp->get() << endl;
    cout << "shared ptr:" << wsp2->get() << endl;
    delete wsp2;
    cout << "-shared.\n";
    delete sp;
    cout << "-strong. obj should be destroyed.\n";
    cout << "weak ptr:" << wp->get() << endl;
    delete wp;
    cout << "-weak\n";

    cout << "------\n";
    obj = new Toto;
    wsp2 = new shared_ptr<Toto>( shared_ptr<Toto>::WeakShared, obj );
    cout << "+weak_shared\n";
    ASSERT( obj->tryDelete() );
    cout << "tryDelete done - obj should be destroyed.\n";
    cout << "weak_shared ptr (sould be null): " << wsp2->get() << endl;
    ASSERT( !wsp2->get() );
    delete wsp2;
    cout << "-weak_shared\n";
  }

  cout << "---- / shared_ptr copy/reset tests / ----\n";
  {
    Toto *obj = new Toto;
    shared_ptr<Toto> sp( shared_ptr<Toto>::WeakShared, obj );
    shared_ptr<Toto> sp2 = sp;
    cout << "weak_shared, 2 refs allocated: " << rc_ptr_trick::refCount( sp2 )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp2 ) << endl;
    sp2.reset( shared_ptr<Toto>::Strong, obj );
    cout << "muted to rc_ptr, refs: " << rc_ptr_trick::refCount( sp2 )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp2 ) << endl;
    sp2.reset( shared_ptr<Toto>::WeakShared, obj );
    cout << "muted to weak_shared_ptr, refs: " << rc_ptr_trick::refCount( sp2 )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp2 ) << endl;
    sp2.reset( shared_ptr<Toto>::Weak, obj );
    cout << "muted to weak_ptr: " << sp2.get() << ", refs: "
        << rc_ptr_trick::refCount( *obj )
        << ", weak_count: " << rc_ptr_trick::weakCount( *obj ) << endl;
    sp2.reset( shared_ptr<Toto>::Strong, obj );
    cout << "muted to rc_ptr, refs: " << rc_ptr_trick::refCount( sp2 )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp2 ) << endl;
    sp2.reset( shared_ptr<Toto>::Weak, obj );
    cout << "muted to weak_ptr: " << sp2.get() << ", refs: "
        << rc_ptr_trick::refCount( sp2 )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp2 ) << endl;
    sp2.reset( shared_ptr<Toto>::Strong, obj );
    cout << "muted to rc_ptr, refs: " << rc_ptr_trick::refCount( sp2 )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp2 ) << endl;
    sp.reset( shared_ptr<Toto>::Strong, 0 );
    cout << "removed a weak_shared, refs: " << rc_ptr_trick::refCount( sp2 )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp2 ) << endl;
    sp2.reset( shared_ptr<Toto>::Weak, obj );
    cout << "muted to weak_ptr: " << sp2.get() << " (should be destroyed)\n";

    cout << "------\n";
    obj = new Toto;
    sp.reset( shared_ptr<Toto>::WeakShared, obj );
    cout << "new weak_shared: " << rc_ptr_trick::refCount( sp )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp ) << endl;
    list<shared_ptr<Toto> >  l;
    l.push_back( sp );
    cout << "list (1 weak_shared), refs: " << rc_ptr_trick::refCount( sp )
        << ", weak_count: " << rc_ptr_trick::weakCount( sp ) << endl;
    cout << "1st pointer: " << l.begin()->get() << endl;
    sp.reset( shared_ptr<Toto>::Weak, 0 );
    cout << "-weak_shared: " << rc_ptr_trick::refCount( *l.begin() )
        << ", weak_count: " << rc_ptr_trick::weakCount( *l.begin() ) << endl;
  }

  return 0;
}

