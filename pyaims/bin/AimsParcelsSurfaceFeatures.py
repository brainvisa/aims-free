#!/usr/bin/env python

from soma import aims
from soma.aims import texturetools
from soma.utils import csv_utils
import argparse
import json
import csv
try:
    import yaml
except:
    yaml = None  # well, OK


parser = argparse.ArgumentParser(
    'Record surface parcel-level features (surface, boundary length) on a set '
    'of clusters textures')
parser.add_argument('-i', '--input', action='append', help='input textures')
parser.add_argument('-o', '--output', help='output features (.json or .csv)')
parser.add_argument('-m', '--mesh', action='append', help='input meshes')
parser.add_argument('-a', '--attribute', action='append',
                    help='item attributes. Each -a item corresponds to an '
                    'input texture. Attributes will be stored in the features '
                    'file as index keys. A value here is parsed as a JSON '
                    'dict. A special key, "index" allows to use only the '
                    'specified timestep in the corresponding input texture '
                    '(otherwise all timesteps are used)')
options = parser.parse_args()

tex_names = options.input
mesh_names = options.mesh
out_features_name = options.output
attribs = [json.loads(x) for x in options.attribute]

print(tex_names)
print(mesh_names)
print(attribs)
print(out_features_name)

mesh = None
init_tex_index = -1
for sattribs in attribs:
    if sattribs.get('index') not in (None, -1):
        init_tex_index = 0

features = {}
for i, tex_name in enumerate(tex_names):
    print('processing %d/%d...' % (i, len(tex_names)))
    if i >= len(mesh_names):
        if mesh is None:
            mesh = aims.read(mesh_names[-1])
    else:
        mesh = aims.read(mesh_names[i])
    if len(attribs) > i:
        sattribs = attribs[i]
    else:
        sattribs = {}

    if 'index' in sattribs:
        tex_index = sattribs['index']
        sattribs = dict(sattribs)
        del sattrib['index']
    else:
        tex_index = init_tex_index

    texture = aims.read(tex_name)
    tfeat = texturetools.parcels_surface_features(
        mesh, texture, tex_index=tex_index, as_csv_table=False)
    # tfeat.update(sattribs)
    features[i] = tfeat

shape_subdict = {'areas': {'parcel': 'area'},
                 'lengths': {'parcel': 'length'}}
if init_tex_index < 0:
    shape_subdict = {'timestep': shape_subdict}
shape_dict = {'tex_index': shape_subdict}

if out_features_name.endswith('.csv'):
    features = csv_utils.dict_to_table(features, shape_dict)
    with open(out_features_name, 'w') as f:
        writer = csv.writer(f)  # , dialect='csv')
        for row in features:
            writer.writerow(row)
else:
    if out_features_name.endswith('.yaml') and yaml is not None:
        write_mod = yaml
    else:
        write_mod = json
    features['shape'] = shape_dict
    with open(out_features_name, 'w') as f:
        write_mod.dump(features, f, indent=4)

