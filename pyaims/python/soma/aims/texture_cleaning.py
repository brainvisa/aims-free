#!/usr/bin/env python

from soma import aims, aimsalgo
import numpy as np
import json
import sys
import queue
from soma import mpfork


# constants (or default values)*

default_ero_dist = {'unknown': 7.,
                    'GapMap': 1.5,
                    'other': 0.5}
default_dil = 1.5
default_min_cc_size = 100.  # mm2


def read_labels_hie(nomenclature):
    ''' Read a nomenclature as labels dict. Used by :func:`read_labels`, does
    not need to be called directly.
    '''
    # print('READ .hie labels')
    hie = aims.read(nomenclature)
    labels = {}
    todo = [hie]
    while todo:
        tree = todo.pop(0)
        todo += tree.children()
        if 'label' in tree and 'name' in tree:
            num = int(tree['label'])
            if 'color' in tree:
                color = [float(c)/255 for c in tree['color']]
                color += [1.] * (4 - len(color))
            else:
                color = [.5, .5, .5, 1.]
            labels[num] = {'Label': tree['name'], 'RGB': color}
    if 0 not in labels:
        labels[0] = {'Label': 'unknown', 'RGB': [0., 0., 0., 1.]}

    # print('labels:', len(labels))
    # delete thingsin correct order to avoid crash
    del tree, todo
    del hie

    return labels


def read_labels(nomenclature):
    ''' Read a nomenclature file, which may be in several formats:

    - JSON (JulichBrain nomenclature):

        {"properties": {"regions": [<region_def>, ...]}}

        region_def:
        {
            "labelIndex": "12",
            "name": "rototo",
            "color": [255, 255, 0],
            "children": [<region_def>, ...]
        }

    - CSV (JulichBrain nomenclature):

        12, rototo

        (all colors will be black)

    - HIE (AIMS Hierarchical nomenclature)

    Returns
    -------
    labels: dict
        {12: {"Label": "rototo", "RGB": [1., 1., 0., 1.]}, ...}
    '''

    if nomenclature.endswith('.json'):
        # json format (v 2.9+)
        labels = {}
        # process void also
        labels[0] = {'Label': 'unknown', 'RGB': [0., 0., 0., 1.]}
        with open(nomenclature) as f:
            labels_map = json.load(f)
        todo = list(labels_map['properties']['regions'])
        while todo:
            item = todo.pop(0)
            index = item.get('labelIndex')
            name = item['name']
            if index is not None:
                index = int(index)
                if index in labels and labels[index]['Label'] != name:
                    print('multiple index', index, ':', labels[index]['Label'],
                          'and:', name)
                    continue
                labels[index] = {'Label': name}
                color = item.get('color')
                if color is not None:
                    color = [float(c.strip()) / 255
                             for c in color.split(',')] + [1.]
                    labels[index]['RGB'] = color
            todo += item.get('children', [])
    elif nomenclature.endswith('.hie'):
        return read_labels_hie(nomenclature)
    else:
        with open(nomenclature) as f:
            labels = f.read().strip().split('\n')
        labels = [l.strip().split() for l in labels]
        labels = {int(l[0]): {'Label': l[1]} for l in labels}
        # process void also
        labels[0] = {'Label': 'unknown', 'RGB': [0., 0., 0., 1.]}

    return labels


def _clean_one_tex(tex, mesh, otex, lvalue, label, ero_dist, dilation):
    print('region:', label, lvalue)
    if label.endswith('(GapMap)'):
        ero_label = 'GapMap'
    else:
        ero_label = 'other'
    if label in ero_dist:
        ero_label = label
    ero = ero_dist[ero_label]
    ntex = aims.TimeTexture_S16()
    ntex[0].resize(len(tex[0]))
    ntex[0].np[tex[0].np != lvalue] = 0
    ntex[0].np[tex[0].np == lvalue] = 12
    # print('dirty:', len(np.where(ntex[0].np != 0)[0]), ero)
    # 1. erode to eliminate small crap
    dtex = aims.meshdistance.MeshErosion(mesh, ntex, 0, 1, ero, True)
    # 2. dilate to grow back, and over to connect disconnected parts
    dtex = aims.meshdistance.MeshDilation(mesh, dtex, 0, 1, ero + dilation,
                                          True)
    # 3. re-erode back to original size
    dtex = aims.meshdistance.MeshErosion(mesh, dtex, 0, 1, dilation, True)
    # print('clean1:', len(np.where(dtex[0].np != 0)[0]))
    #aims.write(dtex, '/tmp/dil_%s_%d.gii' % (label, lvalue))
    #raise RuntimeError('DEBUG STOP')

    #with lock:
    #otex[0].np[otex[0].np == lvalue] = -1
    #otex[0].np[dtex[0].np != 0] = lvalue
    #return otex
    otex = np.zeros(tex[0].np.shape, dtype=np.int16) - 32000
    otex[tex[0].np == lvalue] = -1
    otex[dtex[0].np != 0] = lvalue
    #print(label, 'done.')
    return otex


