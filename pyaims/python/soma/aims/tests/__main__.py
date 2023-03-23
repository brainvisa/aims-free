import subprocess
import sys

all_res = []

for module in ['test_pyaims_io', 'test_pyaims', # 'test_pyaims_thread_read',
               'test_volume_strides']:
    cmd = [sys.executable, '-m', 'soma.aims.tests.%s' % module] + sys.argv[1:]
    print('run:', cmd)
    res = subprocess.call(cmd)
    all_res.append(res)
    #if res != 0:
        #print('FAILED: %d' % res)
    #else:
        #print('OK')

glob_res = all([res == 0 for res in all_res])
if glob_res:
    sys.exit(0)
else:
    sys.exit(1)
