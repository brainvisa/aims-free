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

#include <aims/transformation/transformationgraph3d.h>
#include <aims/transformation/transformation_chain.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/reader.h>
#include <graph/graph/dedge.h>
#include <cartobase/uuid/uuid.h>

using namespace aims;
using namespace carto;
using namespace soma;
using namespace std;


namespace
{

  class TransformationGFactory;

  class ReferentialVertex : public Vertex
  {
  public:
    ReferentialVertex( const string & s ) : Vertex( s )
    {
      UUID uuid;
      uuid.generate();
      setProperty( "uuid", uuid.toString() );
    }

    virtual ~ReferentialVertex() {}

  private:
    friend class TransformationGFactory;
  };


  class TransformationEdge : public DEdge
  {
  public:
    TransformationEdge( const string & s )
      : DEdge( s ), transformation( 0 )
    {
      PropertySet & self = getValue();
      self.addBuiltinProperty( "transformation", transformation );
      UUID uuid;
      uuid.generate();
      self.setProperty( "uuid", uuid.toString() );
    }

    virtual ~TransformationEdge() {}

    rc_ptr<Transformation3d> transformation;

  private:
    friend class TransformationGFactory;
  };


  class TransformationGFactory : public GraphFactory
  {
  public:
    TransformationGFactory() : GraphFactory()
    {
      registerGenerator( "referential", buildReferential );
      registerGenerator( "transformation", buildTransformation );
    }

    static GenericObject *buildReferential( const string & s )
    {
      Vertex *v = new ReferentialVertex( s );
      return v;
    }

    static GenericObject *buildTransformation( const string & s )
    {
      Edge *e = new TransformationEdge( s );
      return e;
    }

  };

}


TransformationGraph3d::TransformationGraph3d()
  : Graph( rc_ptr<GraphFactory>( new TransformationGFactory),
           "transformations" )
{
}


TransformationGraph3d::~TransformationGraph3d()
{
}


void TransformationGraph3d::updateIds()
{
  _refs_by_id.clear();
  _tr_by_id.clear();

  iterator i, e = end();
  for( i=begin(); i!=e; ++i )
  {
    Vertex *v = *i;
    if( v->hasProperty( "uuid" ) )
    {
      std::string id = v->getProperty( "uuid" )->getString();
      _refs_by_id[ id ] = v;
    }
  }

  const set<Edge *> & edges = this->edges();
  set<Edge *>::const_iterator ie, ee = edges.end();
  for( ie=edges.begin(); ie!=ee; ++ie )
  {
    Edge *t = *ie;
    if( t->hasProperty( "uuid" ) )
    {
      std::string id = t->getProperty( "uuid" )->getString();
      _tr_by_id[ id ] = t;
    }
  }
}


Vertex* TransformationGraph3d::referentialById( const std::string & id ) const
{
  map<string, Vertex *>::const_iterator i = _refs_by_id.find( id );
  if( i != _refs_by_id.end() )
    return i->second;
  return 0;
}


Edge* TransformationGraph3d::transformationById( const std::string & id ) const
{
  map<string, Edge *>::const_iterator i = _tr_by_id.find( id );
  if( i != _tr_by_id.end() )
    return i->second;
  return 0;
}


rc_ptr<Transformation3d>
TransformationGraph3d::transformation( const Edge *edge )
{
  const TransformationEdge *tedge
    = dynamic_cast<const TransformationEdge *>( edge );
  if( tedge )
    return tedge->transformation;
  rc_ptr<Transformation3d> trans( 0 );
  try
  {
    edge->getProperty( "transformation", trans );
  }
  catch( ... )
  {
  }
  return trans;
}


rc_ptr<Transformation3d>
TransformationGraph3d::transformation( const std::string & id ) const
{
  Edge* edge = transformationById( id );
  if( edge )
    return transformation( edge );

  return rc_ptr<Transformation3d>( 0 );
}


Edge* TransformationGraph3d::getTransformation(
  const std::string & src_ref, const std::string dst_ref ) const
{
  Vertex *v1, *v2;
  v1 = referentialById( src_ref );
  if( !v1 )
    return 0;
  v2 = referentialById( dst_ref );
  if( !v2 )
    return 0;
  return getTransformation( v1, v2 );
}


