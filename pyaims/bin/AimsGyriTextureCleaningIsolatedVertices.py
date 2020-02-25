#!/usr/bin/env python
###############################################################################
# This software and supporting documentation are distributed by CEA/NeuroSpin,
# Batiment 145, 91191 Gif-sur-Yvette cedex, France. This software is governed
# by the CeCILL license version 2 under French law and abiding by the rules of
# distribution of free software. You can  use, modify and/or redistribute the
# software under the terms of the CeCILL license version 2 as circulated by
# CEA, CNRS and INRIA at the following URL "http://www.cecill.info".
###############################################################################

"""
This script does the following:
*
*

Author: Sandrine Lefranc, 2015
"""

#----------------------------Imports-------------------------------------------


# python system module
from __future__ import print_function
from __future__ import absolute_import
import argparse
import textwrap
import sys

# aims module
from soma import aims
from soma.aims.texturetools import clean_gyri_texture
from soma.aims.texturetools import find_wrong_labels


#----------------------------Functions-----------------------------------------


def parse_args(argv):
    """Parses the given list of arguments."""

    # creating a parser
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=textwrap.dedent("""\
            -------------------------------------------------------------------
            Clean a label AimsTimeTexture from isolated vertices
            -------------------------------------------------------------------
            """))

    #adding arguments
    parser.add_argument("gyriseg", type=str, help="input gyri texture")
    parser.add_argument("mesh", type=str, help="input mesh")
    parser.add_argument("ogyriseg", type=str,
                        help="gyri texture with one connected component per "
                             "gyrus")
    # parsing arguments
    return parser, parser.parse_args(argv)


def main():
    # load the arguments of parser (delete script name: sys.arg[0])
    parser, args = parse_args(sys.argv[1:])

    # load the files with aims
    tex = aims.read(args.gyriseg)
    mesh = aims.read(args.mesh)

    cont = True
    count = 0
    while cont and count < 10:
        print("clean up number ", str(count + 1), " :")
        tex = clean_gyri_texture(mesh, tex)
        wrong_labels = find_wrong_labels(mesh, tex)
        cont = False
        count += 1
        if len(wrong_labels) > 0:
            cont = True
    # write the file on the disk
    aims.write(tex, args.ogyriseg)


#----------------------------Main program--------------------------------------


if __name__ == "__main__":
    main()
