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

using namespace std;

GeodesicPath::GeodesicPath( AimsSurfaceTriangle surface, int method, int strain ) :
    _surface(surface), _method(method), _strain(strain)
{
  cout << "compute texture curvature : ";
  _texCurv = TimeTexture<float>(1, _surface.vertex().size());
  _texCurv = AimsMeshCurvature(_surface[0]);
  cout << "done" << endl;

  computeGraphDijkstra(_surface, _texCurv, _method, _strain);
}

GeodesicPath::GeodesicPath( AimsSurfaceTriangle surface ,TimeTexture<float> texCurv, int method, int strain ) :
    _surface(surface), _texCurv(texCurv), _method(method), _strain(strain)
{
  computeGraphDijkstra(_surface, _texCurv, _method, _strain);
}

GeodesicPath::~GeodesicPath()
{
  _surface.erase();
  _texCurv.erase();
}

//
void GeodesicPath::computeGraphDijkstra (AimsSurfaceTriangle surface, TimeTexture<float> texCurv, int method,int strain)
{
  vector<float> &curv = _texCurv[0].data();

  // copy vertex and faces vector
  vector<double> pointsSP;
  vector<unsigned> facesSP;

  vector<Point3df> & vert = surface.vertex();
  vector<AimsVector<uint, 3> > & tri = surface.polygon();
  pointsSP.resize(3*vert.size());
  facesSP.resize(3*tri.size());

  for (uint j = 0; j < (int) vert.size(); j++)
  {
    pointsSP[3*j] = vert[j][0];
    pointsSP[3*j+1] = vert[j][1];
    pointsSP[3*j+2] = vert[j][2];
  }
  for (uint j = 0; j < (int) tri.size(); j++)
  {
    facesSP[3*j] = tri[j][0];
    facesSP[3*j+1] = tri[j][1];
    facesSP[3*j+2] = tri[j][2];
  }

  // compute adjacence graph
  cout << "compute adjacence graph : ";
  if (method >= 0 && method < 3)
    _meshSP.initialize_mesh_data(pointsSP,facesSP, &curv[0],method ,strain);
  if (method == 3)
    _meshSP.initialize_mesh_data(pointsSP,facesSP, NULL ,method ,0);
  if (method > 3)
      _meshSP.initialize_mesh_data(pointsSP,facesSP, NULL ,0 ,0);
  cout << "done" << endl;
}

vector<Point3df> GeodesicPath::shortestPathCoordVextex(unsigned source, unsigned target)
{
  vector<int> listIndexVertexPathSP;

  // compute shortest path
  //cout << "compute shortest path : ";

  vector<geodesic::SurfacePoint> sources;
  sources.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[source]));

  vector<geodesic::SurfacePoint> targets;
  targets.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[target]));

  //printf("indice source = %d target = %d \n",source, target);

  // clear path
  vector<geodesic::SurfacePoint> SPath;
  SPath.clear();

  // dijkstra method
  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  //listIndexVertexPathSP.clear();

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  geodesic::SurfacePoint short_targets(&_meshSP.vertices()[target]);

  dijkstra_algorithm->geodesic(short_sources,short_targets, SPath, listIndexVertexPathSP);

  reverse(listIndexVertexPathSP.begin(),listIndexVertexPathSP.end());
  listIndexVertexPathSP.push_back((int)target);

  std::vector< Point3df > vertexList;
  Point3df newVertex;

  int i;
  for (i = 0; i < SPath.size(); i++)
  {
    newVertex[0] = SPath[i].x();
    newVertex[1] = SPath[i].y();
    newVertex[2] = SPath[i].z();
    //cout << "(" << SPath[i].x() << ',' << SPath[i].y() << ',' <<  SPath[i].z() << ")\n";
    vertexList.push_back(newVertex);
  }

  //Rajout olivier
  delete(dijkstra_algorithm);

  return vertexList;
}

float GeodesicPath::shortestPathLength(unsigned source, unsigned target)
{
  float length=0.0;

  vector<Point3df> listIndexVertexPathSP;

  listIndexVertexPathSP = shortestPathCoordVextex(source, target);

  int i;
  float dx,dy,dz;

  for (i = 0; i < listIndexVertexPathSP.size() - 1; i++)
  {
    dx = listIndexVertexPathSP[i+1][0]-listIndexVertexPathSP[i][0];
    dy = listIndexVertexPathSP[i+1][1]-listIndexVertexPathSP[i][1];
    dz = listIndexVertexPathSP[i+1][2]-listIndexVertexPathSP[i][2];

    length += sqrt(dx*dx + dy*dy + dz*dz);
  }

  return length;
}

