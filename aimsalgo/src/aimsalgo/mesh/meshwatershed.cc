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

#include <aims/mesh/meshwatershed.h>

using namespace aims;
using namespace std;

namespace aims
{

  void distancesFromMesh( const AimsSurfaceTriangle & mesh,
                          vector<AimsVector<uint,2> > & edges,
                          vector<double> & weights )
  {
    const vector<Point3df> & vert = mesh.vertex();
    const vector<AimsVector<uint,3> > & poly = mesh.polygon();
    int i, N = vert.size();
    int E = poly.size();
    int sa, sb, sc;
    set<pair<int, int> > sedges;

    // create symmetric and non-redundant edges
    for( i=0; i<E; ++i )
    {
      sa = poly[i][0];
      sb = poly[i][1];
      sc = poly[i][2];

      sedges.insert( make_pair( sa, sb ) );
      sedges.insert( make_pair( sb, sa ) );
      sedges.insert( make_pair( sa, sc ) );
      sedges.insert( make_pair( sc, sa ) );
      sedges.insert( make_pair( sb, sc ) );
      sedges.insert( make_pair( sc, sb ) );
    }

    E = sedges.size();
    edges.clear();
    edges.reserve( E );
    weights.clear();
    weights.reserve( E );
    set<pair<int, int> >::iterator is, es = sedges.end();
    for( is=sedges.begin(); is!=es; ++is )
    {
      edges.push_back( AimsVector<uint,2>( is->first, is->second ) );
      weights.push_back( (vert[is->first] - vert[is->second]).norm() );
    }
  }


  int meshWatershed( const AimsSurfaceTriangle & mesh,
                     const vector<double> & field, vector<int> & lidx,
                     vector<int> & ldepth, vector<int> & lmajor,
                     vector<int> & label, double th )
  {
    // taken from old fff_field.c (ancestor of nipy)
    int i,r,N = mesh.vertex().size();
    vector<AimsVector<uint,2> > edges;
    vector<double> weights;

    distancesFromMesh( mesh, edges, weights );

    int E = edges.size();

    int nA,nB,remain;
    double delta;

    int k = 0;

    vector<int> win( N, 0 );
    vector<int> maj1( N, 0 );
    vector<int> maj2;
    vector<int> incwin( N, 0 );
    vector<double> mfield;
    vector<double> Mfield;
    if( field.size() != N )
    {
      mfield.reserve( N );
      mfield.insert( mfield.begin(), 0., N );
    }
    else
      mfield = field;
    Mfield = mfield;

    for (i=0 ; i<N ; i++)
    {
      maj1[i] = i;
      if( field[i] > th )
        win[i] = 1;
    }
    maj2 = maj1;

    /* Iterative dilation  */
    for( r=0 ; r<N ; r++ )
    {
      for (i=0 ; i<E ; i++)
      {
        nA = edges[i][0];
        nB = edges[i][1];
        if( field[nA] > th )
          if( mfield[nA] <  mfield[nB] )
          {
            win[nA] = 0;
            if( Mfield[nA] < mfield[nB] )
            {
              Mfield[nA] = mfield[nB];
              maj2[nA] = maj2[nB];
              if( incwin[nA] == r )
                maj1[nA] = maj2[nB];
            }
          }
      }
      remain = 0;

      for( i=0; i<N; ++i )
        mfield[i] -= Mfield[i];
      delta = 0; // dot prod
      for( i=0; i<N; ++i )
        delta += mfield[i] * mfield[i];
      mfield = Mfield;
      for( i=0; i<N; ++i )
        incwin[i] += win[i];
      for( i=0 ; i<N ; i++ )
        remain += (win[i]>0);

      if (remain<2)
        break;
      if (delta==0)
        break;
      /* stop when all the maxima have been found  */
    }

    /* get the local maximum associated with any point  */
    int j,aux;
    for( i=0 ; i<N ; i++ )
    {
      if( field[i] > th )
      {
        j = maj1[i];
        while( incwin[j] ==0 )
          j = maj1[j];
        maj1[i] = j;
      }
    }

    /* number of bassins  */
    for( i=0 ; i<N ; i++ )
      k+= (incwin[i]>0);

    if( lidx.size() != k )
      lidx.resize( k );
    if( ldepth.size() != k )
      ldepth.resize( k );
    if( lmajor.size() != k )
      lmajor.resize( k );

    /* write the maxima and related stuff  */
    j=0;
    for( i=0 ; i<N ; i++ )
      if( incwin[i] > 0 )
      {
        lidx[j] = i;
        ldepth[j] = incwin[i];
        maj2[i] = j;/* ugly, but OK  */
        j++;
      }
    for( j=0 ; j<k ; j++ )
    {
      i = lidx[j];
      if( maj1[i] != i )
      { /* i is not a global maximum */
        aux = maj2[maj1[i]];
        lmajor[j] = aux;
      }
      else
        lmajor[j] = j;
    }

    if( label.size() != N )
      label.resize( N );

    /* Finally set the labels */
    for( i=0 ; i<N ; i++ )
    {
      if( field[i] <= th )
        label[i] = -1;
      else
      {
        aux = maj2[maj1[i]];
        label[i] = aux;
      }
    }
    for( j=0 ; j<k ; j++ )
    {
      i = lidx[j];
      label[i] = j;
    }

    return k;
  }


  int meshWatershed( const AimsSurfaceTriangle & mesh,
                     const TimeTexture<double> & field,
                     TimeTexture<int> & idx,
                     TimeTexture<int> & depth, TimeTexture<int> & major,
                     TimeTexture<int> & label, double threshold )
  {
    return meshWatershed( mesh, field.begin()->second.data(), idx[0].data(),
                          depth[0].data(), major[0].data(), label[0].data(),
                          threshold );
  }


