#!/usr/bin/env python

import sys, os, re, sipconfig
from optparse import OptionParser
import subprocess, platform

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

def makeTemplate( infile, outfile, types, templates = {}, cpp = 'cpp -C',
  quiet=0 ):
  #print 'input :', infile
  #print 'output:', outfile
  #print 'types :', types

  fi = open( infile )
  fo = open( outfile, 'w' )
  if cpp:
    c = sipconfig.Configuration()
    cppcmd = [ cpp, '-DSIP_VERSION=' + '0x%06x' % c.sip_version ]
    # determine Qt version
    try:
      qtdir = os.getenv( 'QTDIR' )
      if qtdir:
        moc = os.path.join( qtdir, 'bin', 'moc' )
        if not os.path.exists( moc ):
          moc = 'moc'
      else:
        moc = 'moc'
      l = subprocess.Popen( [ moc, '-v' ], stdout=subprocess.PIPE,
        stderr=subprocess.PIPE ).communicate()[1]
      x = re.search( '^.*\(Qt ([^\)]*)\).*$', l ).group(1)
      qv = [ convert_string_to_int(k) for k in x.split( '.' ) ]
      qver = qv[0] * 0x10000 + qv[1] * 0x100 + qv[2]
      cppcmd.append( '-DQT_VERSION=' + hex( qver ) )
      #print 'Qt version:', hex( qver )
    except Exception, e:
      print e
      #pass # Qt not available ?
    if not quiet:
        print ' '.join( cppcmd )
    #fo2, cppout = os.popen2( cppcmd )
    if platform.system() == 'Windows':
      p = subprocess.Popen( cppcmd, shell=True, bufsize=0,
                stdin=subprocess.PIPE, stdout=subprocess.PIPE )
    else:
      p = subprocess.Popen( cppcmd, shell=True, bufsize=0,
                stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True )
    fo2, cppout = ( p.stdin, p.stdout )

  templatere = re.compile( '(%(Template[0-9]+)([^%]*)%)' )
  disableprere = re.compile( '(^\s*)(#)(.*$)', re.M )
  enableprere = re.compile( '(^\s*)(//!#!)(.*$)' )
  preprocre = re.compile( '(^\s*)(%#)(.*)%$' )
  removeprere = re.compile( '^\s*#.*$' )

  for line in fi.xreadlines():
    lo = line
    templ = templatere.search( lo )
    while templ:
      tempn = templ.group(2)
      tempv = templ.group(3)
      # print 'fount template:', tempn, tempv
      t = templates.get( tempn )
      val = None
      if t:
        if not tempv:
          val = t
        else:
          tv = types.get( t )
          if tv:
            val = tv.get( tempv )
      if val is not None:
        # print templ.group(0), '->', val
        pos = templ.start(1) + len(val)
        # print lo, 'pos:', pos
        lo = templatere.sub( val, lo, 1 )
      else:
        pos = templ.end(1)
      templ = templatere.search( lo, pos )

    lo = disableprere.sub( '\\1//!#!\\3', lo )
    lo = preprocre.sub( '\\1#\\3', lo )
    lo = lo.replace( '%%', '' ) # remove empty macros
    # feed the C preprocessor
    if not cpp:
      lo = enableprere.sub( '\\1#\\3', lo )
      fo.write( lo )
    else:
      fo2.write( lo )

  if cpp:
    #cppout = p.communicate()[0]
    fo2.close()
    for line in cppout.xreadlines():
      lo = line
      if removeprere.match( lo ):
        lo = ''
      lo = enableprere.sub( '\\1#\\3', lo )
      fo.write( lo )

  fo.close()
  fi.close()



if __name__ == '__main__':
  parser = OptionParser( description = 'SIP template generation' )
  parser.add_option( '-i', '--input', dest='infile', 
                   help='input .tpl SIP-like template' )
  parser.add_option( '-o', '--output', dest='outfile', help='output SIP file' )
  parser.add_option( "-p", "--preprocess", dest='preprocess', 
                     action="store_true", help="use C preprocessor" )
  parser.add_option( "-P", "--preprocessor", dest='preprocessor', 
                     help="C preprocessor command (default: 'cpp -C')" )
  parser.add_option( "-t", "--templates", dest='templates', 
                     action='append', help="templates list" )
  parser.add_option( "-s", "--subs", dest='subs', 
                     help="substitutions file (python file)" )

  (options, args) = parser.parse_args()
  if not options.infile or not options.outfile or len( args ) % 2 != 0:
    parser.parse_args( [ '-h' ] )

  infile = options.infile
  outfile = options.outfile
  cpp = options.preprocess
  cppc = options.preprocessor
  if cpp and not cppc:
    cppc = 'cpp -C'
  elif not cpp:
    cppc = None

  types = {}
  templates = {}
  if options.templates:
    if len( options.templates ) % 2 != 0:
      print 'template arguments go by pairs (key, value)'
      sys.exit( 1 )
    for i in xrange( len( options.templates ) / 2 ):
      templates[ options.templates[ i*2 ] ] = options.templates[i*2+1]

  #print 'templates:', options.templates
  #print 'subs:', options.subs

  if options.subs:
    execfile( options.subs )
    types = typessub

  for i in xrange( len( args ) / 2 ):
    types[ args[ i*2 ] ] = args[i*2+1]

  makeTemplate( infile, outfile, types, templates, cppc )
