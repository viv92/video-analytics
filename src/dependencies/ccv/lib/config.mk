CC := gcc
AR := ar
NVCC := 
CUDA_OBJS := 
DEFINE_MACROS := -D HAVE_CBLAS -D HAVE_LIBPNG -D HAVE_LIBJPEG -D HAVE_AVCODEC -D HAVE_AVFORMAT -D HAVE_SWSCALE -D HAVE_SSE2
prefix := /usr/local
exec_prefix := ${prefix}
CFLAGS := -msse2 $(DEFINE_MACROS) -I${prefix}/include -I/usr/local/Cellar/openblas/0.2.20/include
NVFLAGS := --use_fast_math -arch=sm_30 $(DEFINE_MACROS)
LDFLAGS := -L${exec_prefix}/lib -L/usr/local/Cellar/openblas/0.2.20/include -lm -lcblas -latlas -lpng -ljpeg -lavcodec -lavformat -lswscale
