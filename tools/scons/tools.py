import copy
import os
#env = Environment(ENV = os.environ)
env = Environment()

env["CC"   ] = env['CROSS'] + "gcc"
env["CXX"  ] = env['CROSS'] + "g++"
env["LD"   ] = env['CROSS'] + "g++"
env["AR"   ] = env['CROSS'] + "ar"
env["STRIP"] = env['CROSS'] + "strip"
env["PATH"] = env["CROSS_DIR"]





# env_options={
#     "CC"    :   "aarch64-himix100-linux-gcc",
#     "CXX"   :   "aarch64-himix100-linux-g++",
#     "LD"    :   "aarch64-himix100-linux-g++",
#     "AR"    :   "aarch64-himix100-linux-ar",
#     "STRIP" :   "aarch64-himix100-linux-strip",
# }
# env = Environment(**env_options)
# env.Append(ENV={'PATH':os.environ['PATH']})
# Export('env')

# src = Glob('src/*.c')
# src = src + Glob('*.cpp')
# include_path = [
#     'src/',
#     'lib/'
# ]
# lib_path = 'lib/'
# static_libs =[
#      lib_path + 'libtestvector.a'
# ]

# execute = env.Program(
#     target = 'test',
#     #LIBPAHT = lib_path,
#     LIBS = [File(lib) for lib in static_libs],
#     source = src,
#     CPPPATH = include_path,
# )

# env.Alias('all',[execute])
# Default('all')
