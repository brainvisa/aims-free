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