vector<int> GeodesicPath::shortestPathIndiceVextex(unsigned source, unsigned target)
{
  vector<int> listIndexVertexPathSP;

  // compute shortest path
  //cout << "compute shortest path : ";

  vector<geodesic::SurfacePoint> sources;
  sources.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[source]));

  vector<geodesic::SurfacePoint> targets;
  targets.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[target]));

  //printf("indice source = %d target = %d \n",source, target);

  // clear path
  vector<geodesic::SurfacePoint> SPath;
  SPath.clear();

  // dijkstra method
  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  listIndexVertexPathSP.clear();

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  geodesic::SurfacePoint short_targets(&_meshSP.vertices()[target]);

  dijkstra_algorithm->geodesic(short_sources,short_targets, SPath, listIndexVertexPathSP);

  //std::vector<int>::iterator ite;
  reverse(listIndexVertexPathSP.begin(),listIndexVertexPathSP.end());
  listIndexVertexPathSP.push_back((int)target);

  // OK
  delete(dijkstra_algorithm);

  return listIndexVertexPathSP;
}

vector<int> GeodesicPath::shortestPathIndice3Vextex(unsigned source, unsigned middle, unsigned target)
{
  vector<int> first;
  vector<int> second;
  first=shortestPathIndiceVextex(source, middle);
  first.erase(remove(first.begin(), first.end(), middle), first.end());
  second=shortestPathIndiceVextex(middle, target);
  first.insert( first.end(), second.begin(), second.end() );
  return first;

}

void GeodesicPath::shortestPath2Texture(unsigned source, unsigned target, float texturevalue, TimeTexture<float> &tex)
{
  vector<int> listIndexVertexPathSP;

  listIndexVertexPathSP = shortestPathIndiceVextex(source,target);

  for (unsigned i = 0; i < listIndexVertexPathSP.size(); i++)
    tex[0].item(listIndexVertexPathSP[i]) = texturevalue;
}

void GeodesicPath::shortestPathIndiceCoordVextex(unsigned source, unsigned target,std::vector<int>& indice, std::vector<Point3df>& coord3D )
{
  // compute shortest path
  //cout << "compute shortest path : ";

  vector<geodesic::SurfacePoint> sources;
  sources.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[source]));

  vector<geodesic::SurfacePoint> targets;
  targets.push_back(geodesic::SurfacePoint(&_meshSP.vertices()[target]));

  //printf("indice source = %d target = %d \n",source, target);

  // clear path
  vector<geodesic::SurfacePoint> SPath;
  SPath.clear();

  // dijkstra method
  geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
  dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&_meshSP);

  //listIndexVertexPathSP.clear();

  geodesic::SurfacePoint short_sources(&_meshSP.vertices()[source]);
  geodesic::SurfacePoint short_targets(&_meshSP.vertices()[target]);

  dijkstra_algorithm->geodesic(short_sources,short_targets, SPath, indice);

  reverse(indice.begin(),indice.end());
  indice.push_back((int)target);

  //std::vector< Point3df > vertexList;
  Point3df newVertex;

  int i;
  for (i = 0; i < SPath.size(); i++)
  {
    newVertex[0] = SPath[i].x();
    newVertex[1] = SPath[i].y();
    newVertex[2] = SPath[i].z();
    //cout << "(" << SPath[i].x() << ',' << SPath[i].y() << ',' <<  SPath[i].z() << ")\n";
    coord3D.push_back(newVertex);
  }
  // rajout Olivier
  delete(dijkstra_algorithm);

}


//GeodesicPath sp(surface,texCurv,0,3);
//meshResult=mt.makeTubes(value);

//    float *f = (float*) malloc (texCurv[0].nItem() * sizeof(float));
//    for( uint i = 0; i < texCurv[0].nItem(); i++)
//    {
//    f[i] = (float)(texCurv[0].item(i));
//    }

