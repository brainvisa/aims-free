TEMPLATE=	subdirs

# on solaris/irix SUBDIRS can't be empty
# because it causes a shell error

SUBDIRS = cartoIO_test \
  cartoVolume_test

PSUBDIRS = 
