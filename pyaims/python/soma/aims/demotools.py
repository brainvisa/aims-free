
'''
Utility module to list and install demo datasets available on the BrainVisa server.

The main functions are :func:`get_demo_datasets` and :func:`install_demo_data`.
'''

from soma import aims
from urllib.request import urlopen
import os
import os.path as osp
import zipfile
import tarfile
import re


def unzip_zip(filename, destdir):
    with open(filename, 'rb') as f:
        zf = zipfile.ZipFile(f, 'r')
        # extract zip files one by one
        # extractall() is not an option since on Mac at least it tries to
        # re-make directories even if they exist
        namelist = zf.namelist()
        fnlist = []
        for name in namelist:
            dname = os.path.join(destdir, name)
            if os.path.exists(dname):
                if os.path.isdir(dname):
                    pass  # skip existing dirs
                else:  # existing file: remove it first
                    os.unlink(dname)
                    fnlist.append(name)
            else:
                fnlist.append(name)
        del namelist
        zf.extractall(destdir, fnlist)

    fnlist = [f[:-1] if f.endswith('/') else f for f in fnlist]
    dirs = {osp.join(destdir, f)
            if osp.isdir(osp.join(destdir, f))
            else osp.dirname(osp.join(destdir, f))
            for f in fnlist}
    n = len(dirs)
    m = n + 1
    while n != m:
        dirs = {d for d in dirs if osp.dirname(d) not in dirs}
        m = n
        n = len(dirs)
    return dirs


def unzip_tar(filename, destdir):
    with tarfile.open(filename, 'r') as tf:
        members = tf.getnames()
        tf.extractall(destdir)

    members = [f[:-1] if f.endswith('/') else f for f in members]
    dirs = {osp.join(destdir, f)
            if osp.isdir(osp.join(destdir, f))
            else osp.dirname(osp.join(destdir, f))
            for f in members}
    n = len(dirs)
    m = n + 1
    while n != m:
        dirs = {d for d in dirs if osp.dirname(d) not in dirs}
        m = n
        n = len(dirs)
    return dirs


def unzip_file(filename, destdir=None):
    ''' unzip / untar the given archive in the directory it is
    '''
    if destdir is None:
        destdir = osp.dirname(filename)
    if filename.endswith('.zip'):
        return unzip_zip(filename, destdir)
    else:
        return unzip_tar(filename, destdir)


def url_listdir(url):
    res = urlopen(url).read().decode()
    r2 = res[res.find('Parent Directory'):]
    rexp = re.compile('<a href="([^"]+)">', re.M)
    m = re.findall(rexp, r2)
    return m


def get_demo_datasets(download_url='https://brainvisa.info/download/data'):
    ''' List available demo datasets on the BrainVisa server
    '''
    files = url_listdir(download_url)
    allowed_exts = {'.zip', '.tar.gz', '.tar.bz2'}
    filtered = []
    for fname in files:
        if fname.endswith('/'):  # dir
            continue
        if len([True for ext in allowed_exts if fname.endswith(ext)]) == 0:
            continue
        if 'descriptive_models' in fname:
            continue
        if '_atlas_' in fname:
            continue
        if 'Atlas' in fname:
            continue
        filtered.append(fname)

    return filtered


def install_demo_data(dataset='test_data.zip', download_dir=None,
                      download_url='https://brainvisa.info/download/data',
                      install_dir=None):
    ''' Downoad a demo dataset from brainvisa server to the ``download_dir`` directory, and install it in the local filesystem in ``install_dir``.
    If ``download_dir`` is not given, the output download location will be guessed as such:

    1. try to write in ``$BRAINVISA_SHARE/brainvisa_demo/``
    2. if ``install_dir`` is provided, download in ``install_dir``
    3. try to wtite in the current directory
    4. raise an error

    If ``install_dir`` is not given, download and install will be done in the same directory, namely ``download_dir``.
    If ``install_dir`` is given, the archive will be extracted to this directory.

    The return value is the dataset directories on the local filesystem.
    '''
    full_url = download_url + '/' + dataset
    if download_dir is None:
        download_dirs = [osp.join(aims.carto.Paths.globalShared(),
                                  'brainvisa_demo'),
                         os.getcwd()]
        if install_dir is not None:
            download_dirs.insert(1, install_dir)
        for download_dir in download_dirs:
            if not osp.exists(download_dir):
                try:
                    os.makedirs(download_dir)
                except Exception:
                    pass
            try:
                with open(osp.join(download_dir, 'testfile'), 'w') as f:
                    f.write('test writing\n')
                ok = True
                os.unlink(osp.join(download_dir, 'testfile'))
            except Exception:
                ok = False
            if ok:
                break  # use this one
        if not ok:
            raise ValueError('Cound not find a suitable writable directory '
                             'for download.')

    print('download dir:', download_dir)
    if install_dir is None:
        install_dir = download_dir
    elif not osp.exists(install_dir):
        os.makedirs(install_dir)
    print('install dir:', install_dir)

    zf = osp.join(download_dir, dataset)
    tzf = zf + '.part'

    ds = dataset.split('.')
    while ds[-1] in ('gz', 'zip', 'tar', 'bz2'):
        del ds[-1]

    with urlopen(full_url) as fr:
        fsize = int(fr.headers.get('content-length'))
        download = True
        if osp.exists(zf) and os.stat(zf).st_size == fsize:
            download = False
            print('already downloaded, skipping download.')

        if download:
            chunksize = 100000
            fread = 0
            with open(tzf, 'wb') as fw:
                while fread < fsize:
                    pg = fread * 100 / fsize
                    print(f'\r{full_url}: {int(pg)}%', end='')
                    data = fr.read(chunksize)
                    fw.write(data)
                    fread += len(data)

                pg = fread * 100 / fsize
                print(f'\r{full_url}: {int(pg)}%')

            if osp.exists(zf):
                os.unlink(zf)
            os.rename(tzf, zf)
            print('download done')

        print('installing', dataset, '...')
        out_dirs = unzip_file(zf, destdir=install_dir)

    return out_dirs
