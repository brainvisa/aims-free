#!/usr/bin/env python
# -*- coding: utf-8 -*-
#  This software and supporting documentation are distributed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
#
# This software is governed by the CeCILL-B license under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL-B license as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-B license and that you accept its terms.
from soma import aims
from soma import aimsalgo
import numpy

class FoldsGraphThickness:
  def __init__(self, fold_graph, lgw_vol, gm_wm_mesh, gm_lcr_mesh):
    self.fold_graph = fold_graph
    self.lgw_vol = lgw_vol
    self.gm_wm_mesh = gm_wm_mesh
    self.gm_lcr_mesh = gm_lcr_mesh
    self.LCR_label = 255
    self.GM_label = 100
    self.WM_label = 200

  def preProcess(self):
    print "preProcess"
    #self.seed = aims.Volume_S16(self.lgw_vol.getSizeX(), self.lgw_vol.getSizeY(),self.lgw_vol.getSizeZ());
    #self.seed.header()["voxel_size"] = self.lgw_vol.header()["voxel_size"]
    seed = - self.lgw_vol
    voxel_size = self.lgw_vol.header()["voxel_size"]

    def printbucket( bck, vol, value ):
      c = aims.RawConverter_BucketMap_VOID_AimsData_S16( False, True, value )
      c.printToVolume( bck.get(), vol )

    seed_label_list = []
    for v in self.fold_graph.vertices():
      try:
        b = v[ 'aims_ss' ]
        index = v[ 'skeleton_label' ]
        seed_label_list.append(int(index))
        printbucket( b, seed, index )
        printbucket( b, self.lgw_vol, self.LCR_label ) #pour que le lcr rentre jusqu au fond des silons
        try:
          b = v[ 'aims_bottom' ]
          printbucket( b, seed, index )
        except:
          pass
        try:
          b = v[ 'aims_other' ]
          printbucket( b, seed, index )
        except:
          pass
      except:
        pass

    f1 = aims.FastMarching()
    print "Voronoi in Grey matter"
    f1.doit(seed, [-self.LCR_label, -self.GM_label], seed_label_list)
    self.voronoi_vol = f1.voronoiVol()
    print "Voronoi in White matter"
    n = numpy.array( voronoi_vol, copy=False )
    n[ n == -1 ] = -100 # avoid value -1 which doesn't seem to work (!)
    del n
    f1.doit(self.voronoi_vol, [-100], seed_label_list)
    self.voronoi_vol = f1.voronoiVol()

    def vorTexCreation(mesh):
      mesh_vertex_set = mesh.vertex()
      mesh_size = mesh_vertex_set.size()
      tex = aims.TimeTexture_S16()
      text = tex[0]
      text.reserve(mesh_size)
      compteur = 0
      for mesh_vertex in mesh_vertex_set:
        compteur = compteur + 1
        mesh_vertex_x = int(round(mesh_vertex[0]/voxel_size[0]))
        mesh_vertex_y = int(round(mesh_vertex[1]/voxel_size[1]))
        mesh_vertex_z = int(round(mesh_vertex[2]/voxel_size[2]))
        text.push_back(self.voronoi_vol.at(int(round(mesh_vertex[0]/voxel_size[0])), int(round(mesh_vertex[1]/voxel_size[1])), int(round(mesh_vertex[2]/voxel_size[2]))))
        #print str(compteur)
      return tex

    print 'extracting meshes'
    self.gm_wm_tex = vorTexCreation(self.gm_wm_mesh)
    self.gm_lcr_tex = vorTexCreation(self.gm_lcr_mesh)
    print 'making thickness map'
    f1 = aims.FastMarching(True)
    dist = f1.doit(self.lgw_vol, [self.GM_label], [self.WM_label, self.LCR_label])
    self.mid_interface_bck = f1.midInterface(self.WM_label, self.LCR_label)
    self.mid_interface = f1.midInterfaceVol(self.WM_label, self.LCR_label)
    self.fastmarching = f1
    return self.voronoi_vol, self.mid_interface

  def graphProcess(self):
    voxel_size = self.lgw_vol.header()["voxel_size"]
    voxel_volume = voxel_size[0]*voxel_size[1]*voxel_size[2]
    coords = self.mid_interface.arraydata()!=-1
    data = self.mid_interface.arraydata()[coords]
    self.fold_graph['thickness_mean'] = data.mean()
    self.fold_graph['thickness_std'] = data.std()
    coords = self.lgw_vol.arraydata()==self.GM_label
    GM_voxels = numpy.where(coords)[0].size
    self.fold_graph['GM_volume'] = GM_voxels * voxel_volume
    coords = self.lgw_vol.arraydata()==self.LCR_label
    LCR_voxels = numpy.where(coords)[0].size
    self.fold_graph['LCR_volume'] = LCR_voxels * voxel_volume
    self.fold_graph['CSF_volume'] = LCR_voxels * voxel_volume

  def vertexProcess(self):
    print "vertexProcess"
    voxel_size = self.lgw_vol.header()["voxel_size"]
    voxel_volume = voxel_size[0]*voxel_size[1]*voxel_size[2]
    coords = self.mid_interface.arraydata()!=-1

    def vertex_mesh(self, v, skel):
      cut_mesh = aims.SurfaceManip.meshExtract( self.gm_wm_mesh, self.gm_wm_tex, skel )[0]
      v['white_surface_area'] = aims.SurfaceManip.meshArea(cut_mesh)
      #aims.GraphManip.storeAims( self.fold_graph, v, 'cortexWhite',  aims.rc_ptr_AimsTimeSurface_3(cut_mesh))
      cut_mesh = aims.SurfaceManip.meshExtract( self.gm_lcr_mesh, self.gm_lcr_tex, skel )[0]
      v['grey_surface_area'] = aims.SurfaceManip.meshArea(cut_mesh)
      #aims.GraphManip.storeAims( self.fold_graph, v, 'cortexHemi',  aims.rc_ptr_AimsTimeSurface_3(cut_mesh))

    print 'calculating vertices thickness'
    fat = aims.FoldGraphAttributes( self.voronoi_vol, self.fold_graph )
    fat.thickness( self.mid_interface_bck, self.voronoi_vol )

    print 'calculating GM and CSF volumes'
    fat.greyAndCSFVolumes( self.lgw_vol, self.voronoi_vol )

    print 'calculating surfaces areas'
    compteur = 0
    n = len(self.fold_graph.vertices())
    for v in self.fold_graph.vertices():
      try:
        skel = v['skeleton_label']
        print "skel : " + str(skel) + " compteur : " + str(compteur) + '/' + str(n)
        compteur = compteur + 1
        vertex_mesh(self, v, skel)
      except:
        pass
    return self.fold_graph

  def process(self):
    self.preProcess()
    self.graphProcess()
    self.vertexProcess()
    return self.fold_graph