// copy vertex and faces vector
//    std::vector<double> pointsSP;
//    std::vector<unsigned> facesSP;
//    vector<Point3df> & vert = surface.vertex();
//    vector<AimsVector<uint, 3> > & tri = surface.polygon();
//    pointsSP.resize(3*vert.size());
//    facesSP.resize(3*tri.size());
//
//    for (uint j = 0; j < (int) vert.size(); j++)
//    {
//      pointsSP[3*j] = vert[j][0];
//      pointsSP[3*j+1] = vert[j][1];
//      pointsSP[3*j+2] = vert[j][2];
//    }
//    for (uint j = 0; j < (int) tri.size(); j++)
//    {
//      facesSP[3*j] = tri[j][0];
//      facesSP[3*j+1] = tri[j][1];
//      facesSP[3*j+2] = tri[j][2];
//    }
//
//    // compute adjacence graph
//    geodesic::Mesh meshSP;
//    cout << "compute adjacence graph : ";
//    if (constraintType != 3)
//      meshSP.initialize_mesh_data(pointsSP,facesSP, f,constraintType,strain);
//    else
//      meshSP.initialize_mesh_data(pointsSP,facesSP, NULL ,constraintType,0);
//
//    cout << "done" << endl;
//
//    // compute shortest path
//    cout << "compute shortest path : ";
//
//    std::vector<geodesic::SurfacePoint> sources;
//    sources.push_back(geodesic::SurfacePoint(&meshSP.vertices()[source]));
//
//    std::vector<geodesic::SurfacePoint> targets;
//    targets.push_back(geodesic::SurfacePoint(&meshSP.vertices()[target]));
//
//    printf("indice source = %d target = %d \n",source, target);
//
//    // clear path
//    std::vector<geodesic::SurfacePoint> SPath;
//    SPath.clear();
//
//    if (constraintType != 3)
//    {
//      //writing path in the output texture
//      TimeTexture<float> texOut(1, surface.vertex().size() );
//
//      // dijkstra method
//      geodesic::GeodesicAlgorithmDijkstra *dijkstra_algorithm;
//      dijkstra_algorithm = new geodesic::GeodesicAlgorithmDijkstra(&meshSP);
//
//      std::vector<int> listIndexVertexPathSP;
//      listIndexVertexPathSP.clear();
//
//      geodesic::SurfacePoint short_sources(&meshSP.vertices()[source]);
//      geodesic::SurfacePoint short_targets(&meshSP.vertices()[target]);
//
//      dijkstra_algorithm->geodesic(short_sources,short_targets, SPath, listIndexVertexPathSP);
//
//      //std::vector<int>::iterator ite;
//      reverse(listIndexVertexPathSP.begin(),listIndexVertexPathSP.end());
//      listIndexVertexPathSP.push_back((int)target);
//
//      cout << "shortest path (index vertex) = ";
//      for (unsigned i = 0; i < listIndexVertexPathSP.size(); i++)
//        cout << listIndexVertexPathSP[i] << " " ;
//
//      cout << endl;
//
//      for (unsigned i = 0; i < listIndexVertexPathSP.size(); i++)
//        texOut[0].item(listIndexVertexPathSP[i]) = 1;
//
//      FileOut = FileOut + ".tex";
//      Writer<TimeTexture<float> > texW(FileOut);
//      texW << texOut;
//    }
//    else
//    {
//      geodesic::GeodesicAlgorithmExact *exact_algorithm;
//      exact_algorithm = new geodesic::GeodesicAlgorithmExact(&meshSP);
//
//      //geodesic::GeodesicAlgorithmSubdivision *subdivision_algorithm;
//      //subdivision_algorithm = new geodesic::GeodesicAlgorithmSubdivision(&meshSP,2);
//
//      exact_algorithm->propagate(sources);    //cover the whole mesh
//      exact_algorithm->print_statistics();
//      exact_algorithm->trace_back(targets[0], SPath);
//      //geodesic::print_info_about_path(SPath);
//
//      AimsSurfaceTriangle meshOut, *tmpMeshOut;
//
//      tmpMeshOut = new AimsSurfaceTriangle;
//
//      std::vector< Point3df > vertexList;
//      Point3df newVertex;
//
//      int i;
//
//      for (i = 0; i < SPath.size(); ++i)
//      {
//        newVertex[0] = SPath[i].x();
//        newVertex[1] = SPath[i].y();
//        newVertex[2] = SPath[i].z();
//        cout << "(" << SPath[i].x() << ',' << SPath[i].y() << ',' <<  SPath[i].z() << ")\n";
//        vertexList.push_back(newVertex);
//      }
//
//      for (i = 0; i < vertexList.size() - 1; ++i)
//      {
//        tmpMeshOut =  SurfaceGenerator::sphere(vertexList[i], 0.25 ,20 );
//        SurfaceManip::meshMerge( meshOut, *tmpMeshOut );
//        delete tmpMeshOut;
//
//        tmpMeshOut = SurfaceGenerator::cylinder( vertexList[i],vertexList[i+1], 0.2, 0.2, 12, false, true );
//        SurfaceManip::meshMerge( meshOut, *tmpMeshOut );
//        delete tmpMeshOut;
//      }
//
//      tmpMeshOut =  SurfaceGenerator::sphere(vertexList[i], 0.2 ,10 );
//      SurfaceManip::meshMerge( meshOut, *tmpMeshOut );
//      delete tmpMeshOut;
//
//      FileOut = FileOut + ".mesh";
//      Writer<AimsSurfaceTriangle> wm(FileOut);
//      wm.write(meshOut);
//    }
//    cout << "writing " << FileOut << " done\n";
