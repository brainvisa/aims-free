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

# .APC (commissure coordinates) IO and other tools

from soma import aims
import types

def apcRead( filename, imagefile = None ):
    f = open( filename )
    lines = f.readlines()
    f.close()
    apcdict = {}
    ac = []
    pc = []
    ip = []
    acm = []
    pcm = []
    ipm = []
    for l in lines:
        if l[:3] == 'AC:':
            apcdict[ 'ac' ] = map( lambda x: int(x), l.split()[1:4] )
        elif l[:3] == 'PC:':
            apcdict[ 'pc' ] = map( lambda x: int(x), l.split()[1:4] )
        elif l[:3] == 'IH:':
            apcdict[ 'ip' ] = map( lambda x: int(x), l.split()[1:4] )
        elif l[:5] == 'ACmm:':
            acm = map( lambda x: float(x), l.split()[1:4] )
            apcdict[ 'acmm' ] = acm
        elif l[:5] == 'PCmm:':
            pcm = map( lambda x: float(x), l.split()[1:4] )
            apcdict[ 'pcmm' ] = pcm
        elif l[:5] == 'IHmm:':
            ipm = map( lambda x: float(x), l.split()[1:4] )
            apcdict[ 'ipmm' ] = ipm
        else:
            comment = apcdict.get( 'comment', None )
            if comment is None:
                comment = []
                apcdict[ 'comment' ] = comment
            comment.append( l.strip() )

    if len( acm ) == 3 and len( pcm ) == 3 and len( ipm ) == 3:
        return apcdict
    if imagefile is None:
        return apcdict

    if type( imagefile ) in types.StringTypes:
        f = aims.Finder()
        if f.check( imagefile ):
            image = f.header()
        else:
            raise IOError( 'Cannot read header of: ' + filename )
    else:
        try:
          image = imagefile.header()
        except: # otherwise it is considered to already be a header
          image = imagefile
          if not hasattr( image, 'has_key' ) \
              or not image.has_key( 'voxel_size' ):
            image = { 'voxel_size' : imagefile } # directly the VS list ?
    vs = image[ 'voxel_size' ]
    if len( acm ) != 3:
        acm = [ ac[0] * vs[0], ac[1] * vs[1], ac[2] * vs[2] ]
        apcdict[ 'acmm' ] = acm
    if len( pcm ) != 3:
        pcm = [ pc[0] * vs[0], pc[1] * vs[1], pc[2] * vs[2] ]
        apcdict[ 'pcmm' ] = pcm
    if len( ipm ) != 3:
        ipm = [ ip[0] * vs[0], ip[1] * vs[1], ip[2] * vs[2] ]
        apcdict[ 'ipmm' ] = ipm
    try:
        apcWrite( filename, apcdict )
    except:
        # could not write modifications
        pass

    return apcdict


def apcWrite( apcdict, filename ):
    ac = apcdict[ 'ac' ]
    pc = apcdict[ 'pc' ]
    ip = apcdict[ 'ip' ]
    f = open( filename, 'w' )
    f.write( 'AC: ' + ' '.join( [ str(x) for x in ac ] ) + '\n' )
    f.write( 'PC: ' + ' '.join( [ str(x) for x in pc ] ) + '\n' )
    f.write( 'IP: ' + ' '.join( [ str(x) for x in ip ] ) + '\n' )
    comment = apcdict.get( 'comment', None )
    acm = apcdict.get( 'acmm', None )
    pcm = apcdict.get( 'pcmm', None )
    ipm = apcdict.get( 'ipmm', None )
    if comment is None:
        comment = []
        if acm and pcm and ipm:
            comment = [ '# The previous coordinates, used by the system, ' \
            'are defined in voxels',
            '# They stem from the following coordinates in millimeters:' ]
    for l in comment:
        f.write( l + '\n' )
    if acm and pcm and ipm:
        f.write( 'ACmm: ' + ' '.join( [ str(x) for x in acm ] ) + '\n' )
        f.write( 'PCmm: ' + ' '.join( [ str(x) for x in pcm ] ) + '\n' )
        f.write( 'IPmm: ' + ' '.join( [ str(x) for x in ipm ] ) + '\n' )

    f.close()

def apcTransform( apcdict, transform, outimagevoxelsize ):
    if type( outimagevoxelsize ) in types.StringTypes:
        f = aims.Finder()
        if f.check( outimagevoxelsize ):
            outhdr = f.header()
        else:
            raise IOError( 'Cannot read header of: ' + outimagevoxelsize )
    else:
        try:
          outhdr = outimagevoxelsize.header()
        except: # otherwise it is considered to already be a header
          outhdr = outimagevoxelsize
          if not hasattr( outhdr, 'has_key' ) \
              or not outhdr.has_key( 'voxel_size' ):
            outhdr = { 'voxel_size' : outhdr } # directly the VS list ?
    outvs = outhdr[ 'voxel_size' ]
    for p in ( 'ac', 'pc', 'ip' ):
        px = apcdict.get( p, None )
        pmm = apcdict[ p + 'mm' ]
        pmm2 = transform.transform( pmm )
        apcdict[ p + 'mm' ] = pmm2
        vs = [ pmm[i] / px[i] for i in xrange(3) ]
        px2 = [ int( round( x / y ) ) for x,y in zip( pmm2, outvs ) ]
        apcdict[ p ] = px2

def apcFileTransform( inAPCfilename, outAPCfilename, transform,
    outimagevoxelsize, imagefile = None ):
    apc = apcRead( inAPCfilename, imagefile )
    apcTransform( apc, transform, outimagevoxelsize )
    apcWrite( apc, outAPCfilename )

