from distutils.core import setup, Extension

setup(
    ext_modules=[Extension("_wrapper", ["_wrapper.c", "wrapper.c"])],
    # include_dirs=numpy.distutils.misc_util.get_numpy_include_dirs(),
)
