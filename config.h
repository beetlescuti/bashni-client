#ifndef config
#define config

typedef struct {
    char game[128];
    char portnumber[128];
    char hostname[128]; 
} configuration;

configuration read_conf_file();

#endif
