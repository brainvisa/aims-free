#! /usr/bin/env python
# -*- coding: utf-8; -*-

from __future__ import division, print_function, unicode_literals

import argparse
import logging
import math
import sys

import numpy


logger = logging.getLogger(__name__)


def read_trm_matrix(trm_filename, dtype=numpy.float_):
    raw_matrix = numpy.loadtxt(trm_filename, dtype=dtype)
    assert raw_matrix.shape == (4, 3)
    ret = numpy.empty((4, 4), dtype=dtype)
    ret[:3, :3] = raw_matrix[1:, :]
    ret[:3, 3] = raw_matrix[0, :]
    ret[3, :] = [0, 0, 0, 1]
    return ret


def matrix_is_orthogonal(mat):
    return (numpy.allclose(numpy.matmul(mat.T, mat),
                           numpy.eye(mat.shape[0], mat.shape[0]))
            and numpy.allclose(numpy.matmul(mat, mat.T),
                               numpy.eye(mat.shape[1], mat.shape[1])))


def matrix_is_diagonal(mat, atol=1e-8):
    mat = numpy.copy(mat)
    numpy.fill_diagonal(mat, 0)
    return numpy.allclose(mat, 0, rtol=0, atol=atol)


def normalize_vect(vec):
    return vec / numpy.linalg.norm(vec)


