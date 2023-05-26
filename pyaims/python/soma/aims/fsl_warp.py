# FSL warp fields conversion to AIMS


from soma import aims, aimsalgo
import numpy as np


def fsl_to_aims_warp_field(W, input_hdr, output_hdr):
    '''
    FSL warp fields conversion to AIMS

    Deals with FSL fields output from fnirt, as relzative deformation fields
    (not spline coefs).
    FSL fields code the dispalcement in mm from a source point in Nifti disk
    space coordinates of the Destination (output) image to the disk space
    coordinates (in mm, thus) of the source (input) image

    AIMS is similar, but codes from/to its LPI oriented image coordinates.

    After this conversion, the field may be used using AimsApplyTransform, or
    in AIMS programs as a FfdTransformation.

    Notations:

    Say Nifti images are in LAS orientation (it's an example, no matter).
    Aims works in LPI.
    i prefix/suffix is for input image spaces
    o prefix/suffix is for output image spaces
    v is for voxel coordinates (int)
    m is for mm coordinates (float)
    LPI / LAS are the aims / disk orientations identifiers

    thus:
    PiLASm: position in input image in disk orientation, in mm
    PoLPIv: position in output image in aims orientation, in voxels
    VSi, VSo: voxel sizes transform matrices (aims LPI orientation)
    VSiLAS, VSoLAS: voxel sizes transform matrices in disk LAS orientation
    S2Mi, S2Mo: storage-to-memory matrices, in voxels ('storage_to_memory'
    fields of aims volumes)
    W: FSL warping field image (the "main" input of this script)
    WLPI: aims warping field image (what we want, the output of this script)
        displacement in mm from output LPI space to input LPI space

    We need also:
    input image space grid (dims, voxel size, S2Mi)
    output image space grid (also called "reference" in applywarp and in
      AimsApplyTransform)
    we actually need only headers for these images.

    Math part::

    PiLASm = W.PoLASv + PoLASm = (W + VSoLAS).PoLASv
    PoLPIv = S2Mo.PoLASv, so PoLASv = S2Mo^-1.PoLPIv
    PiLPIv = S2Mi.PiLASv = S2Mi.VSiLAS^-1.PiLASm
    PiLPIv = S2Mi.VSiLAS^-1.(W + VSoLAS).PoLASv
    PiLPIm = VSi.S2Mi.VSiLAS^-1.(W + VSoLAS).PoLASv
    we want:
    PiLPIm = WLPI.PoLPIv + PoPLIm (WPLI is the relative warp)
    combine:
    WLPI.PoLPIv = VSi.S2Mi.VSiLAS^-1.(W + VSoLAS).PoLASv - PoPLIm
    replace PoLASv and PoPLIm using PoPLIv:
    WLPI.PoLPIv = [VSi.S2Mi.VSiLAS^-1.(W + VSoLAS).S2Mo^-1 - VSo].PoLPIv

    WLPI = VSi.S2Mi.VSiLAS^-1.(W + VSoLAS).S2Mo^-1 - VSo

    Parameters
    ----------
    W: volume
    input_hdr: dict-like
    output_hdr: dictr-like

    Returns
    -------
    WLPI: volume
    '''
    S2Mi = aims.AffineTransformation3d(input_hdr['storage_to_memory'])
    S2Mo = aims.AffineTransformation3d(output_hdr['storage_to_memory'])
    VSi = aims.AffineTransformation3d()
    np.fill_diagonal(VSi.np[:3, :3], input_hdr['voxel_size'][:3])
    VSo = aims.AffineTransformation3d()
    np.fill_diagonal(VSo.np[:3, :3], output_hdr['voxel_size'][:3])
    VSiLAS = aims.AffineTransformation3d()
    np.fill_diagonal(VSiLAS.np[:3, :3],
                     np.abs(S2Mi.inverse().transformVector(
                        aims.Point3df(input_hdr['voxel_size'][:3]))))
    VSoLAS = aims.AffineTransformation3d()
    np.fill_diagonal(VSoLAS.np[:3, :3],
                     np.abs(S2Mo.inverse().transformVector(
                        aims.Point3df(output_hdr['voxel_size'][:3]))))
    S2Mo_inv = S2Mo.inverse()
    # We cannot write "W + VSoLAS" etc. W in the math is a transform function
    # so we write WLPI as a function first
    #def _WLPI(left, VSoLAS, VSo, S2Mo_inv, W, p):
        #p2 = S2Mo_inv.transform(p)
        #res = left.transform(W[p2[0], p2[1], p2[2]] \
            #+ VSoLAS.transform(aims.Point3dd(p2))).np \
            #- VSo.transform(p.np.astype(float))
        #return res

    # re-write WLPI using np matrix operations to avoid slow loops
    def _WLPI2(left, VSoLAS, VSo, S2Mo_inv, W):
        # print('WLPI2\ngenerate indices...')
        print('0%', end='')
        o = np.indices(W.shape[:3], dtype=np.int32)
        indices = np.array([o[i].ravel() for i in range(o.shape[0])])
        ind_t = tuple(indices)
        N = indices.shape[1]
        indices = np.vstack((indices, np.ones((N, ), dtype=np.int32)))
        #print('indices:', indices.shape)
        print('\r20%', end='')
        WLPI = aims.Volume(W)
        # compose W + VSoLAS
        WLPI[ind_t] += VSoLAS.np.dot(indices)[:3,:].T
        #print('W+VSoLAS done')
        print('\r40%', end='')
        # compose left.WLPI
        WLPI[ind_t] = left.np.dot(
            np.vstack((WLPI[ind_t].T, np.ones((N, )))))[:3, :].T
        #print('left.Wtmp done')
        print('\r60%', end='')
        # compose WLPI.S2Mo_inv
        T = np.round(S2Mo_inv.np.dot(indices)[:3, :]).astype(np.int32)
        WLPI[ind_t] = WLPI[tuple(T)]
        #print('Wtmp.S2Mo_inv done')
        print('\r80%', end='')
        # compose WLPI - VSo
        WLPI[ind_t] -= VSo.np.dot(indices)[:3,:].T
        print('\r100%')

        return WLPI

    # not sure why this is needed, W contents seem flipped (upside down etc)
    # and thus not sure if we must apply input or output S2M
    Wbis = W
    iorient = Wbis.referential().orientationFromTransform(S2Mi)
    Wbis.referential().setOrientation(iorient)
    Wbis.flipToOrientation('LPI')

    left = VSi * S2Mi * VSiLAS.inverse()

    #WLPI = aims.Volume(Wbis)
    #for z in range(Wbis.shape[2]):
        #print('\r{}%  '.format(int(z*100/Wbis.shape[2])), end='')
        #for y in range(Wbis.shape[1]):
            #for x in range(Wbis.shape[0]):
                #WLPI[x, y, z, :] = _WLPI(left, VSoLAS, VSo, S2Mo_inv, Wbis,
                                         #aims.Point3d(x, y, z))
    #print('\r100%  ')

    WLPI = _WLPI2(left, VSoLAS, VSo, S2Mo_inv, Wbis)
    return WLPI


