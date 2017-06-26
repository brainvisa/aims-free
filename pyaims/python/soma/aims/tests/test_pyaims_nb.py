from __future__ import print_function
import os
import subprocess
import tempfile
import re
import sys

import nbformat
from jupyter_core.command import main as main_jupyter


def notebook_run(path):
    """Execute a notebook via nbconvert and collect output.
       :returns (parsed nb object, execution errors)

       from: http://blog.thedataincubator.com/2016/06/testing-jupyter-notebooks/
    """
    dirname, __ = os.path.split(path)
    os.chdir(dirname)
    with tempfile.NamedTemporaryFile(suffix=".ipynb") as fout:
        args = ["jupyter", "nbconvert", "--to", "notebook", "--execute",
          "--ExecutePreprocessor.timeout=60",
          "--ExecutePreprocessor.kernel_name=python%d" % sys.version_info[0],
          "--output", fout.name, path]
        try:
            old_argv = sys.argv
            sys.argv = args
            sys.argv[0] = re.sub(r'(-script\.pyw|\.exe)?$', '', sys.argv[0])

            try:
                sys
                ret_code = main_jupyter()
                #subprocess.check_call(args)

                fout.seek(0)
                nb = nbformat.read(fout, nbformat.current_nbformat)
            except Exception as e:
                return None, [e]
        finally:
          sys.argv = old_argv

    errors = [output for cell in nb.cells if "outputs" in cell
                     for output in cell["outputs"]\
                     if output.output_type == "error"]

    return nb, errors


def main(argv=sys.argv):

    print('ARGV:', argv)

    nb_dir = os.path.dirname(argv[0])
    nb_file = os.path.join(nb_dir, "pyaims_tutorial_nb.ipynb")
    print("running notebook test for", nb_file)
    nb, errors = notebook_run(nb_file)

    if len(errors) == 0:
        code = 0
    else:
        code = 1
    return code


if __name__ == '__main__':
    sys.exit(main(sys.argv))
