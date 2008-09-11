TEMPLATE	= lib
TARGET		= aimsalgopub${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS= config/aimsalgopub_config.h		\
		ffd/tabulSpline.h			\
		information/information.h         \
		information/pdf.h                 \
		math/math_g.h			\
		math/balanc.h			\
		math/balbak.h			\
		math/bessel.h			\
		math/curv2Diso.h		\
		math/curv3Diso.h		\
		math/discriminantanalysis.h    \
		math/discriminantanalysis_d.h    \
		math/distspheric.h		\
		math/eigen.h			\
		math/elmhes.h			\
		math/eltran.h			\
		math/gaussj.h			\
		math/geometry.h			\
		math/gradient.h			\
		math/householder.h		\
		math/hqr.h			\
		math/hypergeometric.h		\
		math/knn.h			\
		math/multilinfit.h		\
		math/random.h			\
		math/svd.h			\
		math/pca.h			\
		math/pca_d.h			\
		math/ppca.h			\
		math/ppca_d.h			\
		math/bspline2.h                 \
		math/bspline3.h                 \
		math/tensorMatrix.h		\
		math/tqli.h			\
		optimization/covsrt.h			\
		optimization/deterministic.h		\
		optimization/levmrq.h			\
		optimization/lm2gauss.h		\
		optimization/lmfunc.h			\
		optimization/lmgamma.h			\
		optimization/lmgauss.h			\
#		optimization/lmmultcompmodel.h		\
		optimization/objective.h		\
		optimization/optimization_g.h		\
		optimization/optimizer.h		\
		optimization/powell.h			\
		optimization/singleoptimizer.h		\
		pyramid/maxfunc.h			\
		pyramid/meanfunc.h			\
		pyramid/medfunc.h			\
		pyramid/pyramid_g.h			\
		pyramid/pyramid.h			\
		pyramid/pyr-func.h			\
		resampling/bucketmapsampler.h	\
		resampling/cubicresampler.h		\
		resampling/cubicresampler_d.h		\
		resampling/generalsampler.h	\
		resampling/linearresampler.h		\
		resampling/linearresampler_d.h		\
		resampling/masklinresampler.h		\
		resampling/nearestneighborresampler.h	\
		resampling/nearestneighborresampler_d.h	\
		resampling/quadraticresampler.h	\
		resampling/quadraticresampler_d.h	\
		resampling/quarticresampler.h	\
		resampling/quarticresampler_d.h	\
		resampling/quinticresampler.h	\
		resampling/quinticresampler_d.h	\
		resampling/resampling_g.h		\
		resampling/resampler.h			\
		resampling/resampler_d.h		\
		resampling/resamplerfactory.h		\
		resampling/resamplerfactory_d.h	\
		resampling/samplable.h	\
		resampling/samplable_cone.h	\
		resampling/samplable_sphere.h	\
		resampling/sampler.h	\
		resampling/sixthorderresampler.h	\
		resampling/sixthorderresampler_d.h	\
		resampling/seventhorderresampler.h	\
		resampling/seventhorderresampler_d.h	\
		resampling/splineresampler.h		\
		resampling/splineresampler_d.h		\
		resampling/polynomial.h


SOURCES = \
                information/corratio.cc		       \
		information/mutual.cc		       \
		ffd/tabulSpline.cc                \
		information/pdf.cc		          \
		math/balanc.cc			\
		math/balbak.cc			\
		math/bessel.cc			\
		math/bspline3.cc		\
		math/bspline2.cc		\
		math/discriminantanalysis.cc    \
		math/eigen.cc			\
		math/elmhes.cc			\
		math/eltran.cc			\
		math/gaussj.cc			\
		math/geometry.cc		\
		math/householder.cc		\
		math/hqr.cc			\
		math/hypergeometric.cc		\
		math/knn.cc			\
		math/icosahedronDistrib.cc	\
		math/multilinfit.cc		\
		math/randlist.cc		\
		math/random.cc			\
		math/spheredistrib.cc		\
		math/svd.cc			\
		math/pca.cc			\
		math/ppca.cc			\
		math/tensorMatrix.cc		\
		math/toeplitz.cc		\
		math/tqli.cc			\
		optimization/covsrt.cc		       \
		optimization/levmrq.cc		       \
		resampling/bucketmapsampler.cc	\
		resampling/generalsampler.cc	\
		resampling/resamplers.cc	\
		resampling/samplable.cc	\
		resampling/polynomial.cc