def main():
    import argparse

    parser = argparse.ArgumentParser(
        '''FSL warp fields conversion to AIMS

Deals with FSL fields output from fnirt, as relzative deformation fields
(not spline coefs).
FSL fields code the dispalcement in mm from a source point in Nifti disk
space coordinates of the Destination (output) image to the disk space
coordinates (in mm, thus) of the source (input) image.

AIMS is similar, but codes from/to its LPI oriented image coordinates.

After this conversion, the field may be used using AimsApplyTransform, or
in AIMS programs as a FfdTransformation.
''')
    parser.add_argument(
        '-i', '--input',
        help='input FSL warp field (must not be a coefficients file)')
    parser.add_argument('-o', '--output', help='output AIMS FFD field')
    parser.add_argument('-s', '--source', help='source input volume space')
    parser.add_argument(
        '-r', '--reference',
        help='output reference volume (may be the field -i itself, '
        'the default)')
    options = parser.parse_args()

    input_name = options.input
    output_name = options.output
    ref_name = options.reference
    source_name = options.source
    if ref_name is None:
        ref_name = input_name
    W = aims.read(input_name)
    finder = aims.Finder()
    finder.check(ref_name)
    output_hdr = finder.header()
    finder.check(source_name)
    input_hdr = finder.header()

    WLPI = fsl_to_aims_warp_field(W, input_hdr, output_hdr)
    aims.write(WLPI, output_name)


if __name__ == '__main__':
    main()
