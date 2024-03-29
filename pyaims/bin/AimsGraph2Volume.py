#!/usr/bin/env python

from soma import aims
import argparse


def labels_from_graph(graph, nomenclature):
    '''
    Returns
    -------
    labels_int_map: dict {int: int}
        graph ROI label: nomenclature label
    labels_names: dict {int: str}
        nomenclature label: nomenclature region name
    colors: dict {str: RGBA}
    '''

    def get_free_value(taken):
        n = 0
        while True:
            if n in taken:
                n += 1
            else:
                taken.add(n)
                return n

    nom_labels = {'unknown': 0}
    taken = set([0])
    labels_int_map = {0: 0}
    labels_names = {0: 'unknown'}
    colors = {'unknown': [0, 0, 0, 255]}
    new_labels = {}

    # get labels table
    if nomenclature:
        # parse nomenclature
        todo = [nomenclature]
        while todo:
            tree = todo.pop(0)
            todo += tree.children()
            name = tree.get('name')
            if name is not None:
                color = tree.get('color')
                if color:
                    colors[name] = color
                label = tree.get('label')
                if label is not None:
                    label = int(label)
                    nom_labels[name] = label
                    labels_names[label] = name

    for v in graph.vertices():
        name = v.get('name')
        value = v.get('roi_label')
        if value is None:
            continue
        if name:
            ovalue = nom_labels.get(name)
            if ovalue is not None:
                taken.add(ovalue)
                labels_int_map[value] = ovalue
            else:
                new_labels[value] = name
                print('name', name, 'not in nomenclature')

    for value, name in new_labels.items():
        ovalue = get_free_value(taken)
        print(name, '->', value, ovalue)
        labels_int_map[value] = ovalue
        nom_labels[name] = ovalue
        labels_names[ovalue] = name

    for value, name in labels_names.items():
        if value not in labels_int_map:
            ovalue = get_free_value(taken)
            labels_int_map[value] = ovalue

    return labels_int_map, labels_names, colors


def graph_to_vol(graph, nomenclature, item_attribute):
    bb = graph['boundingbox_max']
    vol = aims.Volume([b+1 for b in bb], dtype='S16')
    vol.fill(0)
    vol.header()['voxel_size'] = graph['voxel_size']
    labels = {}

    # get labels table
    labels_int_map, labels_names, colors \
        = labels_from_graph(graph, nomenclature)

    # print('labels_int_map:', labels_int_map)
    # print('\nlabels_names:', labels_names)
    # print('\ncolors:', colors)

    # determine which are bucket regions
    get = aims.GraphManip.get_element_table(graph)
    get = {k: {ke: kv for ke, kv in v.items()
               if kv['objectType'] == 'Bucket'}
           for k, v in get.items()}

    for v in graph.vertices():
        gec_map = get.get(v.getSyntax())
        if not gec_map:
            continue
        if item_attribute is None and len(gec_map) != 1:
            raise ValueError(
                'several item attributes in graph. You should select one '
                'using the --attribute option. Found: %s'
                % repr(gec_map.keys()))
        if item_attribute is not None:
            gec = gec_map.get(item_attribute)
        else:
            gec = next(iter(gec_map.values()))
        if not gec:
            continue

        value = v.get(gec['global_index_attribute'])
        if value is None:
            continue
        bck = v.get(gec['attribute'])
        if bck is None:
            continue
        if value not in labels_int_map:
            name = v.get('name')
            raise RuntimeError('unknown value %s with name: %s'
                               % (value, name))
        ovalue = labels_int_map[value]
        for p in bck[0].keys():
            vol.setValue(ovalue, p)

    if colors:
        labels = {}
        for value, name in labels_names.items():
            item = {'Label': name}
            #ovalue = labels_int_map[value]
            color = colors.get(name)
            if not color:
                print('no name: %s, setting it to gray' % name)
                color = [128, 128, 128, 255]
            item['RGB'] = [float(c) / 255 for c in color] \
                + [1.] * (4 - len(color))
            labels[value] = item
        vol.header()['GIFTI_labels_table'] = labels
    else:
        #labels = {labels_int_map[value]: name
                  #for value, name in labels_names.items()}
        vol.header()['labels'] = labels_names

    return vol


parser = argparse.ArgumentParser(
    'Convert a ROI graph to a volume, following labels and colors in a '
    'hierarchical nomenclature: values used in the volume will not be the '
    'index in the graph, but the "label" value in the nomenclature: thus the '
    'nomenclature has to have this attribute for each region, and be a '
    'number.\n'
    'If no nomenclature is provided, regions numbers will be generated in an '
    'arbitrary order\n'
    'The output volume will have a labels table with colors in the header '
    'attribute "GIFTI_labels_table", as in Gifti textures.')
parser.add_argument('-i', '--input', help='input graph')
parser.add_argument('-n', '--nomenclature', help='nomenclature (optional)')
parser.add_argument('-o', '--output', help='output volume')
parser.add_argument('-a', '--attribute',
                    help='item attribute id (bucket attribute id in '
                    'vertices). Default: use the one available if only one is '
                    'present')
options = parser.parse_args()

graph_f = options.input
nomenclature_f = options.nomenclature
vol_out_f = options.output
item_attribute = options.attribute

print(graph_f, nomenclature_f, vol_out_f)

graph = aims.read(graph_f)
nomenclature = None
if nomenclature_f:
    nomenclature = aims.read(nomenclature_f)

vol = graph_to_vol(graph, nomenclature, item_attribute)
aims.write(vol, vol_out_f)