def _filter_cc(mesh, otex, lvalue, min_cc_size):
    ftex = aimsalgo.AimsMeshFilterConnectedComponent(
        mesh, otex, lvalue, -1, 0, 0, min_cc_size)
    # check that all components have not been erased
    if not np.any(ftex[0].np == lvalue):
        # try with smaller limit
        ftex = aimsalgo.AimsMeshFilterConnectedComponent(
            mesh, otex, lvalue, -1, 0, 0, min_cc_size / 5)
        if not np.any(ftex[0].np == lvalue):
            # last trial: keep only the biggest component
            ftex = aimsalgo.AimsMeshFilterConnectedComponent(
                mesh, otex, lvalue, -1, 1, 0, 0)
    # has anything changed ?
    ootex = None
    changed = False
    if len(np.where(ftex[0].np == lvalue)[0]) \
            != len(np.where(otex[0].np == lvalue)[0]):
        # replace dtex with filtered texture
        ootex = ftex[0].np
        changed = True

    return changed, ootex, lvalue


def clean_texture(mesh, tex, labels, ero_dist=default_ero_dist,
                  dilation=default_dil, min_cc_size=default_min_cc_size,
                  max_threads=0):
    ''' Clean labels texture:

    - for each label in the nomenclature:
      - erode a certain amount depending on the label to eliminate small crap
      - dilate the same amount + ``dilation`` mm, to grow back and to connect
        disconnected parts
      - re-erode back to original size
    - Voronoi for all regions to fill gaps
    - filter out small disconnected parts (< ``min_cc_size`` mm2)
    - set a labels and colors table in the texture

    Parameters
    ----------
    mesh: Aims mesh
    tex: Aims texture
    labels: dict
        labels map, normally obtained using :func:`read_labels`
    ero_dist: dict
    dilation: float
    min_cc_size: float
    max_threads: int
        0: all CPU cores
        1: mono-core
        2+: that number of worker threads
        -n: all but n cores

    Returns
    -------
    otex: Aims texture
        cleaned output texture
    '''

    if isinstance(mesh, str):
        mesh = aims.read(mesh)
    if isinstance(tex, str):
        tex = aims.read(tex)
    if isinstance(labels, str):
        labels = read_labels(labels)

    otex = aims.TimeTexture_S16()
    #otex[0].assign([0] * len(tex[0]))
    otex[0].assign(np.round(tex[0].np).astype(np.int16))
    sides = ['l', 'r']
    side = 0
    # aims.write(otex, '/tmp/otex_%s_init.gii' % sides[side])
    used = np.unique(tex[0].np)

    # make unkonwn the 1st region so that others grow over it, not the
    # contrary
    lvalues = [l for l, lv in labels.items() if lv['Label'] == 'unknown'] \
        + [l for l, lv in labels.items() if lv['Label'] != 'unknown']

    # handle parallel processing
    q = queue.Queue()
    workers = mpfork.allocate_workers(q, max_threads)
    print('n workers:', len(workers))

    res = [None] * len(lvalues)
    i = 0
    for lvalue in lvalues:
        label_def = labels[lvalue]
        label = label_def['Label']
        # color = label_def.get('RGB')
        #if color is None:
            #color = [float(i) / (len(labels) - 1) for x in range(3)] + [1.]
            #print('modify color for', label, ':', color)
            #label_def['RGB'] = color

        if lvalue not in used:
            continue

        job = (i, _clean_one_tex,
               (tex, mesh, otex, lvalue, label, ero_dist, dilation),
               {}, res)
        q.put(job)
        i += 1

    for i in range(len(workers)):
        q.put(None)
    # wait for every job to complete
    q.join()
    # terminate all threads
    for w in workers:
        w.join()
    for onp in res:
        if onp is None:
            continue
        otex[0].np[onp!=-32000] = onp[onp!=-32000]

    # 4. Voronoi for all regions
    otex = aims.meshdistance.MeshVoronoi(mesh, otex, -1, -2, 10000, True,
                                          True)

    # 5. filter out small disconnected parts
    if min_cc_size > 0:
        workers = mpfork.allocate_workers(q, max_threads)
        res = [None] * len(labels)
        i = 0
        for lvalue, label_def in labels.items():
            label = label_def['Label']
            if lvalue not in used:
                continue
            job = (i, _filter_cc, (mesh, otex, lvalue, min_cc_size),
                   {}, res)
            q.put(job)
            i += 1
        njobs = i

        for i in range(len(workers)):
            q.put(None)
        # wait for every job to complete
        q.join()
        # terminate all threads
        for w in workers:
            w.join()

        changed = False
        for resval in res[:njobs]:
            c, onp, lvalue = resval
            if not c:
                continue
            otex[0].np[otex[0].np == lvalue] = -1
            otex[0].np[onp==lvalue] = lvalue
            changed = True

        if changed:
            # perform another voronoi to fill the new holes
            otex = aims.meshdistance.MeshVoronoi(mesh, otex, -1, -2, 10000,
                                                 True, True)

    # write result
    otex.header()['GIFTI_labels_table'] = labels
    otex.header()['texture_properties'] = [{'interpolation': 'rgb'}]
    #otex.header()['palette'] = {'palette': 'parcellation720'}

    # check size changes
    for lvalue, label_def in labels.items():
        label = label_def['Label']
        s1 = len(np.where(tex[0].np == lvalue)[0])
        if s1 != 0:
            s2 = len(np.where(otex[0].np == lvalue)[0])
            if s2 < 0.5 * s1:
                col = '0;31'  # red
            elif s2 < 0.75 * s1:
                col = '0;33'  # orange
            elif s2 > 2 * s1:
                col = '0;35'  # purple
            elif s2 > 1.5 * s1:
                col = '1;35'  # light purple
            else:
                col = '0;32'  # green
            print(lvalue, ':', label, ', size:\033[%sm' % col,
                  s1, '->', s2, '\033[0m')

    return otex