Edge* TransformationGraph3d::getTransformation( const Vertex *src_ref,
                                                const Vertex *dst_ref ) const
{
  Vertex::const_iterator ie, ee = src_ref->end();
  Edge *edge;
  for( ie=src_ref->begin(); ie!=ee; ++ie )
  {
    edge = *ie;
    if( edge->isDirected()
        && *edge->begin() == src_ref && *edge->rbegin() == dst_ref )
    {
      loadTransformation( edge );
      return edge;
    }
  }
  return 0;
}


Edge* TransformationGraph3d::getTransformation(
  const std::string & src_ref, const std::string dst_ref, bool allow_compose )
{
  Vertex *v1, *v2;
  v1 = referentialById( src_ref );
  if( !v1 )
    return 0;
  v2 = referentialById( dst_ref );
  if( !v2 )
    return 0;
  return getTransformation( v1, v2, allow_compose );
}


Edge* TransformationGraph3d::getTransformation(
  Vertex *src_ref, Vertex *dst_ref, bool allow_compose )
{
  Edge *edge = getTransformation( src_ref, dst_ref );

  if( !allow_compose || edge )
    return edge;

  if( _disconnected.find( make_pair( src_ref, dst_ref ) )
      != _disconnected.end() )
    // disconnected path: no transform
    return 0;

  // compose and register (even if the chain is empty to avoid vainly
  // re-searching for the same chain next time)
  rc_ptr<Transformation3d> chain = getTransformChain( src_ref, dst_ref );
  return registerTransformation( src_ref, dst_ref, chain, true );
}


rc_ptr<Transformation3d>
TransformationGraph3d::loadTransformation( Edge *edge, bool affine_only ) const
{
  rc_ptr<Transformation3d> trans( 0 );
  TransformationEdge* tedge = dynamic_cast<TransformationEdge *>( edge );
  if( tedge )
    trans = tedge->transformation;
  if( tedge && !trans )
  {
    // lazy load from filename if one is available
    try
    {
      string filename = tedge->getProperty( "filename" )->getString();
      // cout << "load transformation\n";
      if( filename.empty() )
      {
        // cout << "ID\n";
        trans.reset( new AffineTransformation3d );
        tedge->transformation = trans;
      }
      else
      {
        if( affine_only )
        {
          // cout << "from file: " << filename << endl;
          Reader<AffineTransformation3d> r( filename );
          trans.reset( r.read() );
          // cout << "loaded affine transform: " << filename << endl;
        }
        else
        {
          // cout << "from file: " << filename << endl;
          Reader<Transformation3d> r( filename );
          trans.reset( r.read() );
          // cout << "loaded transform: " << filename << endl;
        }
        tedge->transformation = trans;
        if( trans && trans->header()->hasProperty( "uuid" ) )
        {
          // change edge uuid
          string old_uuid = edge->getProperty( "uuid" )->getString();
          _tr_by_id.erase( old_uuid );
          edge->setProperty( "uuid", trans->header()->getProperty( "uuid" ) );
          string new_uuid
            = trans->header()->getProperty( "uuid" )->getString();
          _tr_by_id[ new_uuid ] = edge;
        }
      }
    }
    catch( ... )
    {
    }
  }
  return trans;
}


rc_ptr<Transformation3d> TransformationGraph3d::getTransformChain(
  const std::string & src_ref, const std::string & dst_ref ) const
{
  Vertex *v1, *v2;
  v1 = referentialById( src_ref );
  if( !v1 )
    return rc_ptr<Transformation3d>( new TransformationChain3d );
  v2 = referentialById( dst_ref );
  if( !v2 )
    return rc_ptr<Transformation3d>( new TransformationChain3d );
  return getTransformChain( v1, v2 );
}


