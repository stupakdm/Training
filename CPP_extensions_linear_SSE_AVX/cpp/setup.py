from setuptools import setup
from torch.utils.cpp_extension import BuildExtension, CppExtension

setup(
    name='linear_cpp',
    ext_modules=[
        CppExtension(
        	name = 'linear_cpp', 
        	sources = ['linear.cpp'],
        	extra_compile_args=['-march=haswell', '-ffast-math'])
    ],
    cmdclass={
        'build_ext': BuildExtension
    })
