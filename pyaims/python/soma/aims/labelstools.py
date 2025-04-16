'''
Manipulate labels maps in several formats (json, csv, xml, hie...) in a GIFTI-colormap compatible dict.

Such labels maps can be saved in textures or volumes headers. In a GIFTI texture, it will be saved in native GIFTI colormap format, otherwise it will be saved in the object .minf. Anatomist will read and use it.

'''


from soma import aims
import json
import os.path as osp
import xml.etree.cElementTree as ET
import argparse
import numpy as np


read_labels_formats = ['json', 'hie', 'csv', 'xml']


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

    - XML (JulichBrain niftis)

    Returns
    -------
    labels: dict
        {12: {"Label": "rototo", "RGB": [1., 1., 0., 1.]}, ...}
    '''

    ext = osp.splitext(nomenclature)[1][1:]
    if ext:
        formats = [ext] + [format for format in read_labels_formats
                           if format !=ext]
    else:
        formats = read_labels_formats

    for format in formats:
        exc = None
        if nomenclature.endswith('.%s' % format):
            read_func = globals().get('read_labels_%s' % format)
            if read_func:
                try:
                    return read_func(nomenclature)
                except Exception as e:
                    if not exc:
                        exc = e
                    pass
        if exc:
            raise exc


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


def read_labels_json(nomenclature):
    '''
    Read a nomenclature as labels dict. Used by :func:`read_labels`, does
    not need to be called directly.

    JSON (JulichBrain nomenclature v2.9)::

        {"properties": {"regions": [<region_def>, ...]}}

        region_def:
        {
            "labelIndex": "12",
            "name": "rototo",
            "color": [255, 255, 0],
            "children": [<region_def>, ...]
        }
    '''
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

    return labels


def read_labels_csv(nomenclature):
    '''
    Read a nomenclature as labels dict. Used by :func:`read_labels`, does
    not need to be called directly.

    CSV (JulichBrain nomenclature)::

        12, rototo

    (all colors will be black)
    '''
    with open(nomenclature) as f:
        labels = f.read().strip().split('\n')
    labels = [l.strip().split() for l in labels]
    labels = {int(l[0]): {'Label': l[1]} for l in labels}
    # process void also
    labels[0] = {'Label': 'unknown', 'RGB': [0., 0., 0., 1.]}

    return labels


def read_labels_xml(nomenclature):
    '''
    Read a nomenclature as labels dict. Used by :func:`read_labels`, does
    not need to be called directly.

    XML (JulichBrain v2.9-)
    '''
    xml_trees = [ET.parse(nomenclature)]

    gifti_label_map = {}

    todo = []
    for xml_tree in xml_trees:
      todo += xml_tree.getroot()[0][:]
    while todo:
        region = todo.pop(0)
        # print(region.text, region.items())
        if region.get('grayvalue'):
            int_k = int(region.get('grayvalue'))
        else:
            int_k = int(region.get('id'))
        label = region.text
        rgb = region.get('color')
        if not rgb:
            rgb = np.random.rand(4)
            print('assign random color for', label, ':', rgb)
            rgb[3] = 1.
        else:
            rgb = rgb.split('(')[-1].split(')')[0].split(',')
            rgb = [float(s.strip()) / 255. for s in rgb]
        gifti_label_map[int_k] = {'Label': label, 'RGB': rgb}

        todo = region[:] + todo

    return gifti_label_map


def read_labels_bsa(nomenclature):
    '''
    Read a nomenclature as labels dict.

    CSV (Brain Sulci Atlas, BrainVisa 2008 nomenclature)::

        i, x, y, z, r, g, b, brainvisa_acronym, label
    (all colors will be black)
    '''
    with open(nomenclature) as f:
        labels = f.read().strip().split('\n')
    labels = [l.strip().split(',') for l in labels[1:]]
    labels = {
        int(l[0]): {'Label': l[7],
                    'RGB': [float(l[4]), float(l[5]), float(l[6])],
                    'FullLabel': l[8]}
        for l in labels}

    return labels


def set_labels_to_object(aims_object, gifti_label_map):
    '''
    Set a labels map (as obtained by read_labels(), or from a GIFTI texture
    header, into an Aims object header
    '''
    aims_object.header()['GIFTI_labels_table'] = gifti_label_map
    if not type(aims_object).__name__.startswith('Volume'):
        aims_object.header()['texture_properties'] = [{'interpolation': 'rgb'}]


if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        description='Set JulichBrain labels maps and colors into a '
        'Gifti texture or volume')
    parser.add_argument('-i', '--input',
                        help='input AIMS object (texture/volume...)')
    parser.add_argument('-o', '--output',
                        help='output AIMS object')
    parser.add_argument('-n', '--nomenclature', action='append',
                        default=[],
                        help='json, xml, csv, hie nomenclature. Several '
                        'values are allowed')

    options = parser.parse_args()
    #print(options)

    if options.nomenclature:
        ontology_fnames = options.nomenclature
    tex_fname = options.input
    out_fname = options.output

    tex = aims.read(tex_fname)

    labels_maps = [read_labels(f) for f in ontology_fnames]
    labels_map = {}
    for lm in labels_maps:
        labels_map.update(lm)
    set_labels_to_object(tex, labels_map)

    print('save:', out_fname)
    aims.write(tex, out_fname)


