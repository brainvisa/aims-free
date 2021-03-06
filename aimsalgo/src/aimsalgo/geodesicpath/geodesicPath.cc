/*
 *  Copyright (C) 2000-2004 CEA
 *
 *  This software and supporting documentation were developed by
 *    CEA/DSV/SHFJ
 *    4 place du General Leclerc
 *    91401 Orsay cedex
 *    France
 */

#include <aims/geodesicpath/geodesicPath.h>

using namespace aims;
using namespace geodesic;
using namespace std;

GeodesicPath::GeodesicPath( const AimsSurfaceTriangle & surface, int method,
                            int strain ) :
    _strain(strain), _method(method), _surface(surface)
{
  //cout << "compute texture curvature : ";
  _texCurv = TimeTexture<float>(1, _surface.vertex().size());
  _texCurv = AimsMeshCurvature(_surface[0]);
  //cout << "done" << endl;

  computeGraphDijkstra(_surface, _texCurv, _method, _strain);
}

GeodesicPath::GeodesicPath( const AimsSurfaceTriangle & surface,
                            const TimeTexture<float> & texCurv, int method,
                            int strain ) :
    _strain(strain), _method(method), _surface(surface), _texCurv(texCurv)
{
  computeGraphDijkstra(_surface, _texCurv, _method, _strain);
}

GeodesicPath::~GeodesicPath()
{
  _surface.erase();
  _texCurv.erase();
}

//
void GeodesicPath::computeGraphDijkstra( const AimsSurfaceTriangle & surface,
                                         const TimeTexture<float> & texCurv,
                                         int method,int strain )
{
  const vector<float> &curv = texCurv.begin()->second.data();

  // copy vertex and faces vector
  vector<double> pointsSP;
  vector<unsigned> facesSP;

  const vector<Point3df> & vert = surface.vertex();
  const vector<AimsVector<uint, 3> > & tri = surface.polygon();
  pointsSP.resize(3*vert.size());
  facesSP.resize(3*tri.size());

  for (size_t j = 0; j < vert.size(); j++)
  {
    pointsSP[3*j] = vert[j][0];
    pointsSP[3*j+1] = vert[j][1];
    pointsSP[3*j+2] = vert[j][2];
  }
  for (size_t j = 0; j < tri.size(); j++)
  {
    facesSP[3*j] = tri[j][0];
    facesSP[3*j+1] = tri[j][1];
    facesSP[3*j+2] = tri[j][2];
  }

  // compute adjacence graph
//  cout << "compute adjacence graph : ";
  if (method >= 0 && method < 3)
    _meshSP.initialize_mesh_data(pointsSP,facesSP, &curv[0],method ,strain);
  if (method == 3)
    _meshSP.initialize_mesh_data(pointsSP,facesSP, NULL ,method ,0);
  if (method > 3)
      _meshSP.initialize_mesh_data(pointsSP,facesSP, NULL ,0 ,0);
//  cout << "done" << endl;
}

void GeodesicPath::updateWeight( const TimeTexture<float> & texCurv,
                                 int method, int strain, double sigmo )
{
  const vector<float> &curv = texCurv.begin()->second.data();

  if (method >= 0 && method < 3)
    _meshSP.update_weight(&curv[0],method,(double)strain,sigmo);
  if (method == 3)
    _meshSP.update_weight(NULL,method,0.0,sigmo);
  if (method > 3)
    _meshSP.update_weight(NULL,0,0.0,2.0);
}


vector<Point3df> GeodesicPath::shortestPath_1_1_xyz(unsigned source, unsigned target)
{
  vector<unsigned> listIndexVertexPathSP;
  listIndexVertexPathSP.clear();

  // clear path
  vector<geodesic::SurfacePoint> SPath;
  SPath.clear();

  // dijkstra method
  geodesic::GeodesicAlgorithmBase *dijkstra_algorithm;

  if (_method != 3)
    dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);
  else
    dijkstra_algorithm = new geodesic::GeodesicAlgorithmExact(&_meshSP);


  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  geodesic::SurfacePoint short_targets(&_meshSP.vertices()[target]);

  std::vector<geodesic::SurfacePoint> all_sources(1,short_sources);
  if (_method == 3)
  {
    dijkstra_algorithm->propagate(all_sources);

//------------------first task: compute the pathes to the targets----
    std::vector<geodesic::SurfacePoint> path;

    geodesic::SurfacePoint t(&_meshSP.vertices()[target]);

    dijkstra_algorithm->trace_back(t, path);
    dijkstra_algorithm->print_info_about_path(path);

//    for(int j=0; j<path.size(); ++j)
//      //cout << endl;
//      cout << path[j].x() << " " << path[j].y() << " "<< path[j].z() << "\n";

    cout << endl;

  }

