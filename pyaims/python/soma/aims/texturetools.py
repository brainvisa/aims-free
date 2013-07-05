#!/usr/bin/env python
# -*- coding: utf-8 -*-
from soma import aims

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
  