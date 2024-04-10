#include <stdio.h>
#include "ini.h"

static void learn_classifier_param_w(int feature_length, int input_w, int input_h, int class_num, int sample_num)
{
  FILE *fp = NULL;
  fp = fopen("./learn.ini", "w");
  if (NULL == fp)
  {
    printf("save learn.ini err! please chcek file system!\n");
    return;
  }
  fprintf(fp, "[base]\n");
  fprintf(fp, "feature_length = %d\n", feature_length);
  fprintf(fp, "input_w = %d\n", input_w);
  fprintf(fp, "input_h = %d\n", input_h);
  fprintf(fp, "class_num = %d\n", class_num);
  fprintf(fp, "sample_num = %d\n", sample_num);
  fclose(fp);
}

int main(int argc, char *argv[])
{
  ini_t *config = ini_load("config.ini");
  if(NULL == config)
  {
    printf("file open faile!\n");
    exit(1);
  }
  const char *name = ini_get(config, "owner", "name");
  if (name)
  {
    printf("name: %s\n", name);
  }

  const char *server = "default";
  int port = 80;

  ini_sget(config, "database", "server", NULL, &server);
  ini_sget(config, "database", "port", "%d", &port);

  printf("server: %s:%d\n", server, port);
  ini_free(config);
  learn_classifier_param_w(512, 224, 224, 5, 15);

  ini_t *config_s = ini_load("./learn.ini");

  const char *feature_length = ini_get(config_s, "base", "feature_length");
  if (feature_length)
  {
    printf("feature_length: %s\n", feature_length);
  }

  const char *sample_num = ini_get(config_s, "base", "sample_num");
  if (sample_num)
  {
    printf("sample_num: %s\n", sample_num);
  }
  ini_free(config_s);

  return 0;
}