def classify_matrix(trm, verbose=False, rtol=1e-5, translation_tol=1e-3):
    trm = numpy.asarray(trm, dtype=float)
    dim = trm.shape[1] - 1
    assert trm.shape == (dim + 1, dim + 1)
    logger.debug('Classifying transformation:\n%r', trm)
    logger.debug('rtol=%g, translation_tol=%g', rtol, translation_tol)

    # Compute a decomposition of the transformation in pre-rotation (V),
    # scaling along axes (S), and post-rotation (U)
    U, S, V = numpy.linalg.svd(trm[:-1, :-1])

    # Same algorithm as numpy.linalg.matrix_rank
    # rank = numpy.count_nonzero(
    #     S > S.max() * max(trm.shape) * numpy.finfo(S.dtype).eps)
    rank = numpy.count_nonzero(S > S.max() * rtol)
    if verbose:
        print('Rank: {0:d}'.format(rank))

    if rank < dim:
        trm_trm = numpy.matmul(trm, trm)
        if (numpy.allclose(trm_trm[:-1, :-1], trm[:-1, :-1],
                           rtol=rtol, atol=0)
                and numpy.allclose(trm_trm[:-1, -1], trm[:-1, -1],
                                   rtol=0, atol=translation_tol)):
            print('Projection')
            return 'projection'
        print('Non-invertible affine transformation')
        return 'affine'

    determinant = numpy.linalg.det(trm[:-1, :-1])
    has_direct_orientation = determinant >= 0

    has_scaling = not numpy.allclose(S[0], 1, rtol=rtol, atol=0)
    isotropic_scaling = numpy.allclose(S[0], S[1:], rtol=rtol, atol=0)

    has_rotation = not numpy.allclose(numpy.matmul(U, V), numpy.eye(3),
                                      rtol=0, atol=rtol)
    has_no_pre_rotation = matrix_is_diagonal(
        numpy.matmul(numpy.matmul(U, numpy.diag(S)), U.T),
        atol=rtol * S[0])
    has_no_post_rotation = matrix_is_diagonal(
        numpy.matmul(numpy.matmul(V.T, numpy.diag(S)), V),
        atol=rtol * S[0])

    if logger.isEnabledFor(logging.DEBUG):
        logger.debug('has_rotation: %s', has_rotation)
        logger.debug('U:\n%s', U)
        logger.debug('S:\n%s', S)
        logger.debug('V:\n%s', V)
        logger.debug('U @ diag(S) @ Ut:\n%s',
                     numpy.matmul(numpy.matmul(U, numpy.diag(S)), U.T))
        if has_rotation:
            logger.debug('has_no_pre_rotation: %s', has_no_pre_rotation)
        logger.debug('Vt @ diag(S) @ V:\n%s',
                     numpy.matmul(numpy.matmul(V.T, numpy.diag(S)), V))
        if has_rotation:
            logger.debug('has_no_post_rotation: %s', has_no_post_rotation)

    if verbose:
        print('Determinant: {0:f}'.format(determinant))
        print('Orientation: {0}'
              .format('direct' if has_direct_orientation else 'indirect'))
        print('Volume dilation factor: {0:g}'
              .format(numpy.prod(S) ** (1 / dim)))
        print('Scaling: {0}'.format('{0} (isotropic)'.format(S.mean())
                                    if isotropic_scaling
                                    else '{0} (anisotropic)'.format(S)))
        invariant = numpy.matmul(
            numpy.linalg.inv(numpy.eye(dim) - trm[:-1, :-1]),
            trm[:-1, -1],
        )
        print('Invariant point: {0}'.format(invariant))
        if has_no_pre_rotation or has_no_post_rotation:
            angle_of_rotation_deg = math.degrees(math.acos(numpy.dot(
                normalize_vect(numpy.matmul(trm[:-1, :-1],
                                            [1] + [0] * (dim - 1))),
                [1] + [0] * (dim - 1),
            )))
            print('Angle of rotation: {0:.3f}°'.format(angle_of_rotation_deg))

    if isotropic_scaling:
        if not has_rotation:
            if not has_scaling:
                if numpy.allclose(trm[:-1, -1], 0,
                                  rtol=0, atol=translation_tol):
                    # Transforms that have a reflection have has_rotation=True
                    # so this case is never reached.
                    if not has_direct_orientation:
                        raise RuntimeError()
                        # print('Axis-aligned reflection')
                        # return 'reflection'
                    print('Identity')
                    return 'identity'
                print('Translation')
                return 'translation'
            if has_direct_orientation:
                print('Pure isotropic scaling')
                return 'scaling'
            # Transforms that have a reflection have has_rotation=True
            # so this case is never reached.
            raise RuntimeError()
            # print('Isotropic scaling + reflection')
            # return 'scaling+reflection'
        if numpy.isclose(S[0], 1, rtol=rtol, atol=0):
            if has_direct_orientation:
                print('Rigid')
                return 'rigid'
            print('Rotation-reflection')
            return 'rigid+reflection'
        if has_direct_orientation:
            print('Similarity')
            return 'similarity'
        print('Similarity-reflection')
        return 'similarity+reflection'
    else:
        if not has_rotation:
            if has_direct_orientation:
                print('Anisotropic scaling')
                return 'stretching'
            print('Anisotropic scaling and reflection')
            return 'stretching+reflection'
        elif has_no_post_rotation:
            if has_direct_orientation:
                print('Rigid followed by anisotropic scaling')
                return 'rigid+stretching'
            print('Rotation-reflection followed by anisotropic scaling')
            return 'rotation+reflection+stretching'
        elif has_no_pre_rotation:
            if has_direct_orientation:
                print('Anisotropic scaling followed by rigid')
                return 'stretching+rigid'
            print('Anisotropic scaling followed by rotation-reflection')
            return 'stretching+rotation+reflection'
        else:
            print('Invertible affine transformation')
            return 'invertible_affine'


def classify_trm(trm_filename, verbose=False):
    trm = read_trm_matrix(trm_filename)
    classify_matrix(trm, verbose=verbose)


def parse_command_line(argv=sys.argv):
    """Parse the script's command line."""
    parser = argparse.ArgumentParser(
        description="""\
Find out the transformation type contained in a .trm file.""")
    parser.add_argument("trm_file",
                        help="trm file to analyze")
    parser.add_argument("--verbose", action='store_true',
                        help="print more information about the decomposition")
    parser.add_argument("--debug", action='store_true',
                        help="print out the matrix decomposition")
    args = parser.parse_args(argv[1:])
    return args


def main(argv=sys.argv):
    """The script's entry point."""
    args = parse_command_line(argv)
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    return classify_trm(args.trm_file,
                        verbose=args.verbose) or 0

if __name__ == "__main__":
    sys.exit(main())
