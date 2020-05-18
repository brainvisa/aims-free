#!/usr/bin/env python
# -*- coding: utf-8 -*-
#  This software and supporting documentation are distributed by
#      Institut Federatif de Recherche 49
#      CEA/NeuroSpin, Batiment 145,
#      91191 Gif-sur-Yvette cedex
#      France
#
# This software is governed by the CeCILL license version 2 under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the 
# terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
# knowledge of the CeCILL license version 2 and that you accept its terms.

from __future__ import absolute_import
from __future__ import print_function
import os
try:
    import dicom
except ImportError as e:
    raise Exception("DICOM aggregator requires pydicom.")

class DicomAggregator( object ):
    def __init__( self ):
        self._dicom_sources = []
        self.modalities = []
        self.frameOfReferenceUID = {}

    def add_dicom_sources( self, sources ):
        for source in sources:
            if os.path.isdir( source ):
                for root, dirs, files in os.walk( source ):
                    for name in files:
                        self._dicom_sources.append( os.path.join( root, name ) )
            else:
                self._dicom_sources.append( source )
        self._dicom_sources = list( set( self._dicom_sources ) )

    def aggregate( self ):
        self._aggregate_sources = {}
        serie_sop_uids = {}
        positions_and_orientations = []
        for f in self._dicom_sources:
            try:
                ds = dicom.read_file( f )
            except Exception as e:
                print(e)
                continue
            
            if len(self.modalities) != 0 and \
               ds.Modality not in self.modalities:
                continue
            
            try:
                serie_uid = ds.SeriesInstanceUID
            except Exception as e:
                print(e)
                continue
            
#            try:
#                frameOfRefUID = ds.FrameofReferenceUID
#                if frameOfRefUID not in self.frameOfReferenceUID:
#                    self.frameOfReferenceUID[frameOfRefUID] = []
#                if serie_uid not in self.frameOfReferenceUID[frameOfRefUID]:
#                    self.frameOfReferenceUID[frameOfRefUID].append(serie_uid)
#            except:
#                pass
            
            if serie_uid not in self._aggregate_sources:
                self._aggregate_sources[ serie_uid ] = []
                serie_sop_uids[ serie_uid ] = []

            try:
                position_and_orientation = ( ds.ImagePositionPatient,
                                             ds.ImageOrientationPatient,
                                             ds.FrameReferenceTime )
            except Exception as e:
                print(e)
                position_and_orientation = None
            
            sop_uid = ds.SOPInstanceUID
            if not sop_uid in serie_sop_uids[ serie_uid ] and \
               not position_and_orientation in positions_and_orientations:
                serie_sop_uids[ serie_uid ].append( sop_uid )
                self._aggregate_sources[ serie_uid ].append( f )
                
#                if position_and_orientation is not None:
#                    positions_and_orientations.append( position_and_orientation )
