from setuptools import setup, Extension
from distutils.command.build_ext  import build_ext 
import os,subprocess,sys

with open("README", "r") as fh:
    long_description = fh.read()

if sys.version_info[0] == 3:
    class CommonBuild(build_ext):
        build_temp = 'build'
        def build_common(self):
            if not os.path.exists(CommonBuild.build_temp):
                os.makedirs(CommonBuild.build_temp)

            comm_path = os.path.abspath('common')
            subprocess.check_call(['cmake','-DCMAKE_BUILD_TYPE=Debug',comm_path],cwd=CommonBuild.build_temp)
            subprocess.check_call(['cmake', '--build', '.'], cwd=CommonBuild.build_temp)

        def run(self):
            try:
                out = subprocess.check_output(['cmake', '--version'])
            except OSError:
                raise RuntimeError("CMake must be installed to build the following extensions: " +
                                    ", ".join(e.name for e in self.extensions))
            self.build_common()
            super().run()

    pinpointBuild = CommonBuild

else:
    class CommonBuild(build_ext,object):
        build_temp = 'build'
        def build_common(self):
            if not os.path.exists(CommonBuild.build_temp):
                os.makedirs(CommonBuild.build_temp)

            comm_path = os.path.abspath('common')
            subprocess.check_call(['cmake','-DCMAKE_BUILD_TYPE=Debug',comm_path],cwd=CommonBuild.build_temp)
            subprocess.check_call(['cmake', '--build', '.'], cwd=CommonBuild.build_temp)

        def run(self):
            try:
                out = subprocess.check_output(['cmake', '--version'])
            except OSError:
                raise RuntimeError("CMake must be installed to build the following extensions: " +
                                    ", ".join(e.name for e in self.extensions))
            self.build_common()
            super(CommonBuild,self).run()

    pinpointBuild = CommonBuild

setup(name='pinpointPy',
      version="1.0.1",
      author="eeliu", 
      author_email='eeliu2009@gmail.com',
      license='Apache License 2.0',
      url="https://github.com/pinpoint-apm/pinpoint-c-agent",
      long_description=long_description,
      long_description_content_type='text/markdown',
      ext_modules=[
        Extension('pinpointPy',
          ['src/PY/pinpoint_py.c'],
          include_dirs = ['common/include'],
          library_dirs = [pinpointBuild.build_temp+"/lib"],
          libraries = ['pinpoint_common','jsoncpp', 'rt', 'stdc++']
          )
        ],
      cmdclass={'build_ext': pinpointBuild}
)