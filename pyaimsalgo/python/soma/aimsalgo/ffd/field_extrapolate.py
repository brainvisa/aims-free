from soma import aims, aimsalgo
import numpy as np


def field_extrapolate(field, mask, out_field=None, inv=False,
                      max_seeds=32000, mask_tr=None, mask_orig=None,
                      threshold=0, out_mask=None, grad_th=5., dilation=3.,
                      smooth=10):
    """ Extrapolate FFD deformation field, based on a mask.

    The binary mask tells where the field is really valid. The mask is
    optionally dilated to start with a larger zone, if the field is already
    smooth (otherwise don't use this option).


    This corrections aims to extrapolate deformation fields outside of their
    "good domain" using voronoi propagation.

    To do so:
    * For a given deformation field, and the mask it is from:
    - if the field is an inverse one, transform the mask to dest space
    - binarize the mask to use it as a mask
    - erase values in the mask where the field has nan values
    - dilate the mask
    - re-erase values in the (dilated) mask where the field has nan values
    - After dilation, a gradient is processed, and where this gradient is too
      strong (abs > grad_th), voxels are removed from the dilated mask, because
      they correspond to discontinuities of the field.
    - keep only the biggest conneted component. This is the final mask used in
      later steps.

    This way we have a mask of where field values are valid and should be kept
    untouched.

    - get the border (external voxels) of the mask
    - assign seed value to each voxel of the border. Actually our voronoi algo
      is compiled only for 16 bit seeds volumes, thus we use only 32000
      different seeds, and assign them to values ranges along the border. It's
      enough.
    - for each component x, y, z:
        - grow a Voronoi diagram outside the mask from the seeds. This will
          propagate regions more or less radially.
        - assign (propagate) values of the field at the border to each voronoi
          region

    This is more or less ok but the resulting field is not very smooth.

    - apply gaussian smoothing to the result, in an iterative way (`smooth`
      times 1mm, thus amounting to a smoothing of `smooth`):

        - smooth 1mm
        - set back the original (unsmoothed) field in the mask in order to
          preserve initial values there


    Parameters
    ----------
    field: str or aims.FfdTransformation
        input displacement field, or filename (str)
    mask: aims.Volume or str
        input mask
    out_field: str or None
        if specified, write the output extrapolated field
    inv: bool
        if True, then the field is an inverse transformation field, and the
        mask has to be inverted to go to target space.
    max_seeds: int
        max number of Voronoi seeds. Needs to be < 32765.
    mask_tr: aims.AffineTransformation3d or str
        if specified, this is a trandformation (or transformation filename) to
        be applied to the mask.
    mask_orig: aims.Volume_S16 or str
        if mask_tr is not None, then this image is the transformed mask
        template (dimensions, voxel size). Not used if mask_tr is None.
    threshold: int
        threshold applied to the mask to binarize it.
    out_mask: str
        if not None, the dilated mask will be written in this file.
    grad_th: float
        Gradients threshold: if dilation happens, then voxels which gradients
        exceed this value on any coordinate will be considered as a
        discontinuity and will be removed frm the mask.
    dilation: float
        if > 0, the mask is dilated by this amount (in mm). If the field is
        defined only on the initial mask, make sure to set it to 0 (or None) to
        avoid dilation. The field has to be valid on the dilated mask.
    smooth: int
        number of smoothing iterations. Each iteration is 1mm.

    Returns
    -------
    field: aims.Volume_POINT3DF
        the extrapolated field
    mask: aims.Volume_S16
        the dilated mask actually used for the Voronoi
    """

    if isinstance(field, str):
        field_tr = aims.read(field, dtype='Transformation3d')
        field = field_tr.ctrlDeformations()
    else:
        field_tr = field
        field = aims.Volume(field_tr.ctrlDeformations())
    if isinstance(mask, str):
        mask = aims.read(mask).astype('S16')
    else:
        mask = aims.Volume(mask).astype('S16')
    mask[mask.np <= threshold] = 0
    mask[mask.np != 0] = 32767
    if mask_tr is not None:
        if isinstance(mask_tr, str):
            mask_tr = aims.read(mask_tr)
        if isinstance(mask_orig, str):
            mask_orig = aims.read(mask_orig)
        otpl = aims.Volume(mask_orig.getSize(), dtype='S16')
        otpl.setVoxelSize(mask_orig.getVoxelSize())
        rs = aims.ResamplerFactory_S16().getResampler(0)
        rs.resample_inv(mask, mask_tr, 0, otpl)
        mask = otpl
    if inv:
        # inverse transform: transform the mask to target space
        out_tpl = aims.Volume(field.shape, dtype='S16')
        out_tpl.setVoxelSize(field.getVoxelSize())
        rs = aims.ResamplerFactory_S16().getResampler(0)
        rs.resample_inv(mask, field_tr, 0, out_tpl)
        mask = out_tpl
    # print('mask:', mask.shape, ', field:', field.shape)
    # erase parts with NaN
    mask[np.where(np.isnan(field['v']))[:4]] = 0

    # dilate mask mask
    if dilation is not None and dilation > 0.:
        m = aimsalgo.MorphoGreyLevel_S16()
        m.setChamferBinaryMorphoEnabled(True)
        tpldil = m.doDilation(mask, dilation)
        print('DILATION DONE')
    else:
        tpldil = aims.Volume(mask)
    # erase parts with NaN
    tpldil[np.where(np.isnan(field['v']))[:4]] = 0

    comp = aims.Volume(field.shape[:3], [5, 5, 5], dtype='FLOAT')
    comp.copyHeaderFrom(field.header())

    if dilation is not None and dilation > 0.:
        # gradient
        print('computig gradients')
        for dim in range(3):
            comp[:, :, :, 0] = field['v'][:, :, :, 0, dim]
            tpldil[np.isnan(comp.np)] = 0
            comp[tpldil.np == 0] = 0

            gr = aimsalgo.AimsGradient_FLOAT()
            grx = gr.X(comp)
            tpldil[grx.np < -grad_th] = 0
            tpldil[grx.np > grad_th] = 0
            grx = gr.Y(comp)
            tpldil[grx.np < -grad_th] = 0
            tpldil[grx.np > grad_th] = 0
            grx = gr.Z(comp)
            tpldil[grx.np < -grad_th] = 0
            tpldil[grx.np > grad_th] = 0

    aims.AimsConnectedComponent(tpldil, aims.Connectivity.CONNECTIVITY_26_XYZ,
                                0, True, 0, 0, 1)
    print('CONNECT. COMP. DONE')
    if out_mask is not None:
        aims.write(tpldil, out_mask)

    # get border
    shape = [x - 2 for x in tpldil.shape[:3]]
    vols = []
    i = 1
    j = 1
    for k in range(3):
        vols.append(
            tpldil[i:shape[0] + i, j:shape[1] + j, k:shape[2] + k, :])
    k = 1
    for j in range(0, 3, 2):
        vols.append(
            tpldil[i:shape[0] + i, j:shape[1] + j, k:shape[2] + k, :])
    j = 1
    for i in range(0, 3, 2):
        vols.append(
            tpldil[i:shape[0] + i, j:shape[1] + j, k:shape[2] + k, :])

    bord = aims.Volume(tpldil.shape, [5, 5, 5], dtype='S16')
    rbord = bord.refVolume()
    rbord.fill(0)
    bord[:] = tpldil[:]
    bord[1:-1, 1:-1, 1:-1][np.min(vols, axis=0) != 0] = 32767
    print('border done.')

    seedp = np.where(np.logical_and(bord.np > 0, bord.np < 32767))

    for dim in range(3):

        print('propagating axis', dim)
        comp[:, :, :, 0] = field['v'][:, :, :, 0, dim]
        comp[tpldil.np == 0] = 0
        rcomp = comp.refVolume()

        values = comp[seedp]
        vmin = np.min(values)
        vmax = np.max(values)
        interval = vmax - vmin
        indices = ((values - vmin) * (max_seeds - 0.01)
                   / interval).astype(int) + 1

        bord[seedp] = indices

        fm = aims.FastMarching()
        fm.doit(rbord, {0}, np.unique(indices))
        voro = fm.voronoiVol()
        rcomp[rbord.np != 32767] = (voro[rbord.np != 32767] - 1) \
            * interval / (max_seeds - 0.01) + vmin

        nrcomp = rcomp
        scomp = comp
        for n in range(smooth):
            gs = aimsalgo.Gaussian3DSmoothing_FLOAT(1, 1, 1)
            rscomp = gs.doit(nrcomp)
            scomp = aims.VolumeView(
                rscomp, [5, 5, 5], [x - 10 for x in rscomp.shape[:3]])
            nrcomp = rscomp
            scomp[tpldil.np != 0] = comp[tpldil.np != 0]

        field['v'][:, :, :, 0, dim] = scomp[:, :, :, 0]
        # aims.write(scomp, f'/tmp/comp_{["x", "y", "z"][dim]}.nii.gz')

    if out_field is not None:
        aims.write(field, out_field)
    return field, tpldil


