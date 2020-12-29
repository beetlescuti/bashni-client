//
// Created by svea on 29.12.20.
//

#ifndef BASCHNI_CLIENT_STRUCTDEFINITIONS_H
#define BASCHNI_CLIENT_STRUCTDEFINITIONS_H

typedef struct game_info {

    int player_id;
    int number_of_players;
    pid_t thinker_id;
    pid_t connector_id;
    char player_name[];  // Frage: am Ende geht es, aber sonst nicht?
} GAME_INFO;

typedef struct player{
    int player_id;
    bool ready;
    char player_name[];

} PLAYER;


#endif //BASCHNI_CLIENT_STRUCTDEFINITIONS_H
