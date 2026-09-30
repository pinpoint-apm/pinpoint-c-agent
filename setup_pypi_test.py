from setuptools import setup, Extension, find_namespace_packages
import platform
import json
from pathlib import Path

with open("README", "r") as fh:
    long_description = fh.read()

# Single source of truth for the Python agent version lives in repo-root
# versions.json.
with open("versions.json", "r") as f:
    VERSION = json.load(f)["python"]

name = platform.system().lower()
agent_libraries = []
extra_compile_args_ = ["-DPINPOINT_MT"]
if name == 'windows':
    pass
elif name == 'darwin':
    agent_libraries = ['stdc++']
    extra_compile_args_.append("-std=c++11")
elif name == 'linux':
    agent_libraries = ['rt', 'stdc++']
    extra_compile_args_.append("-std=c++11")
else:
    raise RuntimeError('Unknown platform to us: ' + name)
###############################################

extFiles = [
    'src/PY/_pinpoint_py.cpp',
]

# add pinpoint-common

for a_file in Path("common/src").glob('**/*.cpp'):
    extFiles.append(str(a_file))

for a_file in Path("common/jsoncpp").glob('**/*.cpp'):
    extFiles.append(str(a_file))

cwd = Path.cwd()
include_dirs_ = [Path(cwd, './common/include'), Path(cwd, './common/jsoncpp/include'),
                 Path(cwd, './common/src')]

setup(name='pinpointPy',
      version=VERSION,  # from versions.json
      author="pinpoint members",
      author_email='dl_cd_pinpoint@navercorp.com',
      license='Apache License 2.0',
      url="https://github.com/pinpoint-apm/pinpoint-c-agent",
      long_description=long_description,
      long_description_content_type='text/markdown',
      ext_modules=[
          Extension('_pinpointPy',
                    extFiles,
                    include_dirs=include_dirs_,
                    libraries=agent_libraries,
                    extra_compile_args=extra_compile_args_
                    )
      ],
      package_dir={'': 'plugins/PY'},
      packages=find_namespace_packages(
          'plugins/PY', include=['pinpointPy.*', 'pinpointPy']),
      )
