#!/usr/bin/env python
# -*- coding: utf-8 -*-
from soma import aims, aimsalgo
import exceptions
import numpy
import os

def mergeLabelsFromTexture( tex, labels_list, new_label ):
  """
  inputs:
        tex: labeled texture ( from FreeSurfer or an other )
        labels_list, new_label: you can overwrite numbers ( labels_list ) with your own number ( new_label )
  ouput:
        otex: labeled texture with merged regions
  """
  otex = aims.TimeTexture_S16()
  tex_ar = tex[0].arraydata()
  otex[0].assign( tex_ar )
  otex_ar = otex[0].arraydata()
  for i in labels_list:
    otex_ar[ otex_ar==int(i) ] = new_label
  return otex

def extractLabelsFromTexture( tex, labels_list, new_label ):
  """
  inputs:
        tex: labeled texture ( from FreeSurfer or an other )
        labels_list, new_label: you can overwrite numbers ( labels_list ) with your own number ( new_label )
  output:
        otex: labeled texture with merged regions only
  """
  otex = aims.TimeTexture_S16()
  otex[0].reserve( tex[0].nItem() )
  for i in xrange( tex[0].nItem() ):
    otex[0].push_back(0)
  tex_ar = tex[0].arraydata()
  otex_ar = otex[0].arraydata()
  for i in labels_list:
    otex_ar[ tex_ar==int(i) ] = new_label
  return otex

def connectedComponents( mesh, tex, areas_mode = 0 ):
  """
  inputs:
          mesh
          tex: aimsTimeTexture_S16 (one time step) labeled between 1 and LabelsNb, background = 0, ignored_vertex = -1
          areas_mode: if = 1: computing area measures of the connected components, if = 0: no measure (by default)
  output:
          cctex: onnectedComponentTex: aimsTimeTexture_S16, time step = LabelsNb, for each time step (label in the tex), texture of the connected components corresponding to this label (background = -1, and connected components = values between 1 and ccNb)

          areas_measures = python dictionary, areas_measures[label] = [16.5, 6.0] (numpy array) if label (in tex) has two connected Components 1 and 2 with area = 16.5 and 6.0 respectively, areas are in square mm
  """
  meshVertexNb = int( mesh.vertex().size() )
  print 'Vertices number of mesh: ', meshVertexNb
  areas_measures = {}

  if ( meshVertexNb != tex.nItem() ):
    raise exceptions.ValueError( 'mesh and input texture have not the same dimension...' )

  tex_ar = tex[0].arraydata()
  labels = numpy.unique( tex_ar )
  labelsList = labels.tolist()
  if labelsList.count(0)!=0:
    labelsList.remove(0)
  if labelsList.count(-1)!=0:
    labelsList.remove(-1)
  print 'Labels list: ', labelsList

  dtex = tex[0].arraydata()
  otex = aims.TimeTexture( dtype='S16' )
  cctex = aims.TimeTexture_S16()
  for label in labelsList:
    otex[0].assign( ( dtex == label ).astype( 'int16' ) )
    labelcctex = aimsalgo.AimsMeshLabelConnectedComponent( mesh, otex, 0, 0 )
    if areas_mode:
      ccNb = labelcctex[0].arraydata().max()
      areas_measures[label] = numpy.zeros( ccNb )
      for c in xrange( ccNb ):
        c = c+1
        ccMesh = aims.SurfaceManip.meshExtract( mesh, labelcctex, c )[0]
        ccArea = aims.SurfaceManip.meshArea( ccMesh )
        areas_measures[ label ][ c-1 ] = ccArea
    cctex[ label-1 ].resize( meshVertexNb, 0 )
    cctex[ label-1 ].assign( labelcctex[0].arraydata() )

  if areas_mode:
    return cctex, areas_measures
  else:
    return cctex