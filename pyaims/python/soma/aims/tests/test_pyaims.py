from __future__ import print_function

import unittest
import doctest
import urllib
try:
    from soma import zipfile
except:
    import zipfile
import os
from shutil import rmtree
import tempfile
import sys


def setup_doctest(test):
    tests_dir = os.getenv("BRAINVISA_TESTS_DIR")
    if not tests_dir:
        tests_dir = tempfile.gettempdir()
    tests_dir = os.path.join(tests_dir, "tmp_tests_brainvisa")
    if not os.path.exists(tests_dir):
        os.mkdir(tests_dir)
    os.chdir(tests_dir)
    if not os.path.exists("demo_data.zip"):
        print("Download ftp://ftp.cea.fr/pub/dsv/anatomist/data/demo_data.zip "
              "to ", tests_dir)
        urllib.urlretrieve(
            "ftp://ftp.cea.fr/pub/dsv/anatomist/data/demo_data.zip",
            "demo_data.zip")
    if os.path.exists("data_for_anatomist"):
        rmtree("data_for_anatomist")
    if os.path.exists("data_unprocessed"):
        rmtree("data_unprocessed")
    zf = zipfile.ZipFile("demo_data.zip")
    zf.extractall()


def teardown_doctest(test):
    os.chdir(test.globs['curdir'])


def test_suite():
    suite = unittest.TestSuite()
    if sys.version_info[0] >= 3:
        tutorial_file = "pyaims_tutorial_py3.rst"
    else:
        tutorial_file = "pyaims_tutorial.rst"
    doctest_suite = unittest.TestSuite(
        doctest.DocFileSuite(tutorial_file,
                             globs={'curdir': os.path.abspath(os.curdir)},
                             setUp=setup_doctest,
                             tearDown=teardown_doctest,
                             optionflags=doctest.ELLIPSIS))
    suite.addTest(doctest_suite)
    return suite

if __name__ == '__main__':
    unittest.main(defaultTest='test_suite')
