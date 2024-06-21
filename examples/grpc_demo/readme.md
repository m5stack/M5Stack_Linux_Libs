# grpc_demo
This is a compilation demo for gRPC. Due to the complexity of cross-compiling with gRPC, we directly use the precompiled static library here.  
We used the source code to compile the static library: https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/linux/linux_static_packages/grpc-1.65.0-pre1-src.zip.  

Please note to use the corresponding version of protoc. After compiling grpc-1.65.0-pre1-src, the corresponding protoc tool will be generated. You can directly use this tool for compilation.    

```bash
protoc --cpp_out=. helloworld.proto  
protoc --grpc_out=. --plugin=protoc-gen-grpc=/usr/bin/grpc_cpp_plugin helloworld.proto
```