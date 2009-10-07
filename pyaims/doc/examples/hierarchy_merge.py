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

# read 2 hierarchies as h1 and h2, and merge them into h3

h1 = aims.read( aims.carto.Paths.shfjShared() \
  + '/nomenclature/hierarchy/sulcal_root_colors.hie' )
h2 = aims.read( aims.carto.Paths.shfjShared() \
  + '/nomenclature/hierarchy/stereotaxy.hie' )

h3 = aims.Hierarchy()
h3.setSyntax( 'hierarchy' )
h3[ 'graph_syntax' ] = 'RoiArg' # this hie applies to RoiArg graphs

# create 2 main nodes
h3.insert( aims.Tree( True, 'fold_name' ) )
h3.children()[0]['name'] = 'sulci'
h3.insert( aims.Tree( True, 'fold_name' ) )
h3.children()[1]['name'] = 'stereotaxy'

for c in h1.children():
  h3.children()[0].insert( c )

for c in h2.children():
  h3.children()[1].insert( c )

outputfile = '/tmp/mergedhierarchy.hie'
aims.write( h3, outputfile )

