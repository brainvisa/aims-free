#!/usr/bin/env python

import sys
from soma import aims
import numpy as np


def get_mem_use():
    with open('/proc/self/statm') as f:
        line = f.readline()
    return int(line.split()[0])


def alloc_vol(dims):
    vol = aims.Volume_FLOAT(dims)
    vol.fill(38.38)
    return vol


def alloc_rc_vol(dims):
    vol = aims.rc_ptr_Volume_FLOAT(aims.Volume_FLOAT(dims))
    vol.fill(38.38)
    return vol


def test_vol():
    vol = aims.Volume_FLOAT([1000, 1000, 100])
    vol.fill(38.38)
    return vol


def test_rc_vol():
    vol = aims.rc_ptr_Volume_FLOAT(aims.Volume_FLOAT([1000, 1000, 100]))
    vol.fill(38.38)
    return vol


def test_vector():
    arr = np.ones((100000000, 3), dtype=np.float32)
    vec = aims.vector_POINT3DF(arr)
    vec.np[:, :] = 38.38
    vec2 = aims.vector_POINT3DF(vec)
    vec2.assign(np.ones((50000000, 3), dtype=np.float32))
    return vec2


def repeat_test(ninit, ntest, test_func):
    result = True
    mem0 = get_mem_use()
    print('mem0:', mem0)

    for i in range(ninit):
        item = test_func()
    mem1 = get_mem_use()
    print('mem1:', mem1)

    for i in range(ntest):
        item = test_func()
    mem2 = get_mem_use()
    print('mem2:', mem2)

    del item
    mem3 = get_mem_use()
    print('mem3 (freed):', mem3)

    if mem2 != mem1:
        result = False

    return result


def test_leak():
    if not sys.platform.startswith('linux'):
        # skip this test on non-linux platforms because I don't know how to
        # test memory usage
        return True

    result = repeat_test(5, 5, test_vol)
    result &= repeat_test(5, 5, test_rc_vol)
    result &= repeat_test(5, 5, test_vector)

    return result


if __name__ == '__main__':
    res = test_leak()
    sys.exit(0 if res else 1)


