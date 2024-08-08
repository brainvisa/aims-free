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
#include <aims/resampling/standardreferentials.h>
#include <graph/graph/dedge.h>
#include <cartobase/uuid/uuid.h>
#include <cartobase/stream/fileutil.h>
//--- boost ------------------------------------------------------------------
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

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
      UUID new_uuid;
      new_uuid.generate();
      uuid = new_uuid.toString();
      PropertySet & self = getValue();
      self.addBuiltinProperty( "uuid", uuid );
    }

    virtual ~ReferentialVertex() {}

    string uuid;

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

  if( id == "mni" || id == "mni152" || id == "NIFTI_XFORM_MNI_152"
      || id == StandardReferentials::mniTemplateReferential() )
  {
    i = _refs_by_id.find( StandardReferentials::mniTemplateReferentialID() );
    if( i != _refs_by_id.end() )
      return i->second;
    i = _refs_by_id.find( StandardReferentials::mniTemplateReferential() );
    if( i != _refs_by_id.end() )
      return i->second;
  }

  if( id == "acpc" || id == "talairach" || id == "NIFTI_XFORM_TALAIRACH"
      || id == StandardReferentials::acPcReferential() )
  {
    i = _refs_by_id.find( StandardReferentials::acPcReferentialID() );
    if( i != _refs_by_id.end() )
      return i->second;
    i = _refs_by_id.find( StandardReferentials::acPcReferential() );
    if( i != _refs_by_id.end() )
      return i->second;
  }

  return 0;
}


Edge* TransformationGraph3d::transformationById( const std::string & id ) const
{
  map<string, Edge *>::const_iterator i = _tr_by_id.find( id );
  if( i != _tr_by_id.end() )
    return i->second;
  return 0;
}


string TransformationGraph3d::referential( const Vertex *vertex )
{
  const ReferentialVertex *rvert
    = dynamic_cast<const ReferentialVertex *>( vertex );
  if( rvert )
    return rvert->uuid;
  string ref;
  try
  {
    ref = vertex->getProperty( "uuid" )->getString();
  }
  catch( ... )
  {
  }
  return ref;
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


Edge* TransformationGraph3d::getTransformation_raw(
  const Vertex *src_ref, const Vertex *dst_ref ) const
{
  Vertex::const_iterator ie, ee = src_ref->end();
  Edge *edge;
  for( ie=src_ref->begin(); ie!=ee; ++ie )
  {
    edge = *ie;
    if( edge->isDirected()
        && *edge->begin() == src_ref && *edge->rbegin() == dst_ref )
      return edge;
  }
  return 0;
}


Edge* TransformationGraph3d::getTransformation_raw(
  const std::string & src_ref, const std::string & dst_ref ) const
{
  Vertex *v1, *v2;
  v1 = referentialById( src_ref );
  if( !v1 )
    return 0;
  v2 = referentialById( dst_ref );
  if( !v2 )
    return 0;
  return getTransformation_raw( v1, v2 );
}


Edge* TransformationGraph3d::getTransformation(
  const std::string & src_ref, const std::string & dst_ref ) const
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
  Edge *edge = getTransformation_raw( src_ref, dst_ref );
  if( edge )
    loadTransformation( edge );
  return edge;
}


Edge* TransformationGraph3d::getTransformation(
  const std::string & src_ref, const std::string & dst_ref,
  bool allow_compose )
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

  Edge *edge = getTransformation_raw( src_ref, dst_ref );
  if( edge )
    removeEdge( edge );
  edge = addDirectedEdge( src_ref, dst_ref, "transformation" );
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
  Edge *edge = getTransformation_raw( src_ref, dst_ref );
  if( edge )
    removeEdge( edge );
  edge = addDirectedEdge( src_ref, dst_ref, "transformation" );
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
      string trans;
      try
      {
        trans = dst_it->currentValue()->getString();
      }
      catch( ... )
      {
        try
        {
          // affine with header dict
          Object amat = dst_it->currentValue()->getProperty( "affine" );

          rc_ptr<Transformation3d> aff(
            new AffineTransformation3d( amat ) );
          if( aff )
          {
            try
            {
              Object ahdr = dst_it->currentValue()->getProperty( "header" );
              if( ahdr )
                aff->header()->copyProperties( ahdr );
            }
            catch( ... )
            {
            }

            registerTransformation( source_id, dest_id, aff );
          }
          continue;
        }
        catch( ... )
        {
          // vector matrix for affine
          rc_ptr<Transformation3d> aff(
            new AffineTransformation3d( dst_it->currentValue() ) );
          if( aff )
            registerTransformation( source_id, dest_id, aff );
        }
        continue;
      }
      // cout << source_id << " -> " << dest_id << ": " << trans << endl;
      bool inv = false;
      if( trans.compare( 0, 4, "inv:" ) == 0 )
      {
        inv = true;
        trans = trans.substr( 4, trans.length() - 4 );
      }
      if( !trans.empty() )
      {
        trans = dirname + sep + trans;
        if( inv )
          trans += "?inv=1";
      }

      registerTransformation( source_id, dest_id, trans );
      // TODO: we could unregister any inverse trans here
      // then clear only chained transforms cache at the end of this
      // function.
    }
  }
  clearCache();
}


