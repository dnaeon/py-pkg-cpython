#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name='pkglib',
      version='0.0.1',
      description='Python bindings to libpkg',
      author='Marin Atanasov Nikolov',
      author_email='dnaeon@gmail.com',
      license='BSD',
      ext_modules=[
          Extension(
              'pkglib',
              include_dirs=['/usr/local/include'],
              library_dirs=['/usr/local/lib'],
              libraries=['pkg'],
              sources=['pkglibmodule.c'],
              extra_link_args=['-Wall', '-g'],
          )
      ]
)