rc_ptr<Transformation3d> TransformationGraph3d::getTransformChain(
  const Vertex *src_ref, const Vertex *dst_ref ) const
{
  Edge *edge = getTransformation( src_ref, dst_ref );
  if( edge )
  {
    TransformationEdge *tedge = dynamic_cast<TransformationEdge*>( edge );
    if( tedge )
      return tedge->transformation;
    try
    {
      rc_ptr<Transformation3d> tr;
      if( edge->getProperty( "transformation", tr ) )
        return tr;
    }
    catch( ... )
    {
    }
    return rc_ptr<Transformation3d>( 0 );
  }

  list<Edge *> chain = GraphManip::getPath( src_ref, dst_ref );

  if( chain.empty() )
    return rc_ptr<Transformation3d>( 0 );

  TransformationChain3d tchain;
  list<Edge *>::iterator ie, ee = chain.end();

  for( ie=chain.begin(); ie!=ee; ++ie )
  {
    TransformationEdge *tedge = dynamic_cast<TransformationEdge*>( *ie );
    rc_ptr<Transformation3d> trans;
    if( tedge )
      trans = tedge->transformation;
    else
    {
      if( !(*ie)->getProperty( "transformation", trans ) )
        throw runtime_error( "Edge contains no transformation" );
    }
    if( !trans )
      trans = loadTransformation( *ie );
    tchain.push_back( trans );
  }

  const_ref<Transformation3d> simple = tchain.simplify();
  rc_ptr<Transformation3d>
    rtrans( const_cast<Transformation3d *>( simple.pointer() ) );
  return rtrans;
}


Edge * TransformationGraph3d::registerTransformation(
  const std::string & src_ref, const std::string & dst_ref,
  rc_ptr<Transformation3d> trans, bool deduced )
{
  Vertex *v1, *v2;
  v1 = referentialById( src_ref );
  if( !v1 )
  {
    v1 = addVertex( "referential" );
    v1->setProperty( "uuid", src_ref );
    _refs_by_id[ src_ref ] = v1;
  }
  v2 = referentialById( dst_ref );
  if( !v2 )
  {
    v2 = addVertex( "referential" );
    v2->setProperty( "uuid", dst_ref );
    _refs_by_id[ dst_ref ] = v2;
  }

  return registerTransformation( v1, v2, trans, deduced );
}


Edge * TransformationGraph3d::registerTransformation(
  Vertex *src_ref, Vertex *dst_ref, rc_ptr<Transformation3d> trans,
  bool deduced )
{
  if( !trans && deduced )
  {
    // no transform: don't make a new Edge but mark this path as empty
    _disconnected.insert( make_pair( src_ref, dst_ref ) );
    return 0;
  }

  Edge *edge = addDirectedEdge( src_ref, dst_ref, "transformation" );
  edge->setProperty( "transformation", trans );
  if( trans.get() )
  {
    if( trans->header()->hasProperty( "uuid" ) )
    {
      string old_uuid = edge->getProperty( "uuid" )->getString();
      _tr_by_id.erase( old_uuid );
      edge->setProperty( "uuid", trans->header()->getProperty( "uuid" ) );
      string new_uuid = trans->header()->getProperty( "uuid" )->getString();
      _tr_by_id[ new_uuid ] = edge;
    }
    else
      trans->header()->setProperty( "uuid", edge->getProperty( "uuid" ) );
  }
  if( deduced )
    edge->setProperty( "deduced", true );
  return edge;
}


Edge * TransformationGraph3d::registerTransformation(
  const std::string & src_ref, const std::string & dst_ref,
  const string & filename )
{
  Vertex *v1, *v2;
  v1 = referentialById( src_ref );
  if( !v1 )
  {
    v1 = addVertex( "referential" );
    v1->setProperty( "uuid", src_ref );
    _refs_by_id[ src_ref ] = v1;
  }
  v2 = referentialById( dst_ref );
  if( !v2 )
  {
    v2 = addVertex( "referential" );
    v2->setProperty( "uuid", dst_ref );
    _refs_by_id[ dst_ref ] = v2;
  }

  return registerTransformation( v1, v2, filename );
}


Edge * TransformationGraph3d::registerTransformation(
  Vertex *src_ref, Vertex *dst_ref, const string & filename )
{
  Edge *edge = addDirectedEdge( src_ref, dst_ref, "transformation" );
  edge->setProperty( "filename", filename );
  return edge;
}