  static void _graph_to_neighb( vector<long> & cindices,
                                vector<long> & neighb,
                                vector<double> & weight,
                                const vector<AimsVector<uint, 2> > & edges,
                                const vector<double> & inweights, long V )
  {
    /* This function recomputes the connectivity system of the graph in an efficient way:
    The edges are arraned in the following manner
        origins =  [0..0 1..1 .. V-1..V-1]
        ends    =  [neignb[0].. neighb[E-1]]
        weight  =  [weights[0]..weights[E-1]]

        cindices codes for the origin vector: origin=i between cindices[i] and cindices[i+1]-1
    */
    long E = edges.size();
    long a,b;
    long i,j;
    double aux = 0;

    if( ( cindices.size() < V ) | ( neighb.size() < E )
      | ( weight.size() < E ) )
      throw runtime_error( "inconsistant vector size" );

    for( i=0; i<V; ++i )
      cindices[i] = 0;

    for( i=0 ; i<E ; i++ )
    {
      ++cindices[ edges[i][0] ];
      neighb[i] = -1;
    }

    for( i=0; i<V; i++ )
    {
      j = cindices[i];
      cindices[i] = aux;
      aux += j;
    }
    if( cindices.size() > V )
      cindices[V] = E;

    for( i=0 ; i<E ; i++ )
    {
      a = edges[i][0];
      b = edges[i][1];
      j = cindices[a];
      while( neighb[j] > -1 )
        ++j;
      neighb[j] = b;
      weight[j] = inweights[i];
    }
  }


  int meshBlobsBifurcation( const AimsSurfaceTriangle & mesh,
                             const vector<double> & field, vector<int> & Idx,
                             vector<double> & Height, vector<int> & Father,
                             vector<int> & label, double th )
  {
    // idx: output table of seed node of each blob
    // father: blob number which merges two (or more) others
    // height: field value of the peak (seed) node in a blob
    // label: blob number for each node

    // taken from old fff_field.c (ancestor of nipy)
    int V = mesh.vertex().size();
    vector<AimsVector<uint,2> > edges;
    vector<double> weights;

    distancesFromMesh( mesh, edges, weights );

    int E = edges.size();

    int i,j,k,l,win,start, end;
    int ll = 0;
    vector<long> cindices( V+1, 0 ), neighb( E, 0 );
    vector<double> weight( E, 0. );
    vector<long> father( 2*V, 0 ), possible( V, 0 ), idx( 2*V, 0 );
    vector<double> height( 2*V, 0. );
    vector<long> p( V, 0 );
    long q;

    if( label.size() != V )
      label.resize( V );

    _graph_to_neighb( cindices, neighb, weight, edges, weights, V );

    if( field.size() != V )
      throw runtime_error( "incompatible field / mesh size" );

    /* sort the data */
    multimap<double, int> scfield;
    multimap<double, int>::iterator is, es = scfield.end();
    for( i=0; i<V; ++i )
      scfield.insert( make_pair( -field[i], i ) );
    for( i=0, is=scfield.begin(); is!=es; ++is, ++i )
      p[i] = is->second;
    // p contains an ordered list (decreasing field value) of indices

    for( i=0; i<V; ++i )
      label[i] = -1;
    for( i=0; i<2*V ; ++i )
      father[i] = i;


    for( i=0; i<V ; i++ )
    {
      win = p[i];
      if( field[win] < th )
        break;
      else
      {
        start = cindices[win];
        end = cindices[win+1];
        for( j=0; j<V; ++j )
          possible[j] = -1;
        q = 0;

        for( j=start; j<end; ++j ) // all neighbours of win
        {
          k = label[neighb[j]];

          /* fill in "possible": list of existing labels in neighbourhood of
              win */
          if( k > -1 )
          {
            while( father[k] != k )
              k = father[k];
            for( l=0 ; l<q ; ++l )
              if( possible[l] > -1 )
                if( possible[l] ==k )
                  break;
            if( possible[l] !=k )
            {
              if( l > 1 )
              {
                /* printf("%ld %ld %ld",i,q, l);
                    for (m=0 ; m<l+1 ; m++) printf(" %ld ",possible[m]);
                    printf("\n"); */
              }
              possible[q] = k;
              q++;
            }
          }
        }

        if( q == 0 ) // no neighbouring label yet: assign a new one
        {
          label[win] = ll;
          idx[ll] = win;
          height[ll] = field[win];
          ll++;
        }
        if( q == 1 ) // one neighbouring label: grow the same one
          label[win] = possible[0];
        if( q > 1 )
        {
          /* birfurcation : create a new label */
          for( j=0; j<q ; j++ )
          {
            k =  possible[j];
            father[k] = ll;
          }
          label[win] = ll;
          idx[ll] = win;
          height[ll] = field[win];
          ll++;
        }
      }
    }

    Father.resize( ll );
    Idx.resize( ll );
    Height.resize( ll );
    for( i=0; i<ll; ++i )
    {
      Father[i] = father[i];
      Idx[i] = idx[i];
      Height[i] = height[i];
    }

    return ll;
  }


  int meshBlobsBifurcation( const AimsSurfaceTriangle & mesh,
                            const TimeTexture<double> & field,
                            TimeTexture<int> & idx,
                            TimeTexture<double> & height,
                            TimeTexture<int> & father,
                            TimeTexture<int> & label, double th )
  {
    return meshBlobsBifurcation( mesh, field.begin()->second.data(),
                                 idx[0].data(), height[0].data(),
                                 father[0].data(), label[0].data(), th );
  }

}