def cube_id(shape, vs):
    shape = shape[:3]
    cube_v = aims.Volume(shape, dtype='POINT3DF')
    cube_v.fill((0, 0, 0))
    cube = cube_v['v']
    cube[:, :, :, 0, 0] = np.fromfunction(lambda i, j, k: i, shape)
    cube[:, :, :, 0, 1] = np.fromfunction(lambda i, j, k: j, shape)
    cube[:, :, :, 0, 2] = np.fromfunction(lambda i, j, k: k, shape)
    cube[:, :, :, 0, 0] *= vs[0]
    cube[:, :, :, 0, 1] *= vs[1]
    cube[:, :, :, 0, 2] *= vs[2]
    cube_v.setVoxelSize(vs)
    return cube_v


def correct_inverse_fields_step(field, mask, field_i, mask_i,
                                out_field_c_f=None, out_field_i_c_f=None):
    """ Single step for correct_inverse_fields()
    """
    field_v = field.ctrlDeformations()
    cube = cube_id(field_v.shape, field_v.getVoxelSize())
    # print('cube:', cube.shape)
    field_i_v = field_i.ctrlDeformations()
    cube_i = cube_id(field_i_v.shape, field_i_v.getVoxelSize())
    # print('cube_i:', cube_i.shape)
    cube_i_o = aims.Volume(cube_i)
    cube_o = aims.Volume(cube)
    cdiff = aims.Volume(cube)
    cdiff.fill((0, 0, 0))
    cdiff_i = aims.Volume(cube_i)
    cdiff_i.fill((0, 0, 0))
    rs = aims.ResamplerFactory_FLOAT().getResampler(1)
    for dim in range(3):
        # dl = ['x', 'y', 'z'][dim]
        cube_i_o_x = aims.Volume(cube_i.shape[:3], dtype='FLOAT')
        cube_i_o_x.setVoxelSize(cube_i.getVoxelSize()[:3])
        cube_i_o_x.fill(0)
        cube_x = aims.Volume(cube.shape[:3], dtype='FLOAT')
        cube_x.setVoxelSize(cube.getVoxelSize()[:3])
        cube_x[:, :, :, :] = cube['v'][:, :, :, :, dim]
        cube_o_x = aims.Volume(cube.shape[:3], dtype='FLOAT')
        cube_o_x.setVoxelSize(cube.getVoxelSize()[:3])
        cube_o_x.fill(0)
        rs.resample_inv(cube_x, field_i, np.nan, cube_i_o_x)
        # aims.write(cube_i_o_x, f'/tmp/cube_i_{dl}.nii.gz')
        rs.resample_inv(cube_i_o_x, field, np.nan, cube_o_x)
        # aims.write(cube_i_o_x, f'/tmp/cube{dl}.nii.gz')
        cube_o['v'][:, :, :, 0, dim] = cube_o_x[:, :, :, 0]
        cdiff['v'][:, :, :, :, dim] = cube_o_x - cube['v'][:, :, :, :, dim]
        cdiff['v'][:, :, :, :, dim][np.isnan(cube_o_x.np)] = 0

        cube_i_x = aims.Volume(cube_i.shape[:3], dtype='FLOAT')
        cube_i_x.setVoxelSize(cube_i.getVoxelSize()[:3])
        cube_i_x[:, :, :, :] = cube_i['v'][:, :, :, :, dim]
        cube_o_x.fill(0)
        cube_i_o_x.fill(0)
        rs.resample_inv(cube_i_x, field, np.nan, cube_o_x)
        # aims.write(cube_o_x, f'/tmp/cube2_{dl}.nii.gz')
        rs.resample_inv(cube_o_x, field_i, np.nan, cube_i_o_x)
        # aims.write(cube_i_o_x, f'/tmp/cube2_i_{dl}.nii.gz')
        cube_i_o['v'][:, :, :, 0, dim] = cube_i_o_x[:, :, :, 0]
        cdiff_i['v'][:, :, :, :, dim] \
            = cube_i_o_x - cube_i['v'][:, :, :, :, dim]
        cdiff_i['v'][:, :, :, :, dim][np.isnan(cube_i_o_x.np)] = 0

    if mask is not None:
        cdiff[mask.np != 0] = ((0, 0, 0), )
    # aims.write(cdiff, '/tmp/cdiff.ima')
    if mask_i is not None:
        cdiff_i[mask_i.np != 0] = ((0, 0, 0), )

    if np.all(cdiff['v'][1:-1, 1:-1, 1:-1, 0] == 0):
        print('fields are perfectrly invertible.')
        return False, field, field_i

    x = field_v.get()
    x -= cdiff / 2
    del x
    if out_field_c_f is not None:
        aims.write(field_v, out_field_c_f)

    cdiff_i = cube_i_o - cube_i
    x = field_i_v.get()
    x -= cdiff_i / 2
    del x
    if out_field_i_c_f is not None:
        aims.write(field_i_v, out_field_i_c_f)

    return True, field, field_i


def correct_inverse_fields(field, mask, field_i, mask_i, nsteps=2,
                           out_field_c_f=None, out_field_i_c_f=None):
    """ Iterative correction for extraolated direct and inverse fields: the aim
    is to force them to be inverse one from the other: we substract from the
    direct field half of the difference between a transformed and
    inverse_transformed ID matrix image, and do the same for the inverse field.
    We exclude the validity mask from this correction in order to keep the
    initial correct and validated field unchanged.

    In practical life, iterating times seems enough, leading to differences
    under 0.2 mm.
    """
    gchanged = False
    for step in range(nsteps):
        changed, field, field_i = correct_inverse_fields_step(
            field, mask, field_i, mask_i, out_field_c_f=None,
            out_field_i_c_f=None)
        gchanged |= changed
        if not changed:
            break
    if out_field_c_f is not None:
        aims.write(field.ctrlDeformations(), out_field_c_f)
    if out_field_i_c_f is not None:
        aims.write(field_i.ctrlDeformations(), out_field_i_c_f)

    return gchanged, field, field_i
