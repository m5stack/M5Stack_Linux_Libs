#include <stdio.h>
#include <dlfcn.h>
int main()
{
    int m,n;
    // Open the library file.
    void* handler = dlopen("./lib/libcomponent1.so",RTLD_LAZY);
    if(dlerror() != NULL){
        printf("%s",dlerror());
    }
   
    // Retrieve the add() function from the library file.
    int(*add)(int,int)=dlsym(handler,"add");
    if(dlerror()!=NULL){
        printf("%s",dlerror());
    }
  
    // Retrieve the sub() function from the library file.
    int(*sub)(int,int)=dlsym(handler,"sub");
    if(dlerror()!=NULL){
        printf("%s",dlerror());
    }

    // Retrieve the div() function from the library file.
    int(*div)(int,int)=dlsym(handler,"div");
    if(dlerror()!=NULL){
        printf("%s",dlerror());
    }
    // Implement the relevant functionality using the functions from the library file.
    printf("Input two numbers: ");
    scanf("%d %d", &m, &n);
    printf("%d+%d=%d\n", m, n, add(m, n));
    printf("%d-%d=%d\n", m, n, sub(m, n));
    printf("%d÷%d=%d\n", m, n, div(m, n));
    // Close the library file.
    dlclose(handler);
    return 0;
}