void TransformationGraph3d::loadTransformationsGraph( Object desc,
                                                      const string & dirname )
{
  string sep;
  if( !dirname.empty() )
  {
    string last = dirname.substr( dirname.length() - 2, 1 );
    if( last != "/" && last != "\\" )
      sep = "/";
  }

  Object src_it;
  for( src_it=desc->objectIterator(); src_it && src_it->isValid();
       src_it->next() )
  {
    string source_id = src_it->key();
    Object dst_def = src_it->currentValue();

    Object dst_it;
    for( dst_it=dst_def->objectIterator(); dst_it && dst_it->isValid();
         dst_it->next() )
    {
      string dest_id = dst_it->key();
      string trans = dst_it->currentValue()->getString();
      // cout << source_id << " -> " << dest_id << ": " << trans << endl;
      bool inv = false;
      string sr = source_id;
      if( trans.compare( 0, 4, "inv:" ) == 0 )
      {
        inv = true;
        trans = trans.substr( 4, trans.length() - 4 );
        sr = dest_id;
        dest_id = source_id;
      }
      if( !trans.empty() )
        trans = dirname + sep + trans;

      registerTransformation( sr, dest_id, trans );
      // TODO: we could unregister any inverse trans here
      // then clear only chained transforms cache at the end of this
      // function.
    }
  }
  clearCache();
}


void TransformationGraph3d::clearCache( bool chain_only )
{
  _disconnected.clear();

  // clear deduced transforms
  ESet::iterator ie = edges().begin(), ee = edges().end();
  while( ie != ee )
  {
    if( (*ie)->hasProperty( "deduced" ) )
    {
      try
      {
        bool deduced = bool( (*ie)->getProperty( "deduced" )->getScalar() );
        if( deduced )
        {
          bool remove = true;
          if( chain_only )
          {
            Edge *inv = getTransformation( *(*ie)->rbegin(), *(*ie)->begin() );
            if( inv
                && ( !inv->hasProperty( "deduced" )
                     || !bool( inv->getProperty( "deduced" )->getScalar() ) ) )
              // inverse is not a deduced transform: it's not a chain
              remove = false;
          }
          if( remove )
          {
            ESet::iterator j = ie;
            ++ie;
            removeEdge( *j );
            continue;
          }
        }
      }
      catch( ... )
      {
      }
    }

    ++ie;
  }

  updateIds();
}


void TransformationGraph3d::registerInverseTransformations( bool loadAffines )
{
  set<pair<Vertex*, Vertex*> > done;
  ESet::iterator ie = edges().begin(), ee = edges().end();
  bool modified = false;

  // clear only non-direct deduced transforms (chains)
  clearCache( true );

  if( loadAffines )
    loadAffineTransformations();

  while( ie != ee )
  {
    Edge *edge = *ie;
    Vertex *src = *edge->begin();
    Vertex *dst = *edge->rbegin();

    if( done.find( make_pair( src, dst ) ) != done.end() )
    {
      ++ie;
      continue;
    }
    done.insert( make_pair( src, dst ) );
    done.insert( make_pair( dst, src ) );

    rc_ptr<Transformation3d> trans = transformation( edge );
    if( !trans || !trans->invertible() )
    {
      ++ie;
      continue;
    }

    Edge *inv_edge = getTransformation( dst, src );
    if( inv_edge )
    {
      ++ie;
      continue;
    }

    unique_ptr<Transformation3d> inv_trans_u = trans->getInverse();
    if( inv_trans_u )
    {
      ++ie;

      rc_ptr<Transformation3d> inv_trans( inv_trans_u.release() );
      registerTransformation( dst, src, inv_trans, true );
      modified = true;
      continue;
    }

    ++ie;
  }

  if( modified )
    updateIds();
}


void TransformationGraph3d::loadAffineTransformations()
{
  ESet::iterator ie, ee = edges().end();

  for( ie=edges().begin(); ie!=ee; ++ie )
    loadTransformation( *ie, true );
}


#include <cartobase/object/object_d.h>

namespace carto
{
  INSTANTIATE_GENERIC_OBJECT_TYPE( carto::rc_ptr<soma::Transformation3d> )
}

