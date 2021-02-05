#!/usr/bin/env python

'''Maniupulate the plugins loader to avoid loading some of them, or to load additional plugins afterwards
'''
from __future__ import absolute_import
from soma import aims

dicomfile = 'toto.dcm'

pl = aims.carto.PluginLoader
pluginfiles = pl.pluginFiles()

# remove aimsdata plugins
todel = []
i = 0
for f in pluginfiles:
    if f.filename.endswith('aimsdata.plugins'):
        todel.append(i)
    i += 1
todel.reverse()
for i in todel:
    del pluginfiles[i]

# try to read an object which needs a specific IO plugin
vol = aims.read(dicomfile)  # Fails

# now set back plugins
import os
import glob
aimspath = glob.glob(
    os.path.join(aims.carto.Paths.globalShared(), 'aims-*', 'plugins', 'aimsdata.plugins'))[0]
pluginfiles.append(pl.PluginFile(aimspath))
pl.load()

vol = aims.read(dicomfile)  # succeeds
