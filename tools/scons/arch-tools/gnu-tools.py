
# 创建使用 GNU 工具链的环境
env = Environment(tools=['gcc', 'g++', 'link', 'ar', 'gas', 'as'])

# 用于 C 文件的编译命令
env['CCCOM'] = ''

# 用于 C++ 文件的编译命令
env['CXXCOM'] = ''

# 用于创建静态库的命令
env['ARCOM'] = ''

# 用于创建共享库的链接命令
env['SHLINKCOM'] = ''

# 用于生成链接程序命令

env['LINKCOM'] = '$LINK -o $TARGET $SOURCES $LINKFLAGS $__RPATH  $_LIBDIRFLAGS '