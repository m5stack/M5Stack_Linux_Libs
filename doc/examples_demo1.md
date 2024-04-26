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
Taking the 'mongoose_tcp_echo' TCP echo server example from 'examples/mongoose_tcp_echo' as an example, create a new project by copying the 'examples/demo1' folder.

Mongoose is a lightweight TCP library that can be easily used in devices like ESP32, STM32, RP2040, and Linux, enabling convenient development of TCP projects.

```bash
# Enter the 'examples' folder.
cd examples

# Copy the 'demo1' folder to 'mongoose_tcp_echo'.
cp demo1 mongoose_tcp_echo -r

# Enter the 'mongoose_tcp_echo' directory.
cd mongoose_tcp_echo

# Edit 'main/src/main.c' and input the following content.


############################### mongoose_tcp_echo/main/src/main.c ###############################
#include "mongoose.h"

struct mg_mgr mgr;

void printf_connect(struct mg_connection *c)
{
  if (!c->rem.is_ip6)
  {
    printf("ip:");
    printf("%d", c->rem.ip[0]);
    printf(".");
    printf("%d", c->rem.ip[1]);
    printf(".");
    printf("%d", c->rem.ip[2]);
    printf(".");
    printf("%d", c->rem.ip[3]);
    printf("  port:%d \n", c->rem.port);
  }
}
static void cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
  switch (ev)
  {
  case MG_EV_OPEN:
    break;
  case MG_EV_ACCEPT:
  {
    printf("SERVER accepted a connection  ");
    printf_connect(c);
  }
  break;
  case MG_EV_READ:
  {
    mg_send(c, c->recv.buf, c->recv.len); // Echo received data back
    printf_connect(c);
    printf("say:%s\n", c->recv.buf);
    mg_iobuf_del(&c->recv, 0, c->recv.len); // And discard it
  }
  break;
  case MG_EV_CLOSE:
    break;
  case MG_EV_ERROR:
    break;
  default:
    break;
  }
}
int main(int argc, char *argv[])
{
  mg_mgr_init(&mgr);                               // Init manager
  mg_listen(&mgr, "tcp://0.0.0.0:1234", cb, &mgr); // Setup listener
  for (;;)
  {
    mg_mgr_poll(&mgr, 1000); // Event loop
  }
  mg_mgr_free(&mgr); // Cleanup
  return 0;
}
#################################################################################################

```

Execute in the 'mongoose_tcp_echo' directory.
```bash
scons menuconfig
```
Select 'Components configuration' to enter.

![](../assets/image/1111111.png)

Then select 'Enable mongoose

![](../assets/image/222222.png)

Press 'q' to exit.

![](../assets/image/333333.png)

Press 'y' to save.

Then execute the compilation.
``` bash
scons
```

After successful compilation, the 'mongoose_tcp_echo' executable file will be generated in the 'examples/mongoose_tcp_echo/dist' folder.

Run the 'mongoose_tcp_echo' program.
```bash
./dist/mongoose_tcp_echo
```
Use 'nc' to test the program's execution.

```bash
# Connect to the TCP server.
nc 127.0.0.1 1234

# Input any message and observe the returned information.
```

![](../assets/image/444444.png)


![](../assets/image/555555.png)


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



