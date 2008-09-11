TEMPLATE=	subdirs

# on solaris/irix SUBDIRS can't be empty
# because it causes a shell error
SUBDIRS = cartoConversion_test

PSUBDIRS = \
  cartoAlgorithm_test \
  cartoMinf_test \
  cartoMutex_test \
  cartoMultiThread_test \
  cartoObject_test \
  cartoPythonIO_test \
  cartoRcptr_test \
  cartoScopedptr_test \
  cartoSharedptr_test \
  cartoThreadedRcptr_test \
  cartoUuid_test \
  cartoWeakptr_test