//    for (int j = 0; j < targets.size(); j++)
//    {
//      geodesic::SurfacePoint t(&_meshSP.vertices()[targets[j]]);
//
//      unsigned best_source = dijkstra_algorithm->best_source(t,distance_temp);   //for a given surface point, find closets source and distance to this source
//
//      if (distance_temp < min_distance_temp )
//      {
//        min_distance_temp = distance_temp;
//        *target = targets[j];
//        *length = min_distance_temp;
//      }
//    }
//  dijkstra_algorithm->propagate(short_sources);    //cover the whole mesh
//  dijkstra_algorithm->print_statistics();
//
//  //------------------first task: compute the pathes to the targets----
//  std::vector<geodesic::SurfacePoint> path;
//  for(int i=0; i<short_targets.size(); ++i)
//  {
//    algorithm->trace_back(short_targets[i], path);
//    print_info_about_path(path);
//
//    for(int j=0; j<path.size(); ++j)
//      //cout << endl;
//      cout << path[j].x() << " " << path[j].y() << " "<< path[j].z() << "\n";
//
//    cout << endl;
//  }


  dijkstra_algorithm->geodesic(short_sources,short_targets, SPath, listIndexVertexPathSP);

  reverse(listIndexVertexPathSP.begin(),listIndexVertexPathSP.end());
  listIndexVertexPathSP.push_back((int)target);

  std::vector< Point3df > vertexList;
  Point3df newVertex;

  for (size_t i = 0; i < SPath.size(); i++)
  {
    newVertex[0] = SPath[i].x();
    newVertex[1] = SPath[i].y();
    newVertex[2] = SPath[i].z();
    vertexList.push_back(newVertex);
  }

  delete(dijkstra_algorithm);

  return vertexList;
}

vector<unsigned> GeodesicPath::shortestPath_1_1_ind(unsigned source, unsigned target)
{
  vector<unsigned> listIndexVertexPathSP;
  listIndexVertexPathSP.clear();

  // clear path
  vector<geodesic::SurfacePoint> SPath;
  SPath.clear();

  // dijkstra method
  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  geodesic::SurfacePoint short_targets(&_meshSP.vertices()[target]);

  dijkstra_algorithm->geodesic(short_sources,short_targets, SPath, listIndexVertexPathSP);

  reverse(listIndexVertexPathSP.begin(),listIndexVertexPathSP.end());
  listIndexVertexPathSP.push_back((int)target);

  delete(dijkstra_algorithm);

  return listIndexVertexPathSP;
}


vector<unsigned> GeodesicPath::shortestPath_1_1_ind(unsigned source, unsigned target, TimeTexture<short> subset)
{

  uint i, j, ns=_surface.vertex().size();
  std::vector< AimsVector< uint, 3 > > polyS, poly=_surface.polygon();
  uint np=poly.size();
  AimsVector< uint, 3 > tri;
  AimsSurfaceTriangle surfS;
  std::map<int, int> mapAtoS, mapStoA;

  std::vector< Point3df > vertS, vert=_surface.vertex();

  // Ici subset contient 0 partout sauf la ou on peut chercher le plus court chemin.

  // On commence par verifier que source et target sont dedans.

  if (subset[0].item(source)==0)
  {
    cerr << "GeodesicPath.shortestPath_1_1_ind : looking for a path through a subset to which source does not belong" << endl;
    exit(EXIT_FAILURE);
  }
  if (subset[0].item(target)==0)
  {
    cerr << "GeodesicPath.shortestPath_1_1_ind : looking for a path through a subset to which target does not belong" << endl;
    exit(EXIT_FAILURE);
  }

  // ensuite on extrait une sous-surface et on garde l'�quivalence de noeud avec l'ancienne.
  j=0;
  for (i=0; i<ns; i++)
  {
    if (subset[0].item(i)!=0)
    {
      vertS.push_back(vert[i]);
      mapAtoS[i]=j;
      mapStoA[j]=i;
      j++;
    }
  }
  for (i=0; i<np; i++)
  {
    tri=poly[i];
    if ( (subset[0].item(tri[0])!=0) && (subset[0].item(tri[1])!=0) && (subset[0].item(tri[2])!=0))
      polyS.push_back(AimsVector< uint, 3 >(mapAtoS[tri[0]], mapAtoS[tri[1]], mapAtoS[tri[2]]));
  }
  uint ns_sub=vertS.size();
  TimeTexture<float> curvS(1, ns_sub);
  for (j=0; j<ns_sub; j++)
  {
    curvS[0].item(j)=_texCurv[0].item(mapStoA[j]);
  }
    surfS.vertex()=vertS;
    surfS.polygon()=polyS;

    // Now computing shortest path on the subset surface
  vector<unsigned> listIndexVertexPathSP_sub, listIndexVertexPathSP;

  GeodesicPath geoSub( surfS, curvS, _method, _strain);
  listIndexVertexPathSP_sub=geoSub.shortestPath_1_1_ind(mapAtoS[source], mapAtoS[target]);
  for (i=0; i<listIndexVertexPathSP_sub.size(); i++)
    listIndexVertexPathSP.push_back(mapStoA[listIndexVertexPathSP_sub[i]]);

  return listIndexVertexPathSP;
}


