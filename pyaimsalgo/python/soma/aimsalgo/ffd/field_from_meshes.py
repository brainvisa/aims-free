from soma import aims
from .field_extrapolate import field_extrapolate
import tempfile
import numpy as np
import os


def field_from_deformed_mesh(orig_mesh, deformed_mesh, output_field=None,
                             vs=[1., 1., 1.], max_seeds=32000):
    """ Build a FFD deformation field from a pair of meshes, which should be
    the same mesh before and after a deformation.

    Both meshes should have the same number of vertices, and their order should
    match. We use this vertex correspondance to build the deformation map at
    vertices locations, then extrapolate the field.
    """
    if isinstance(orig_mesh, str):
        orig_mesh = aims.read(orig_mesh)
    if isinstance(deformed_mesh, str):
        deformed_mesh = aims.read(deformed_mesh)

    bbmax = np.ceil(np.max(orig_mesh.vertex().np, axis=0)).astype(int) + 5

    field = aims.Volume(list(bbmax), dtype='POINT3DF')
    field.fill(aims.Point3df(0.))
    mask = aims.Volume(list(bbmax), dtype='S16')
    mask.fill(0)

    for v1, v2 in zip(orig_mesh.vertex(), deformed_mesh.vertex()):
        dep = v2 - v1
        loc = np.round(v1.np).astype(int)
        field[loc[0], loc[1], loc[2], 0] = dep
        mask[loc[0], loc[1], loc[2], 0] = 32767

    tmp = []
    if output_field is None:
        tmp = tempfile.mkstemp(prefix='aims_field_', suffix='.ima')
        output_field = tmp[1]
        os.close(tmp[0])

    try:
        # TODO FIXME: we need a function to transform a Volume_POINT3DF into a
        # FfdTransformation, only the reader does it for now...
        aims.write(field, output_field)

        field, mask2 = field_extrapolate(
            output_field, mask, out_field=output_field, inv=False,
            max_seeds=max_seeds, dilation=None)
    finally:
        if tmp:
            try:
                os.unlink(tmp[1])
                os.unlink(tmp[1][:-4] + '.dim')
                os.unlink(tmp[1] + '.minf')
            except Exception:
                pass

    return field


def main():

    import argparse

    max_seeds = 32000

    parser = argparse.ArgumentParser(
        'build a deformation field between 2 meshes (same mesh before and '
        'after deformation)')
    parser.add_argument('-i', '--input', help='mesh before deformation')
    parser.add_argument('-j', '--input2', help='mesh after deformation')
    parser.add_argument(
        '-o', '--output',
        help='deformation field (Volume of Point3DF, use .ima format)')

    options = parser.parse_args()
    mesh1_f = options.input
    mesh2_f = options.input2
    output_f = options.output

    field_from_deformed_mesh(mesh1_f, mesh2_f, output_field=output_f,
                             max_seeds=max_seeds)


if __name__ == '__main__':
    main()
