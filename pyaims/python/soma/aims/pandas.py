''' Wrapper for Pandas which should work on MacOS 10.5 with python 2.6
'''

from __future__ import absolute_import

import os, sys
if sys.version_info[:2] <= (2, 6):
    # fix for python2.6: patch collections module
    # because pandas needs them to be in collections
    import collections
    import ordereddict # should be installed (using pip/easy_install)
    import counter # should be installed (using pip/easy_install)
    collections.OrderedDict = ordereddict.OrderedDict
    collections.Counter = counter.Counter
    from soma import aims

from pandas import *

