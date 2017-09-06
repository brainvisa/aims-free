#!/usr/bin/env python

from __future__ import print_function

from soma import aims
from argparse import ArgumentParser
import numpy as np
import sys
import six
import gc

# --cifti /volatile/riviere/hcp_course/HCP_2015_Course/home/hcpcourse/day3-wednesday/Diffusion_practicals/100307/MNINonLinear/Connectomes/Conn3_HCPcourse_log.dconn.nii
# meshes:
# /neurospin/population/HCP/S500-1/100307/T1w/fsaverage_LR32k/100307.L.inflated.32k_fs_LR.surf.gii
# /neurospin/population/HCP/S500-1/100307/T1w/fsaverage_LR32k/100307.R.inflated.32k_fs_LR.surf.gii
# gyri labels:
# --gyri /neurospin/population/HCP/S500-1/100307/MNINonLinear/fsaverage_LR32k/100307.L.aparc.32k_fs_LR.label.gii
# --gyri /neurospin/population/HCP/S500-1/100307/MNINonLinear/fsaverage_LR32k/100307.R.aparc.32k_fs_LR.label.gii
# --gyrus L_postcentral

def parse_args(argv):

    parser = ArgumentParser(
        description="Extract a submatrix from a CIFTI file for one gyrus or "
            "several gyri. The extracted submatrix is in the shape currently "
            "expected by the Constellation toolbox and Anatomist viewers.")

    parser.add_argument("-i", "--cifti", help="input CIFTI file")
    parser.add_argument("--gyri", action="append",
                        help="input gyri textures, typically FreeSurfer gyri. "
                        "Several may be specified (left, right)")
    parser.add_argument("--gyrus", action="append",
                        help="gyrus to extract. Several may be specified. May "
                        "be a name or a number. If a name is used, the label "
                        "mapping is expected in the gyri textures header as "
                        "Gifti 'GIFTI_labels_table' attribute.")
    parser.add_argument("-o", "--output", help="output gyrus matrix")

    return parser, parser.parse_args(argv)

def main():

    parser, args = parse_args(sys.argv[1:])
    cifti_filename = args.cifti
    gyri_filenames = args.gyri
    gyrus_list = args.gyrus
    output_gyrus_matrix = args.output

    if not cifti_filename or not gyri_filenames or not gyrus_list \
            or not output_gyrus_matrix:
        parse_args(['-h'])

    mat = aims.read(cifti_filename)

    # read gyri texture
    gyri = []
    for gyri_filename in gyri_filenames:
      gyri.append(aims.read(gyri_filename))

    c = aims.CiftiTools(mat)

    # get surface ROI textures
    troi = c.roiTextureFromDimension(0)

    # get indices of given gyrus
    labels_map = []
    labels_inv_map = []
    for i in range(len(gyri)):
        if 'GIFTI_labels_table' in gyri[i].header():
            labels_map.append(eval(repr(gyri[i].header()[
                'GIFTI_labels_table'])))
            labels_inv_map.append(dict(
                [(y['Label'], x) for x, y in six.iteritems(labels_map[i])]))
        else:
            labels_map.append({})
            labels_inv_map.append({})
    gyri_vert_l = []
    for gyrus in gyrus_list:
        gyrdef = [(i, labels_inv_map[i].get(gyrus))
                  for i in range(len(labels_inv_map))
                  if gyrus in labels_inv_map[i]]
        if len(gyrdef) != 0:
            tex, gyrus_num = gyrdef[0]
            gyrus_indices = np.where(gyri[tex][0].arraydata() == gyrus_num)[0]
            gyri_vert_l.append((tex, gyrus_indices))
        else:
            try:
                gyrus_num = int(gyrus)
            except ValueError:
                print("Gyrus %s not found" % gyrus)
                continue
            texs = [gyrus_num in roi[0].arraydata() for roi in gyri]
            for tex in len(texs):
                if texs[tex]:
                    gyrus_indices = np.where(
                        gyri[tex][0].arraydata() == gyrus_num)[0]
                    gyri_vert_l.append((tex, gyrus_indices))

    # concatenate by texture/surface
    gyri_vert = []
    for tex, gyrus_indices in gyri_vert_l:
        while len(gyri_vert) <= tex:
            gyri_vert.append([])
        gyri_vert[tex] = np.hstack((gyri_vert[tex], gyrus_indices))


    # get corresponding indices in matrix
    row_indices = np.hstack(
        [c.getIndicesForSurfaceIndices(0, tex, gyrus_indices)
         for tex, gyrus_indices in enumerate(gyri_vert)])
    assert(len(row_indices)
           == sum([len(gyrus_indices) for gyrus_indices in gyri_vert]))
    print('row_indices:', len(row_indices))
    total_vertices = sum([len(troi[i][0]) for i in range(len(troi))])
    print('total_vertices:', total_vertices)
    col_vertices = [(i, np.where(troi[i][0].arraydata() != 0)[0])
                    for i in range(len(troi))]
    col_indices = np.hstack([c.getIndicesForSurfaceIndices(1, i, vert)
                              for i, vert in col_vertices])
    print('col_indices:', len(col_indices))
    vert_nums = [0]
    for roi in troi[:-1]:
        vert_nums.append(vert_nums[-1] + len(roi[0]))

    new_col_indices = np.hstack(([vert + vert_nums[i]
                                  for i, vert in col_vertices]))

    # allocate gyrus matrix
    gyr_mat = aims.SparseOrDenseMatrix(len(row_indices), total_vertices)

    for i, row in enumerate(row_indices):
        print('row:', row, ':', i, '/', len(row_indices), 'dense:',
              gyr_mat.isDense())
        mat.readRow(row)
        row_data = mat.getRow(row).arraydata()
        #row_data[np.isnan(row_data)] = 0
        row_data[np.isinf(row_data)] = 0
        mat.freeRow(row)
        new_row_data = np.zeros((total_vertices, ))
        new_row_data[new_col_indices] = row_data
        gyr_mat.setRow(i, new_row_data)
        if i % 20 == 0:
            gyr_mat.muteToOptimalShape()
            gc.collect()
    gyr_mat.muteToOptimalShape()
    gyr_mat.header()['row_labels'] = gyrus_list

    # save gyrus matrix
    aims.write(gyr_mat, output_gyrus_matrix)


if __name__ == '__main__':
    main()