Object TransformationGraph3d::asDict( bool affine_only, bool allow_read,
                                      bool embed_affines ) const
{
  Object dict = Object::value( Dictionary() );
  Vertex::const_iterator ie, ee = edges().end();

  if( ( affine_only || embed_affines ) && allow_read )
    const_cast<TransformationGraph3d *>( this )->loadAffineTransformations();

  for( ie=edges().begin(); ie!=ee; ++ie )
  {
    Edge *e = *ie;
    if( e->hasProperty( "deduced" )
        && bool( e->getProperty( "deduced" )->getScalar() ) )
      continue;
    rc_ptr<Transformation3d> tr = transformation( e );
    if( affine_only && !tr )
      continue; // not a loaded affine

    string uuid = e->getProperty( "uuid" )->getString();
    Vertex *srcv = *e->begin();
    Vertex *dstv = *e->rbegin();
    string suid = referential( srcv );
    string duid = referential( dstv );
    Object sdict;
    if( dict->hasProperty( suid ) )
      sdict = dict->getProperty( suid );
    else
    {
      sdict = Object::value( Dictionary() );
      dict->setProperty( suid, sdict );
    }
    if( embed_affines && tr
        && dynamic_cast<AffineTransformation3d *>( tr.get() ) )
    {
      AffineTransformation3d *aff
        = dynamic_cast<AffineTransformation3d *>( tr.get() );
      Object header = Object::value( Dictionary() );
      header->copyProperties( aff->header() );
      // remove src/dst from header as it is already in the graph
      if( header->hasProperty( "source_referential" ) )
        header->removeProperty( "source_referential" );
      if( header->hasProperty( "destination_referential" ) )
        header->removeProperty( "destination_referential" );
      if( header->hasProperty( "object_type" ) )
        header->removeProperty( "object_type" );
      if( header->hasProperty( "data_type" ) )
        header->removeProperty( "data_type" );
      if( header->hasProperty( "format" ) )
        header->removeProperty( "format" );
      if( header->size() != 0 )
      {
        Object atr = Object::value( Dictionary() );
        atr->setProperty( "header", header );
        atr->setProperty( "affine", aff->toVector() );
        sdict->setProperty( duid, atr );
      }
      else  // no header left
        sdict->setProperty( duid, aff->toVector() );
    }
    else
    {
      string filename;
      if( e->hasProperty( "filename" ) )
      {
        filename
          = FileUtil::basename( e->getProperty( "filename" )->getString() );
        size_t p = filename.rfind( '?' );
        if( p != string::npos )
        {
          filename.clear();
  //         if( filename.find( "?inv=1", p ) != string::npos
  //             || filename.find( "&inv=1", p+1 ) != string::npos )
  //           filename.clear();
        }
      }
      if( filename.empty() )
        filename = suid + "_TO_" + duid + ".trm";
      sdict->setProperty( duid, filename );
    }
  }

  return dict;
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

    unique_ptr<Transformation> inv_trans_u = trans->getInverse();
    if( inv_trans_u )
    {
      ++ie;

      Transformation3d * t3
        = dynamic_cast<Transformation3d *>( inv_trans_u.release() );
      if( t3 )
      {
        rc_ptr<Transformation3d> inv_trans( t3 );
        registerTransformation( dst, src, inv_trans, true );
        modified = true;
      }
      continue;
    }

    ++ie;
  }

  if( modified )
    updateIds();
}


