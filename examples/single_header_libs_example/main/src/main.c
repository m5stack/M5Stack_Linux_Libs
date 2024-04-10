#define INI_IMPLEMENTATION
#include "ini.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
  ini_t *ini = ini_create(NULL);
  ini_property_add(ini, INI_GLOBAL_SECTION, "FirstSetting", 0, "Test", 0);
  ini_property_add(ini, INI_GLOBAL_SECTION, "SecondSetting", 0, "2", 0);
  int section = ini_section_add(ini, "MySection", 0);
  ini_property_add(ini, section, "ThirdSetting", 0, "Three", 0);

  int size = ini_save(ini, NULL, 0); // Find the size needed
  char *data = (char *)malloc(size);
  size = ini_save(ini, data, size); // Actually save the file
  ini_destroy(ini);

  FILE *fp = fopen("test.ini", "w");
  fwrite(data, 1, size - 1, fp);
  fclose(fp);
  free(data);

  return 0;
}