#! /usr/bin/env python
############################################################################
# This software and supporting documentation are distributed by
# CEA/NeuroSpin, Batiment 145, 91191 Gif-sur-Yvette cedex, France.
# This software is governed by the CeCILL license version 2 under
# French law and abiding by the rules of distribution of free software.
# You can  use, modify and/or redistribute the software under the
# terms of the CeCILL license version 2 as circulated by CEA, CNRS
# and INRIA at the following URL "http://www.cecill.info".
############################################################################

import numpy
from soma import aims
import sys

if sys.version_info[0] >= 3:
    xrange = range


def coulomb_force(p1, p2, has_link):
    '''Utility function for sphere_distribution.

    Individual Coulomb force between 2 points
    '''
    r = p1 - p2
    l = r.norm()
    if l < 1e-5:
        l = 1e-5
    l = 1. / (l * l * l)
    ff = r * l
    if has_link:
        # linked refs should be closer
        ff *= 0.1
    return ff


def coulomb_energy(p1, p2, has_link):
    '''Utility function for sphere_distribution.

    Energy functions are the integral of corresponding forces -
    they are used to drive the minimization.

    Individual Coulomb energy between 2 points.
    '''
    l = (p2 - p1).norm()
    if has_link:
        l *= 10.
    if l < 1e-5:
        l = 1e-5
    return 1. / l


def coulomb_and_restoring_force(p1, p2, has_link):
    '''Utility function for sphere_distribution.

    Individual Coulomb force between 2 points, plus a restoring force that
    avoids points expanding away like the universe
    '''
    r = p1 - p2
    l = r.norm()
    if l < 1e-5:
        l = 1e-5
    attract = 1.
    if has_link:
        # linked refs should be closer
        attract = 3.
    l2 = .1 / (l * l) - attract * (l + 0.5) + 1.5
    ff = r * l2 / l
    return ff

def coulomb_and_restoring_energy(p1, p2, has_link):
    '''Utility function for sphere_distribution.

    Energy functions are the integral of corresponding forces -
    they are used to drive the minimization.

    Individual energy between 2 points for the coulomb_and_restoring_force.
    '''
    l = (p2 - p1).norm()
    attract = 1.
    if has_link:
        attract = 3.
    if l < 1e-5:
        l = 1e-5
    e = 1. / l + attract * l * l / 2. + attract * 0.5
    return e


def unconstrained_move(pt, f, step):
    '''Utility function for sphere_distribution.

    pos_constraints function for unconstrained move (not restricted to a
    sphere).

    It merely adds force f * step to the returned point position pt
    '''
    return pt + f * step



def sphere_distribution(points, links={}, indiv_force=coulomb_force,
                        pos_constraints=None, indiv_energy=coulomb_energy):
    '''get a points distribution on a sphere.

    adapted from: http://www.nicoptere.net/blog/index.php/2008/09/20/50-distribution-points-sphere-actionscript
    http://www.nicoptere.net/AS3/distribution/Distribute.as

    A few parameters have been added, to allow fitting spheres, or "free"
    geometries and forces.

    parameters
    ----------
    points: list of 3D points (Point3df), or number
        If points is a number, this number of points in initialized randomly
        on a sphere.
    links: dict
        Optional links. Linked points have different forces to make them
        closer. The links dict maps a point num to a set of linkes points:
        {0: [1, 3], 1: [0, 5], 2: [4], 3: [0]}
    indiv_force: function
        force function between 2 points. Takes 2 points and a bool (has_link),
        and returns the force vector applied to the 1st point.
        Defaults to a Coulomb force (r_vec/r^3)
    pos_constraints: function
        applies force to a point, and add additional constraints.
        Takes a point, a force vector, and a step (factor to the force),
        returns the new point position.
        Default: projects the force to be tangent to a unit sphere, and
        renormalizes the resulting point position to stock on the sphere.
    indiv_energy: function
        energy function between 2 points. Like the force, takes 2 points and
        a bool (has_link) as args, returns the force energy.
        (actually the opposite of the integral of indiv_force)
        Defaults to Coulomb energy (1/r)
    '''
    def init_points(npoints):
        pts = []
        for i in xrange(npoints):
            lon = random.random() * 2. * math.pi
            lat = random.random() * math.pi - math.pi / 2.
            r = math.cos(lat)
            pos = [r * math.cos(lon),
                    r * math.sin(lon),
                    math.sin(lat)]
            pts.append(aims.Point3df(pos))
        return pts

    def has_link(i, j, links):
        l = links.get(i)
        if not l:
            return False
        if j in l:
            return True
        return False

    def get_forces(pts, links={}, indiv_force=coulomb_force):
        np = len(pts)
        f = [aims.Point3df(0., 0., 0.) for i in xrange(np)]
        for i in xrange(np):
            for j in xrange(i+1, np):
                ff = indiv_force(pts[i], pts[j], has_link(i, j, links))
                f[i] += ff
                f[j] -= ff
        return f

    def get_coulomb_energy(pts, links={}, indiv_energy=coulomb_energy):
        np = len(pts)
        e = 0.
        for i in xrange(np):
            for j in xrange(i+1, np):
                e += indiv_energy(pts[i], pts[j], has_link(i, j, links))
        return e

    def distribute(pts, nsteps=100, step=0.01, links={},
                   indiv_force=coulomb_force, pos_constraints=None,
                   indiv_energy=coulomb_energy):
        minimal_step = 1e-10
        np = len(pts)
        pp0 = [aims.Point3df(p) for p in pts]
        pp1 = [aims.Point3df(p) for p in pts]
        e0 = get_coulomb_energy(pp0, links=links, indiv_energy=indiv_energy)
        for k in xrange(nsteps):
            f = get_forces(pp0, links=links, indiv_force=indiv_force)
            for i in xrange(np):
                d = f[i].dot(pp0[i])
                if pos_constraints:
                    pp1[i] = pos_constraints(pp0[i], f[i], step)
                else: # assume sphere of radius 1.
                    f[i] -= pp0[i] * d
                    pp1[i] = pp0[i] + f[i] * step
                    pp1[i].normalize()
            e = get_coulomb_energy(pp1, links=links,
                                   indiv_energy=indiv_energy)
            if e >= e0:
                step /= 2.
                if step < minimal_step:
                    break
                continue
            else:
                pp0 = [aims.Point3df(p) for p in pp1]
                step *= 2.
                e0 = e
        return pp0

    if type(points) is int:
        npoints = points
        pts = init_points(npoints)
    else:
        pts = points # initialized
    pts = distribute(pts, links=links, indiv_force=indiv_force,
                     pos_constraints=pos_constraints,
                     indiv_energy=indiv_energy)
    return pts