list<Edge *> TransformationGraph3d::loadAffineTransformations()
{
  list<Edge *> loaded;
  ESet::iterator ie, ee = edges().end();

  for( ie=edges().begin(); ie!=ee; ++ie )
  {
    if( !transformation( *ie ) )
    {
      rc_ptr<Transformation3d> tr = loadTransformation( *ie, true );
      if( tr )
        loaded.push_back( *ie );
    }
  }

  return loaded;
}


vector<string> TransformationGraph3d::updateFromObjectHeader(
  carto::Object header, bool includeDiskSpace )
{
  const DictionaryInterface *dh
    = header ? header->getInterface<DictionaryInterface>() : 0;
  return updateFromObjectHeader( dh, includeDiskSpace );
}


vector<string> TransformationGraph3d::updateFromObjectHeader(
  const carto::DictionaryInterface *header, bool includeDiskSpace )
{
  bool changed = false;
  string ref;
  if( header )
    try
    {
      ref = header->getProperty( "referential" )->getString();
    }
    catch( ... )
    {
    }
  if( ref.empty() )
  {
    UUID uuid = UUID();
    uuid.generate();
    ref = uuid.toString();
  }

  Vertex *v = referentialById( ref );
  if( !v )
  {
    v = addVertex( "referential" );
    v->setProperty( "uuid", ref );
    _refs_by_id[ ref ] = v;
  }

  if( includeDiskSpace )
  {
    rc_ptr<Transformation3d> a2s = transformToDiskSpace( header );
    if( a2s )
    {
      string diskref = "disk_space_" + ref;
      Vertex *dv = referentialById( diskref );
      if( !dv )
      {
        dv = addVertex( "referential" );
        dv->setProperty( "uuid", diskref );
        _refs_by_id[ diskref ] = dv;
      }
      registerTransformation( v, dv, a2s );
      changed = true;
    }
  }

  Object refs;
  if( header )
    try
    {
      refs = header->getProperty( "referentials" );
    }
    catch( ... )
    {
    }

  Object trans;
  if( header )
    try
    {
      trans = header->getProperty( "transformations" );
    }
    catch( ... )
    {
    }

  vector<string> trefs( 1 );
  trefs[0] = ref;

  if( !trans.isNull() && !refs.isNull() )
  {
    int i;
    trefs.reserve( std::min( trans->size(), refs->size() ) + 1 );
    Object it = trans->objectIterator(), ir = refs->objectIterator();
    for( i=0; it->isValid() && ir->isValid(); it->next(), ir->next(), ++i )
    {
      string dref;
      try
      {
        dref = ir->currentValue()->getString();
      }
      catch( ... )
      {
        cerr << "referential num. " << i << " could not be parsed.\n";
        trefs.push_back( "" );
        continue;
      }

      Object ot = it->currentValue();
      string dref_id;

      try
      {
        AffineTransformation3d *tr = new AffineTransformation3d( ot );
        dref_id = StandardReferentials::referentialID(
          dref, false, false, string( "_" ) + ref );
        registerTransformation( ref, dref_id, rc_ptr<Transformation3d>( tr ) );
        changed = true;
      }
      catch( ... )
      {
        cerr << "transformation num. " << i << " could not be parsed.\n";
        dref_id = dref;
      }

      trefs.push_back( dref_id );
    }
  }

  if( changed )
  {
    clearCache();
    registerInverseTransformations();
  }

  return trefs;
}


Vertex* TransformationGraph3d::referentialByCode(
  const string & id, Object header, const vector<string> & refs ) const
{
  return referentialByCode( id, header.get(), refs );
}


