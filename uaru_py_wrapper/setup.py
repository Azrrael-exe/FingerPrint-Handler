from distutils.core import setup, Extension

makefile = Extension(
    'u_are_u',
    include_dirs = ['/opt/DigitalPersona/urusdk-linux/Include'],
    libraries = ['dpfpdd', 'dpfj'],
    library_dirs = ['/opt/DigitalPersona/urusdk-linux/Linux/lib'],
    sources = ['_u_are_u.c', 'u_are_u.c'],
)

setup(ext_modules=[makefile])