vector<unsigned> GeodesicPath::shortestPath_1_1_1_ind(unsigned source, unsigned middle, unsigned target)
{
  vector<unsigned> first;
  vector<unsigned> second;
  first=shortestPath_1_1_ind(source, middle);
  first.erase(remove(first.begin(), first.end(), middle), first.end());
  second=shortestPath_1_1_ind(middle, target);
  first.insert( first.end(), second.begin(), second.end() );
  return first;
}

void GeodesicPath::shortestPath_1_1_tex(unsigned source, unsigned target, float texturevalue, TimeTexture<float> &tex)
{
  vector<unsigned> listIndexVertexPathSP;

  listIndexVertexPathSP = shortestPath_1_1_ind(source,target);

  for (unsigned i = 0; i < listIndexVertexPathSP.size(); i++)
    tex[0].item(listIndexVertexPathSP[i]) = texturevalue;
}

void GeodesicPath::shortestPath_1_1_ind_xyz(unsigned source, unsigned target,std::vector<unsigned>& indice, std::vector<Point3df>& coord3D )
{
  // clear path
  vector<geodesic::SurfacePoint> SPath;
  SPath.clear();

  // dijkstra method
  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  geodesic::SurfacePoint short_targets(&_meshSP.vertices()[target]);

  dijkstra_algorithm->geodesic(short_sources,short_targets, SPath, indice);

  reverse(indice.begin(),indice.end());
  indice.push_back((int)target);

  Point3df newVertex;

  for (size_t i = 0; i < SPath.size(); i++)
  {
    newVertex[0] = SPath[i].x();
    newVertex[1] = SPath[i].y();
    newVertex[2] = SPath[i].z();
    coord3D.push_back(newVertex);
  }

  delete(dijkstra_algorithm);
}

double GeodesicPath::shortestPath_1_1_len(unsigned source, unsigned target)
{
  double length=0.0;

  vector<Point3df> listIndexVertexPathSP;

  listIndexVertexPathSP = shortestPath_1_1_xyz(source, target);

  double dx,dy,dz;

  for (size_t i = 0; i < listIndexVertexPathSP.size() - 1; i++)
  {
    dx = listIndexVertexPathSP[i+1][0]-listIndexVertexPathSP[i][0];
    dy = listIndexVertexPathSP[i+1][1]-listIndexVertexPathSP[i][1];
    dz = listIndexVertexPathSP[i+1][2]-listIndexVertexPathSP[i][2];

    length += sqrt(dx*dx + dy*dy + dz*dz);
  }

  return length;
}


void GeodesicPath::shortestPath_1_N_ind(unsigned source, vector<unsigned> targets, unsigned *target, double *length)
{
  double distance_temp;
  double min_distance_temp = 100000.;

  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  std::vector<geodesic::SurfacePoint> all_sources(1,short_sources);

  dijkstra_algorithm->propagate(all_sources);

  for (size_t j = 0; j < targets.size(); j++)
  {
    geodesic::SurfacePoint t(&_meshSP.vertices()[targets[j]]);

    unsigned best_source = dijkstra_algorithm->best_source(t,distance_temp);   //for a given surface point, find closets source and distance to this source
    best_source = best_source; // compilation warning...

    if (distance_temp < min_distance_temp )
    {
      min_distance_temp = distance_temp;
      *target = targets[j];
      *length = min_distance_temp;
    }
  }

  delete(dijkstra_algorithm);
}