Vertex* TransformationGraph3d::referentialByCode(
  const string & id, const DictionaryInterface *header,
  const vector<string> & refs ) const
{
  // boost::iequals is used for case-insensitive comparison
  using boost::iequals;

  Vertex *v = referentialById( id );
  if( v )
    return v;

  string ref = refs[0];  // ref[0] is the object AIMS ref

  if( iequals( id, "AIMS" ) || id.empty() )
    return referentialById( ref );

  if( iequals( id, "disk" ) )
  {
    string diskref = "disk_space_" + ref;
    return referentialById( diskref );
  }

  int transform_position = -1;

  if( iequals( id, "first" ) )
    transform_position = 0;
  else if( iequals( id, "last" )
           || iequals( id, "auto" ) )
    transform_position = refs.size() - 2;
  else if( iequals( id, "qform" )
           || iequals( id, "ITK" )
           || iequals( id, "ANTS" ) )
  {
    // TODO determine transform_position
    throw runtime_error("referential ID as qform not implemented yet");
  }
  else if( iequals( id, "sform" ) )
  {
    // TODO determine transform_position
    throw runtime_error("referential ID as sform not implemented yet");
  }
  else
  {
    try
    {
      transform_position = boost::lexical_cast<int>( id );
    }
    catch( const boost::bad_lexical_cast & )
    {
    }
  }

  string referential_name, referential_id;

  if( transform_position >= 0 )
  {
    if( !( transform_position < refs.size() - 1 ) )
    {
      stringstream error_message;
      error_message
        << "referential ID as number cannot use transform in position "
        << transform_position
        << " (0-based), the header contains only "
        << refs.size() - 1 << " transformations.";
      throw runtime_error( error_message.str() );
    }
    referential_id = refs[transform_position + 1];
  }
  else
  {
    if( iequals( id, "scanner" )
        || iequals( id, "NIFTI_XFORM_SCANNER_ANAT" ) )
    {
      referential_name
        = StandardReferentials::commonScannerBasedReferential() + "_" + ref;
      referential_id = referential_name;
    }
    else if( iequals( id, "aligned" )
             || iequals( id, "NIFTI_XFORM_ALIGNED_ANAT" ) )
    {
      referential_name
        = "Coordinates aligned to another file or to anatomical truth";
    }
    else if( iequals( id, "NIFTI_XFORM_TEMPLATE_OTHER" ) )
    {
      referential_name = "Other template";
    }
    else
      return 0;  // we have already looked for id
  }

  if( !referential_name.empty() )
  {
    // look for this name in the header referenatials, and use the same
    // index in refs
    if( !header )
      throw runtime_error(
        "No header provided, and we need one to parse referentials" );
    Object hrefs;
    try
    {
      hrefs = header->getProperty( "referentials" );
    }
    catch( ... )
    {
      throw runtime_error( "The header has no referentials information" );
    }

    Object it;
    int i = 1;
    for( it=hrefs->objectIterator(); it->isValid(); it->next(), ++i )
    {
      string rname;
      try
      {
        rname = it->currentValue()->getString();
      }
      catch( ... )
      {
        continue;
      }
      if( rname == referential_name )
      {
        if( refs.size() > i )
          referential_id = refs[i];
        break;
      }
    }
  }

  return referentialById( referential_id );
}


rc_ptr<soma::Transformation3d> TransformationGraph3d::transformToDiskSpace(
  const carto::DictionaryInterface *ph )
{
  rc_ptr<Transformation3d> tr;

  try
  {
    Object s2mo = ph->getProperty( "storage_to_memory" );
    AffineTransformation3d s2m_vox( s2mo );
    unique_ptr<AffineTransformation3d> m2s_vox = s2m_vox.inverse();
    Point3df vs( 1., 1., 1. );
    try
    {
      Object vso = ph->getProperty( "voxel_size" );
      vs[0] = vso->getArrayItem( 0 )->getScalar();
      vs[1] = vso->getArrayItem( 1 )->getScalar();
      vs[2] = vso->getArrayItem( 2 )->getScalar();
    }
    catch( runtime_error & )
    {
    }
    // get s2m in mm
    AffineTransformation3d tvs1;
    tvs1.affine()( 0, 0 ) = 1. / vs[0];
    tvs1.affine()( 1, 1 ) = 1. / vs[1];
    tvs1.affine()( 2, 2 ) = 1. / vs[2];

    Point3df vss = m2s_vox->transformVector( vs );  // vs in disk orientation
    AffineTransformation3d tvs2;
    tvs2.affine()( 0, 0 ) = fabs( vss[0] );
    tvs2.affine()( 1, 1 ) = fabs( vss[1] );
    tvs2.affine()( 2, 2 ) = fabs( vss[2] );

    AffineTransformation3d *m2s
      = new AffineTransformation3d( tvs2 * *m2s_vox * tvs1 );
    tr.reset( m2s );
  }
  catch( runtime_error & )
  {
  }

  return tr;
}



#include <cartobase/object/object_d.h>

namespace carto
{
  INSTANTIATE_GENERIC_OBJECT_TYPE( carto::rc_ptr<soma::Transformation3d> )
}

