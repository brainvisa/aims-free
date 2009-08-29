#!/usr/bin/env python

# 1. protect C/C++ preprocessor directives: # -> //!#!
# 2. replace macros:
# general shape: %Template<num><macro>% (future evolution)
# %Template1%: C++ type
# %Template1typecode%: DataTypeCode
# %Template1pyFromC%: PyInt_FromLong or other
# %Template1defScalar%: #define PYAIMS_SCALAR
# %Template1defef%: null for numeric types, * for other types
# %Template1new%: null for numeric types, new T for other types
# %#...%: generate regular preprocessor macros
# ...
# 3. preprocess using cpp
# 4. put back C/C++ preprocessor directives: //!#! -> #


import sys, os, types, stat, shutil, filecmp, glob, re
from optparse import OptionParser
import subprocess

parser = OptionParser( description = 'Preprocess a template file to generate '
                       'typed SIP inpuyt files' )
parser.add_option( '-s', '--sourcepath', dest='sourcepath',
                   help='source path to pyaims SIP directory containing '
                   'maketemplate.py [default: same as this program]',
                   default=os.path.dirname( sys.argv[0] ) )
parser.add_option( '-i', '--input', dest='input', 
                   help='input generated types file '
                   '[default: generatedtypes.py]',
                   default='generatedtypes.py' )
parser.add_option( '-p', '--tpl', dest='tpldir', 
                   help='directory where .tpl files are', default='' )
parser.add_option( '-t', '--types', dest='typessub', action='append', 
                   help='types substitution file [default: typessub.py in '
                   'the current directory and/or in sourcepath]' )
parser.add_option( '-o', '--output', dest='output',
                   help='output directory for generated .sip files '
                   '[default:same as sources]', default=None )
parser.add_option( '-c', '--cleanup', dest='cleanup', action='store_true', 
                   help='cleanup obsolete .sip files in the output directory '
                   '[default:false]', default=False )
parser.add_option( '-l', '--listing', dest='listFilesOnly',
                   action='store_true',
                   help='change the display : show the generated file list'
                   '[default:false]', default=False )

(options, args) = parser.parse_args()
if args:
  parser.parse( [ '-h' ] )

if not options.typessub:
  p = [ os.path.join( options.sourcepath, 'typessub.py' ), 'typessub.py' ]
  options.typessub = filter( os.path.exists, p )

pyaimssip = options.sourcepath
sys.path.insert( 0, '.' )
sys.path.insert( 1, pyaimssip )
from maketemplate import makeTemplate
import maketemplate

def convert_string_to_int( s ):
  '''
  Allow to convert string with digit followed by non digits
  Useful to buil Qt version such as 3.3.8b
  '''
  for i in xrange( len( s ) ):
    if not s[i].isdigit():
      s = s[ :i ]
      break
  return int( s )

# determine Qt version
try:
  qtdir = os.getenv( 'QTDIR' )
  if qtdir:
    moc = os.path.join( qtdir, 'bin', 'moc' )
    if not os.path.exists( moc ):
      moc = 'moc'
  else:
    moc = 'moc'
  #f = os.popen3( moc + ' -v' )
  #l = f[2].read()
  #del f
  l = subprocess.Popen( [ moc, '-v' ], stdout=subprocess.PIPE,
    stderr=subprocess.PIPE ).communicate()[1]
  x = re.search( '^.*\(Qt ([^\)]*)\)$', l ).group(1)
  qt_version = [ convert_string_to_int(k) for k in x.split( '.' ) ]
except Exception, e:
  print e
  qt_version = [ 3, 0, 0 ] # Qt not available ?

# read generatedtypes file
# expected to fill in the 'todo' dictionary variable
execfile( options.input, globals(), globals() )

if options.tpldir=='':
  dir_name=os.path.dirname(options.input)
else:
  dir_name=options.tpldir

#read typessub files
typesmtime = 0
for x in options.typessub:
  typesmtime = max( typesmtime, os.stat( x )[ stat.ST_MTIME ] )
  execfile( x, globals(), globals() )
typesmtime = max( typesmtime,
  os.stat( maketemplate.__file__ )[ stat.ST_MTIME ] )
if os.path.exists( sys.argv[0] ):
  typesmtime = max( typesmtime, os.stat( sys.argv[0] )[ stat.ST_MTIME ] )

outfiles = []
allok = True

for file, tps in todo.items():
  # print file, ':', tps
  infile = os.path.join( dir_name, file + '.tpl' )
  if not os.path.exists( infile ):
    infile = os.path.join( pyaimssip, infile )
    if not os.path.exists( infile ):
      infile = os.path.join( pyaimssip, file + '.tpl' )
  if options.output is not None:
    ofilebase = os.path.join( options.output, os.path.basename( file ) )
  else:
    ofilebase = file
  for x in tps:
    if type( x ) is types.StringType:
      templates = { 'Template1' : x }
      ts = typessub[ x ].get( 'typecode' )
      if not ts:
        ts = x
      ofile = ofilebase + '_' + ts + '.sip'
    else:
      i = 1
      ofile = ofilebase
      templates = {}
      for y in x:
        templates[ 'Template' + str( i ) ] = y
        i += 1
        ts = typessub[ y ].get( 'typecode' )
        if not ts:
          ts = y
        ofile += '_' + ts
      ofile += '.sip'
    outfiles.append( ofile )
    try:
      # print 'templates:', templates
      done = 0
      if os.path.exists( ofile ):
        otmpfile = ofile + '.tmp'
        s1 = os.stat( infile )[ stat.ST_MTIME ]
        s2 = os.stat( ofile )[ stat.ST_MTIME ]
        if s1 <= s2 and typesmtime < s2:
          done = 1
          if not options.listFilesOnly:
            print >> sys.stderr, 'skipping', ofile, '- up to date'
      else:
        otmpfile = ofile
      if not done:
        if not options.listFilesOnly:
            sys.stdout.write( 'generating ' + ofile )
        makeTemplate( infile, otmpfile, typessub, templates,
          quiet = options.listFilesOnly )
        if ofile != otmpfile:
          if not filecmp.cmp( ofile, otmpfile ):
            shutil.copyfile( otmpfile, ofile )
            if not options.listFilesOnly:
                print ' - differs'
          else:
            if not options.listFilesOnly:
                print ' - unchanged'
            # copy it anyway because sip.py will take care of it
            shutil.copyfile( otmpfile, ofile )
          os.unlink( otmpfile )
        else:
          if not options.listFilesOnly:
            print
    except Exception, e:
      print >> sys.stderr, 'error in generation of', ofile, ':'
      print >> sys.stderr, e
      allok = False

if options.listFilesOnly:
    print ";".join(outfiles).replace( '\\', '/' )

if allok and options.cleanup and options.output:
  if not options.listFilesOnly:
    print 'cleanup obsolete files...'
  files = glob.glob( os.path.join( options.output, '*.sip' ) )
  for f in files:
    if f not in outfiles \
           and stat.S_IMODE( os.stat( f )[ stat.ST_MODE ] ) & 0200:
      if not options.listFilesOnly:
        print 'deleting', f
      os.unlink( f )

if not allok:
  sys.exit( 1 )
