# create a new project

The 'examples/demo1' folder is a 'Hello, World!' project that can be used as a template for quickly creating your own project by duplicating this folder.

## File tree
``` bash
demo1
├── SConstruct
├── config_defaults.mk
├── main
│   ├── include
│   │   └── main.h
│   ├── Kconfig
|   ├── SConstruct
│   └── src
│       └── main.c
```


- **main** Main code directory
- **main/SConstruct** CMake files for the main code, requiring manual addition of dependency folders.
- **config_defaults.mk** Default configuration files, storing default component configurations.
- **SConstruct** Project's startup entry point, no need for modifications.


## Creating a project example

```bash
# Enter the 'examples' folder.
cd examples

# Copy the 'hello_world' folder to 'demo1'.
cp hello_world demo1 -r

# Enter the 'demo1' directory.
cd demo1

# Edit 'main/src/main.c' and input the following content.


############################### demo1/main/src/main.c ###############################
#include <stdio.h>

int main(int argc,char *argv[])
{
    printf("hello world!\r\n");
    
    return 0;
} 

#################################################################################################

```

Execute in the 'demo1' directory.


Then execute the compilation.
``` bash
scons
```

After successful compilation, the 'demo1' executable file will be generated in the 'examples/demo1/dist' folder.

Run the 'demo1' program.
```bash
./dist/demo1
```

## Cross-compilation
``` bash
# Complete clean
scons distclean

# Set the cross-compiler according
scons menuconfig

# Compile
scons

# Push files according to the Makefile preset
scons push

# Simply enter the development environment and execute

```



