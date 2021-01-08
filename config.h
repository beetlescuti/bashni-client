#define CONFIGLEN 128

#ifndef config
#define config

typedef struct {
    char game[CONFIGLEN];
    char portnumber[CONFIGLEN];
    char hostname[CONFIGLEN]; 
} configuration;

configuration read_conf_file(char filename[]);

#endif