void GeodesicPath::shortestPath_1_N_All_ind(unsigned source, vector<unsigned> targets, vector<vector<unsigned> >&indices)
{
  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);

  std::vector<geodesic::SurfacePoint> all_targets;

  for (int i = 0 ; i < (int)targets.size() ; i++)
    all_targets.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[targets[i]]));

  std::vector<std::vector<SurfacePoint> > paths;

  dijkstra_algorithm->geodesic(short_sources,all_targets,paths,indices);

  delete(dijkstra_algorithm);
}

void GeodesicPath::longestPath_1_N_ind(unsigned source, vector<unsigned> targets, unsigned *target, double *length, int type_distance)
{
  double distance_temp;
  double max_distance_temp = 0.0;

  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  std::vector<geodesic::SurfacePoint> all_sources(1,short_sources);

  // distance pondérée
  if (type_distance == 0)
  {
    dijkstra_algorithm->propagate(all_sources);

    for (size_t j = 0; j < targets.size(); j++)
    {
      geodesic::SurfacePoint t(&_meshSP.vertices()[targets[j]]);
      unsigned best_source = dijkstra_algorithm->best_source(t,distance_temp);   //for a given surface point, find closets source and distance to this source
      best_source = best_source; // compilation warning...

      if (distance_temp > max_distance_temp )
      {
        max_distance_temp = distance_temp;
        *target = targets[j];
        *length = max_distance_temp;
      }
    }
  }

  // distance euclidienne
  if (type_distance == 1)
  {
    std::vector<geodesic::SurfacePoint> all_targets;

    for (int i = 0 ; i < (int)targets.size() ; i++)
      all_targets.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[targets[i]]));


    vector<vector<SurfacePoint> > paths;
    vector<vector<unsigned> >indices;

    paths.clear();

    dijkstra_algorithm->geodesic(short_sources,all_targets,paths,indices);

    vector<vector<SurfacePoint> >::iterator ite = paths.begin();

    double max_len,len;
    max_len = 0.0;
    int ind = 0;

    for (; ite != paths.end(); ++ite)
    {
      ind++;
      len = dijkstra_algorithm->length(*ite);
//     vector< SurfacePoint >::iterator ite_holes = (*ite).begin();
//      for (; ite_holes != (*ite).end(); ite_holes++)
//      {
//       cout << (double)((*ite_holes).x()) << " " << (double)((*ite_holes).y()) << " " << (double)((*ite_holes).z()) << " ";
//      }

      if (len > max_len)
      {
        max_len = len;
        *length = max_len;
        *target = targets[ind-1];
      }
    }
  }

  delete(dijkstra_algorithm);
}

vector<GeodesicPath::vect_ui>
GeodesicPath::longestPath_1_N_len(unsigned source, vector<unsigned> targets,
                                  vector<double> &length, int type_distance)
{
  double distance_temp;
  double max_distance_temp = 0.0;

  vector<vect_ui> indices;

  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  std::vector<geodesic::SurfacePoint> all_sources(1,short_sources);

  // distance pondérée
  if (type_distance == 0)
  {
    dijkstra_algorithm->propagate(all_sources);

    for (size_t j = 0; j < targets.size(); j++)
    {
      geodesic::SurfacePoint t(&_meshSP.vertices()[targets[j]]);
      unsigned best_source = dijkstra_algorithm->best_source(t,distance_temp);   //for a given surface point, find closets source and distance to this source
      best_source = best_source; // compilation warning...

      if (distance_temp > max_distance_temp )
      {
        max_distance_temp = distance_temp;
        //*target = targets[j];
        //*length = max_distance_temp;
      }
    }
  }

  // distance euclidienne
  if (type_distance == 1)
  {
    std::vector<geodesic::SurfacePoint> all_targets;

    for (int i = 0 ; i < (int)targets.size() ; i++)
      all_targets.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[targets[i]]));

    vector<vector<SurfacePoint> > paths;

    paths.clear();

    dijkstra_algorithm->geodesic(short_sources,all_targets,paths,indices);

    vector<vector<SurfacePoint> >::iterator ite = paths.begin();

    double /*max_len,*/len;
    //max_len = 0.0;
    int ind = 0;

    for (; ite != paths.end(); ++ite)
    {
      ind++;
      len = dijkstra_algorithm->length(*ite);

      length.push_back(len);


//     vector< SurfacePoint >::iterator ite_holes = (*ite).begin();
//      for (; ite_holes != (*ite).end(); ite_holes++)
//      {
//       cout << (double)((*ite_holes).x()) << " " << (double)((*ite_holes).y()) << " " << (double)((*ite_holes).z()) << " ";
//      }

//      if (len > max_len)
//      {
//        max_len = len;
//        *length = max_len;
//        *target = targets[ind-1];
//      }
    }
  }

  delete(dijkstra_algorithm);

  return indices;
}

