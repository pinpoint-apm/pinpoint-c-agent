from setuptools import setup, Extension, find_namespace_packages
import platform
from pathlib import Path

with open("README", "r") as fh:
    long_description = fh.read()

# DO NOT edit by hand. Managed by CI (sync-component-versions).
VERSION = "1.4.2"

name = platform.system().lower()
agent_libraries = []
# Build against the Python Stable ABI (abi3) so a single wheel covers all
# supported Python versions (>=3.8) per architecture, instead of one wheel
# per Python version. The extension only uses stable-ABI CPython APIs.
extra_compile_args_ = ["-DPINPOINT_MT", "-DPy_LIMITED_API=0x03080000"]
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
      version=VERSION,
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
                    extra_compile_args=extra_compile_args_,
                    py_limited_api=True
                    )
      ],
      package_dir={'': 'plugins/PY'},
      packages=find_namespace_packages(
          'plugins/PY', include=['pinpointPy.*', 'pinpointPy']),
      )

"""
# Changed
## 1.4.0 [not released]
- add new API `is_root_trace`
- asynchronous API
## 1.3.1
- fix bug https://github.com/pinpoint-apm/pinpoint-c-agent/issues/626
## 1.3.0
- support error analysis
## 1.2.1
- windows build https://github.com/pinpoint-apm/pinpoint-c-agent/releases/tag/v0.5.0
"""
