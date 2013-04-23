#!/usr/bin/env python
# -*- coding: utf-8 -*-

from optparse import OptionParser
from soma.aimsalgo.meshwatershedtools import watershedWithBasinsMerging
from soma import aims
import sys

def parseOpts(argv):
    parser = OptionParser(
        description='Mesh watershed, plus optional basins merging' )
    parser.add_option( "-i", "--intex", dest="input_texture",
        help="inpput texture" )
    parser.add_option( "-m", "--mesh", dest = "mesh", help = "mesh" )
    parser.add_option( "-o", "--outtex", dest = "outtex",
        help = "output parcellated texture" )
    #parser.add_option( "-g", "--group", dest = "grouping",
        #help = "regroup watershed parcells according to ksize and kdepth, grouping = quantiles or = value", default = None )
    parser.add_option( "-k", "--ksize", dest = "k_size",
        help = "size criteria to regroup parcels", type='int', default = 0 )
    parser.add_option( "-q", "--kdepth", dest = "k_depth",
        help = "depth criteria to regroup parcels", type='float', default = 0 )
    parser.add_option( "-z", "--mode", dest = "mode",
        help = "mode of merging : "\
        "mode = \'and\': merge done if parcel_size < k_size and " \
        "parcel_depth < k_depth \n"\
        "mode = \'or\' idem with or operator : more merges", default = 'and' )
    parser.add_option( "-t", "--threshold", dest = "threshold",
        help = "threshold on the input texture intensities", default = 0.01,
        type='float' )

    return parser, parser.parse_args(argv)

def main():
    parser, (options, args) = parseOpts( sys.argv )
    tex = aims.read( options.input_texture )
    mesh = aims.read( options.mesh )

    print "Computing watershed and merging of basins according to size and depth values..."
    outtex = watershedWithBasinsMerging( tex, mesh, options.k_size,
        options.k_depth, options.threshold, options.mode )
    if options.outtex:
        aims.write( outtex, options.outtex )
    print "Done."

if __name__ == "__main__" : main()