vector<unsigned> GeodesicPath::longestPath_N_N_ind(vector<unsigned> points, int* s, int *d, double *length,int type_distance)
{
  unsigned target;
  vector<unsigned> listIndexVertexPathSP;
  vector<unsigned> targetsTemp;

  int index_max_i,index_max_j,index;
  double dist,dist_max;

  index_max_i = -1;
  index_max_j = -1;

  //int nb_combinaison_max = (points.size() * (points.size()-1))/2;
  int nb_combinaison = 0;

  dist_max = -100000;

  //std::cout << "nb extremities " << points.size() << "-->  ";

  for (size_t i=0; i<points.size(); i++)
    {
    targetsTemp.push_back(points[i]);
    //cout << points[i] << " ";
    }

  for (size_t i=0; i<points.size()-1; i++)
  {
    index = points[i];
    targetsTemp.erase(targetsTemp.begin());
    nb_combinaison = nb_combinaison + targetsTemp.size();

    //0 avec la distance euclidienne pondérée
    //1 avec la distance euclidienne

    longestPath_1_N_ind(index, targetsTemp, &target, &dist, type_distance);

    //cout << index << " " << target << " " << dist << endl;

    if (dist >= dist_max)
    {
      dist_max = dist;
      index_max_i = index;
      index_max_j = target;
    }
    //std::cout << "\r\033[K" <<  nb_combinaison << "/" << nb_combinaison_max << " " << index_max_i << " " << index_max_j << " "<< dist_max << std::flush;
  }

  *s = index_max_i;
  *d = index_max_j;
  *length = dist;

  listIndexVertexPathSP = shortestPath_1_1_ind(*s,*d);

  return listIndexVertexPathSP;
}

void GeodesicPath::distanceMap_1_N_ind(unsigned source, vector<float> &distanceMap,double *length, int type_distance)
{
  double distance_temp;
  double max_distance_temp = 0.0;

  // dijkstra method
  geodesic::GeodesicAlgorithmBase *dijkstra_algorithm;

  if (_method != 3)
    dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);
  else
    dijkstra_algorithm = new geodesic::GeodesicAlgorithmExact(&_meshSP);

//  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
//  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  std::vector<geodesic::SurfacePoint> all_sources(1,short_sources);

  //distanceMap.resize(_meshSP.vertices().size());

  // distance pondérée
  if (type_distance == 0)
  {
    dijkstra_algorithm->propagate(all_sources);

    for (size_t j = 0; j < _meshSP.vertices().size(); j++)
    {
      geodesic::SurfacePoint t(&_meshSP.vertices()[j]);
      unsigned best_source = dijkstra_algorithm->best_source(t,distance_temp);   //for a given surface point, find closets source and distance to this source
      best_source = best_source; 

      distanceMap.push_back(distance_temp);

      if (distance_temp > max_distance_temp )
      {
        max_distance_temp = distance_temp;
        *length = max_distance_temp;
      }
    }
  }


//  // distance euclidienne (ok pour les chemins exacts)
  if (type_distance == 1)
  {
    std::vector<geodesic::SurfacePoint> all_targets;

    for (size_t j = 0; j < _meshSP.vertices().size(); j++)
      {
      //cout << j << endl;
      all_targets.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[j]));
      }

    //cout << "size " << _meshSP.vertices().size() << endl;


    vector<vector<SurfacePoint> > paths;
    vector<vector<unsigned> >indices;

    paths.clear();

    dijkstra_algorithm->geodesic(short_sources,all_targets,paths,indices);

    vector<vector<SurfacePoint> >::iterator ite = paths.begin();

    //cout << "size " << paths.size() << endl;

    double max_len,len;
    max_len = 0.0;
    int ind = 0;

    for (; ite != paths.end(); ++ite)
    {
      ind++;
      len = dijkstra_algorithm->length(*ite);

      //cout << len << endl;
      distanceMap.push_back(len);

      if (len > max_len)
      {
        max_len = len;
        *length = max_len;
      }
    }

  }

  delete(dijkstra_algorithm);
}
