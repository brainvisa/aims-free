#!/usr/bin/env python
############################################################################
# This software and supporting documentation are distributed by
# CEA/NeuroSpin, Batiment 145, 91191 Gif-sur-Yvette cedex, France.
# This software is governed by the CeCILL license version 2 under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL license version 2 as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
############################################################################

"""
This script does the following:
*
*

Main dependencies: PyAims library

Author: Sandrine Lefranc, 2015
"""


#----------------------------Imports-------------------------------------------


# python modules
import sys
import argparse
import textwrap

# soma module 
from soma.aimsalgo.meshwatershedtools import watershedWithBasinsMerging
from soma import aims


#----------------------------Functions-----------------------------------------


def parse_args(argv):
    """Parses the given list of arguments."""

    # creating a parser
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=textwrap.dedent("""\
            -------------------------------------------------------------------
            
            -------------------------------------------------------------------
            """))

    # adding arguments
    parser.add_argument(
        "normedprofile", type=str, help="input normed connectivity profile")
    parser.add_argument(
        "whitemesh", type=str, help = "input white mesh")
    parser.add_argument(
        "basins", type=str, help = "output basins segmentation")
    parser.add_argument("--basins_size",
                        type=int,
                        default=10,
                        help = "size criteria to regroup smallest parcels")
    parser.add_argument("--basins_depth",
                        type=float,
                        default=0.05,
                        help = "depth criteria to regroup parcels")
    parser.add_argument("--mode", type=str, default='and',
        help = "mode of merging : "\
        "mode = \'and\': merge done if parcel_size < k_size and " \
        "parcel_depth < k_depth \n"\
        "mode = \'or\' idem with or operator : more merges")
    parser.add_argument("--threshold",
                        type=float,
                        default=0.01,
                        help = "threshold on the input texture intensities")

    # parsing arguments
    return parser, parser.parse_args(argv)


def main():
    # Load the arguments of parser (delete script name: sys.arg[0])
    arguments = (sys.argv[1], sys.argv[2], sys.argv[3])
    parser, args = parse_args(arguments)
    
    # load the texture and its mesh
    tex = aims.read(args.normedprofile)
    mesh = aims.read(args.whitemesh)

    print "Computing watershed"
    outtex = watershedWithBasinsMerging(tex, mesh, args.basins_size, 
                                        args.basins_depth, args.threshold, 
                                        args.mode)
    print "Merging of basins according to size and depth values"

    # write the watershed texture with two criteria on the basins    
    aims.write(outtex, args.basins)


#----------------------------Main program--------------------------------------


if __name__ == "__main__" :
    main()
