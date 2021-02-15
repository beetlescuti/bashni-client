#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

/* reads in a .conf file and populates the configuration struct for use in other places */
configuration read_conf_file(char filename[]) {

    /* read in a config file */
    FILE * config_file = fopen(filename, "r");

    /* initialize a configuration structure */
    configuration game_config;

    /* initialize variables for reading the config file */
    char name[128];
    char val[128];

    /* loop through lines of the config file and save
       variables into the correct struct parameter */
    while(fscanf(config_file, "%s = %s", name, val) == 2) {
        if (strcmp(name, "hostname") == 0) {
            strcpy(game_config.hostname, val);
        }
        else if (strcmp(name, "portnumber") == 0) {
            strcpy(game_config.portnumber, val);
        }
        else if (strcmp(name, "game") == 0) {
            strcpy(game_config.game, val);
        }
    }

    return game_config;
}