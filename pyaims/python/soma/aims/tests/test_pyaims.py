from __future__ import print_function

from __future__ import absolute_import
import unittest
import doctest
import six.moves.urllib.request, six.moves.urllib.parse, six.moves.urllib.error
import zipfile
import os
from shutil import rmtree
import tempfile
import sys

if sys.version_info[0] >= 3:
    from urllib import request as urllib


def setup_doctest(test):
    tests_dir = os.getenv("BRAINVISA_TEST_RUN_DATA_DIR")
    if not tests_dir:
        tests_dir = tempfile.gettempdir()
    tests_dir = os.path.join(tests_dir, "tmp_tests_brainvisa")
    if not os.path.exists(tests_dir):
        os.mkdir(tests_dir)
    os.chdir(tests_dir)
    if not os.path.exists("demo_data.zip"):
        print("Download ftp://ftp.cea.fr/pub/dsv/anatomist/data/demo_data.zip "
              "to ", tests_dir)
        six.moves.urllib.request.urlretrieve(
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
    tutorial_file = "pyaims_tutorial.rst"
    doctest_suite = unittest.TestSuite(
        doctest.DocFileSuite(tutorial_file,
                             globs={'curdir': os.path.abspath(os.curdir),
                                    'print_function': print_function},
                             setUp=setup_doctest,
                             tearDown=teardown_doctest,
                             optionflags=doctest.ELLIPSIS))
    suite.addTest(doctest_suite)
    return suite

if __name__ == '__main__':
    # try the notebook version if it can be processed on this system
    from soma.test_utils import test_notebook as tnb
    try:
        raise Warning('no notebook')
        if tnb.test_notebook(
            os.path.join(os.path.dirname(sys.argv[0]),
                         'pyaims_tutorial_nb.ipynb'),
            timeout=300):
            sys.exit(0)
        else:
            sys.exit(1)
    except Warning:
        #verbosity=0
        #if '-v' in sys.argv:
            #i = sys.argv.index('-v')
            #verbosity = int(sys.argv[i + 1])
            #sys.argv = sys.argv[:i] + sys.argv[i+2:]
        #elif '--verbose' in sys.argv:
            #i = sys.argv.index('--verbose')
            #verbosity = int(sys.argv[i + 1])
            #sys.argv = sys.argv[:i] + sys.argv[i+2:]

        unittest.main(defaultTest='test_suite')