def main(argv=sys.argv):

    import argparse
    import textwrap

    ## v 2.4
    #tex_names = [
        #'lh.JulichBrain_MPMAtlas_l_N10_nlin2Stdicbm152asym2009c_publicDOI_83fb39b2811305777db0eb80a0fc8b53.BV_MNI152_orig_to_hcp32k.gii',
        #'rh.JulichBrain_MPMAtlas_r_N10_nlin2Stdicbm152asym2009c_publicDOI_172e93a5bec140c111ac862268f0d046.BV_MNI152_orig_to_hcp32k.gii'
    #]

    #mesh_names = ['lh.r.white.gii', 'rh.r.white.gii']
    #otex_names = ['/tmp/otex_l.gii', '/tmp/otex_r.gii']
    #nomenclature = 'julichbrain_nomenclature.txt'

    class MultilineFormatter(argparse.HelpFormatter):
        def _fill_text(self, text, width, indent):
            text = text.replace('\n\n* ', '|n |n * ')
            text = text.replace('\n* ', '|n * ')
            text = text.replace('\n\n', '|n |n ')
            text = self._whitespace_matcher.sub(' ', text).strip()
            paragraphs = text.split('|n ')
            multiline_text = ''
            for paragraph in paragraphs:
                formatted_paragraph = textwrap.fill(paragraph, width, initial_indent=indent, subsequent_indent=indent) + '\n\n'
                multiline_text = multiline_text + formatted_paragraph
            return multiline_text

    labels_doc_lines = read_labels.__doc__.split('\n')
    labels_doc_lines2 = []
    for l in labels_doc_lines[2:]:
        if l.strip() == 'Returns':
            break
        labels_doc_lines2.append(l)
    labels_doc = '\n'.join(labels_doc_lines2)

    parser = argparse.ArgumentParser(
        description='Clean parcellation textures (developed initially for '
        'JulichBrain projections). Should normally be used after volume to '
        'texture projection using AimsVol2Tex.\n\n'
        'The nomenclature is basically an int/string labels conversion table, '
        'but may include colors in addition to build a consistent colormap. '
        'The nomenclature file may be given in different formats: JSON, CSV, '
        'or HIE.\n\n' + labels_doc,
        formatter_class=MultilineFormatter)
    parser.add_argument('-t', '--texture', nargs='*', help='input textures')
    parser.add_argument('-m', '--mesh', nargs='*', help='input meshes')
    parser.add_argument('-o', '--output', nargs='*', help='output textures')
    parser.add_argument('-n', '--nomenclature',
                        help='nomenclature int->string table file '
                        '(JSON, or CSV, or HIE)')
    parser.add_argument('-e', '--erosion', default=default_ero_dist,
                        type=json.loads,
                        help='erosion distances dict to remove small parts. Default: %s' % repr(default_ero_dist))
    parser.add_argument('-d', '--dilation', default=default_dil, type=float,
                        help='dilation distance (mm) to connect disconnected '
                        'regions. Default: %f' % default_dil)
    parser.add_argument('-s', '--minsize', default=default_min_cc_size,
                        type=float,
                        help='minimum disconnected component size (in mm2) '
                        'under which remaining small parts are removed. '
                        'Default: %f' % default_min_cc_size)
    parser.add_argument('-p', '--proc', default=0, type=int,
                        help='Use parallel computing using this number of '
                        'processors (cores). 0=all in the machine, positive '
                        'number=this number of cores, negative number=all but '
                        'this number. Default: 0 (all)')

    options = parser.parse_args(argv[1:])
    tex_names = options.texture
    mesh_names = options.mesh
    otex_names = options.output
    nomenclature = options.nomenclature
    ero_dist = options.erosion
    dil = options.dilation
    min_size = options.minsize
    nproc = options.proc

    if not mesh_names or not tex_names or not otex_names or len(mesh_names) != len(tex_names) or len(mesh_names) != len(otex_names):
        raise ValueError(
            'texture, mesh, and output parameters must be specified, with the '
            'same number of values')

    for mname, tname, otex_name in zip(mesh_names, tex_names, otex_names):
        otex = clean_texture(mname, tname, nomenclature, ero_dist=ero_dist,
                             dilation=dil, min_cc_size=min_size,
                             max_threads=nproc)
        aims.write(otex, otex_name)


if __name__ == '__main__':

    main()

