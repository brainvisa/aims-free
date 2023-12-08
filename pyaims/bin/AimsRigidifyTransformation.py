#! /usr/bin/env python3

import argparse
import logging
import sys

import numpy


logger = logging.getLogger(__name__)


def read_trm_matrix(trm_filename, dtype=float):
    """Read a .trm file into a 4×4 matrix in homogeneous coordinates."""
    raw_matrix = numpy.loadtxt(trm_filename, dtype=dtype)
    if raw_matrix.shape != (4, 3):
        raise ValueError('trm file should contain a 4×3 matrix')
    ret = numpy.empty((4, 4), dtype=dtype)
    ret[:3, :3] = raw_matrix[1:, :]
    ret[:3, 3] = raw_matrix[0, :]
    ret[3, :] = [0, 0, 0, 1]
    return ret


def write_trm_matrix(matrix, trm_filename):
    """Write a 4×4 matrix in homogeneous coordinates into a .trm file."""
    # An explicit iteration is used instead of nympy.savetxt for readability of
    # the output trm file, because in Python 2.7 and 3.1+, repr(float(x))
    # produces shorter output strings (the shortest decimal string that is
    # guaranteed to round back to x).
    #
    # NumPy implements this feature starting with version 1.14.0
    with open(trm_filename, 'wt') as f:
        f.write(' '.join(repr(float(x)) for x in matrix[:-1, -1]))
        f.write('\n')
        for line in matrix[:-1, :-1]:
            f.write(' '.join(repr(float(x)) for x in line))
            f.write('\n')


def descale_matrix(trm, centre=(0, 0, 0), centre_space='target'):
    trm = numpy.asarray(trm, dtype=float)
    dim = trm.shape[1] - 1
    assert trm.shape == (dim + 1, dim + 1)

    centre = numpy.asfarray(centre)
    assert centre.shape == (dim,)

    # Compute a decomposition of the transformation in pre-rotation (V),
    # scaling along axes (S), and post-rotation (U)
    U, S, V = numpy.linalg.svd(trm[:-1, :-1])
    logger.debug('SVD decomposition:\nU:\n%s\nS:\n%s\nV:\n%s', U, S, V)

    # Compute coordinates of the centre in the input space
    if centre_space == 'source':
        centre_in_source_space = centre
        centre_in_target_space = (trm @ numpy.r_[centre, [1]])[:-1]
    elif centre_space == 'target':
        centre_in_target_space = centre
        # Same algorithm as numpy.linalg.matrix_rank
        rank = numpy.count_nonzero(
            S > S.max() * max(trm.shape) * numpy.finfo(S.dtype).eps
        )
        if rank < dim:
            raise RuntimeError('The transformation is not invertible')
        inv_linear = V.T @ numpy.diag(1 / S) @ U.T
        inv_trm = numpy.r_[
            numpy.c_[
                inv_linear,
                -(inv_linear @ trm[:-1, -1])[:, numpy.newaxis],
            ],
            numpy.array([0] * dim + [1])[numpy.newaxis, :],
        ]
        logger.debug('Inverse transformation:\n%s', inv_trm)
        centre_in_source_space = (inv_trm @ numpy.r_[centre, [1]])[:-1]
    else:
        raise ValueError('invalid value for centre_space')

    logger.debug('centre in source space: %s', centre_in_source_space)
    logger.debug('centre in target space: %s', centre_in_target_space)

    # Compute the new matrix as a pure rotation and translation
    new_linear_part = U @ V
    new_translation = (
        centre_in_target_space - (new_linear_part @ centre_in_source_space)
    )

    descaled_trm = numpy.zeros_like(trm)
    descaled_trm[:-1, :-1] = new_linear_part
    descaled_trm[:-1, -1] = new_translation
    descaled_trm[-1, -1] = 1

    logger.debug('centre transformed by the descaled transformation: %s',
                 (descaled_trm @ numpy.r_[centre_in_source_space, [1]])[:-1])

    return descaled_trm


def descale_trm(input_trm_filename, output_trm_filename,
                centre=(0, 0, 0), centre_space='target'):
    matrix = read_trm_matrix(input_trm_filename)
    descaled_matrix = descale_matrix(matrix,
                                     centre=centre,
                                     centre_space=centre_space)
    write_trm_matrix(descaled_matrix, output_trm_filename)


def parse_command_line(argv=sys.argv):
    """Parse the script's command line."""
    parser = argparse.ArgumentParser(
        description="""\
Compute the closest rotation matrix to a given affine transformation.""")
    parser.add_argument('input_trm',
                        help='input affine transformation (.trm file)')
    parser.add_argument('output_trm',
                        help='output affine transformation (.trm file)')
    parser.add_argument('--debug', action='store_true',
                        help='print debugging information')
    parser.add_argument('--centre', default='0, 0, 0',
                        help='centre of the descaling. This is the point, '
                        'given in either source or target space coordinates '
                        '(see --centre-in), that will be transformed in the '
                        'same way by the input transformation and the output '
                        'transformation. A comma-separated list of 3 floats '
                        'is expected. [default: 0, 0, 0]')
    parser.add_argument('--centre-space', choices=['source', 'target'],
                        default='target',
                        help='the space (either "source" or "target") '
                        'in which the centre is given (see --centre). '
                        '[default: "target"]')
    args = parser.parse_args(argv[1:])

    centre = tuple(float(x) for x in args.centre.split(','))
    args.centre = centre
    return args


def main(argv=sys.argv):
    """The script's entry point."""
    args = parse_command_line(argv)
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    return descale_trm(args.input_trm,
                       args.output_trm,
                       centre=args.centre,
                       centre_space=args.centre_space) or 0


if __name__ == '__main__':
    sys.exit(main())
