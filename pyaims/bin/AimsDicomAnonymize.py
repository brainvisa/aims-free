#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
from soma import argparse
from soma.dicom import anonymizer

parser = argparse.ArgumentParser(description='Anonymize input DICOM')    
parser.add_argument('-i', '--input', help='input DICOM file/directory')
parser.add_argument( '-o', '--output', help='output file/directory' )
args = parser.parse_args()
if args.input is None or args.output is None:
    parser.parse_args(['-h'])

anonymizer.anonymize(dicom_in=args.input,
                     dicom_out=args.output)
