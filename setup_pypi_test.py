from setuptools import setup, Extension, find_namespace_packages, find_packages
from distutils.command.build_ext import build_ext
import platform
from pathlib import Path

with open("README", "r") as fh:
    long_description = fh.read()

name = platform.system().lower()
agent_libraries = []
if name == 'windows':
    pass
    # agent_libraries: ['stdc++']
elif name == 'darwin':
    agent_libraries = ['stdc++']
elif name == 'linux':
    agent_libraries = ['rt', 'stdc++']
else:
    raise RuntimeError('Unknow platform to me: '+name)
###############################################

extFiles = [
    'src/PY/_pinpoint_py.cpp',
]

# add pinpoint-common

for a_file in Path("common/src").glob('**/*.cpp'):
    extFiles.append(str(a_file))

for a_file in Path("common/jsoncpp").glob('**/*.cpp'):
    extFiles.append(str(a_file))

setup(name='pinpointPy',
      version="1.2.1",  # don't forget update __version__ in pinpointPy/__init__.py
      author="cd_pinpoint members",
      author_email='dl_cd_pinpoint@navercorp.com',
      license='Apache License 2.0',
      url="https://github.com/pinpoint-apm/pinpoint-c-agent",
      long_description=long_description,
      long_description_content_type='text/markdown',
      ext_modules=[
          Extension('_pinpointPy',
                    extFiles,
                    include_dirs=['common/include',
                                  'common/jsoncpp/include', 'common/src'],
                    libraries=agent_libraries,
                    extra_compile_args=['-std=c++11']
                    )
      ],
      package_dir={'': 'plugins/PY'},
      packages=find_namespace_packages(
          'plugins/PY', include=['pinpointPy.*', 'pinpointPy']),
      )
