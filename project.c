#include "project.h"
#include "stdbool.h" 
#include "stdio.h"     
#include "string.h"    
#include "stdlib.h"    

/* Main function sets the main operation loop of the game*/
int main(void) {
    
    // Description of the game

    printf("Welcome to the game Fighters!\nIn the game you are able to create your characters and attack opponents.\n");
    printf("This game works by user commands. Described as follows:\n\n");
    printf("    A - add a character.                    Input format: A <name> <hit-points> <weapon-name> <weapon-damage>\n");
    printf("    H - attack.                             Input format: H <attacker-name> <target-name>\n");
    printf("    L - print the game.                     Input format: L\n");
    printf("    W - save game to file.                  Input format: W <filename>\n");
    printf("    O - load game from file.                Input format: O <filename>\n");
    printf("    Q - quit program.                       Input format: Q\n\n");

    // takes in user input and calls a function accordingly
    char input[1000];
    char command;
    int result;

    bool ongoing = true;

    // create the game
    Game game;
    memset(&game, 0, sizeof(game));
    game.characters = malloc(sizeof(Character));
    game.amount = 0;

    // get input & execute it accordingly
    while (ongoing) {
        
        fgets(input, 1000, stdin);
        command = input[0];
        input[strcspn(input, "\n")] = 0;

        switch(command) {
            case 'A':
                // call add_character function
                result = add_character(&game, input);

                if (result == 0) {
                    printf("SUCCESS\n");
                    game.amount += 1;
                } else if (result == -1) {
                    printf("ERROR: This character name already exists.\n");
                } else {
                    printf("Invalid command: too many/few arguments in input.\n");
                }

                break;

            case 'H':
                // call attack function
                result = attack(&game, input);

                if (result == 0) {
                    printf("SUCCESS\n");
                } else if (result == -1) {
                    printf("ERROR: Attacker/target is not in the game.\n");
                } else if (result == -2) {
                    printf("Invalid command: too many/few arguments in input.\n");
                } else {
                    printf("Try another attack!\n");
                }

                break;

            case 'L':
                // call print_game function
                if (strlen(input) == 1) {
                    print_game(&game);
                    printf("SUCCESS\n");
                } else {
                    printf("Invalid command: too many/few arguments in input.\n");
                }
            
                break;

            case 'W':
                // call save_to_file function
                result = save_to_file(&game, input);

                if (result == 0) {
                    printf("SUCCESS\n");
                } else if (result == -1) {
                    printf("ERROR: could not open file.\n");
                } else {
                    printf("Invalid command: too many/few arguments in input.\n");
                }

                break;

            case 'O':
                // call load_from_file function
                result = load_from_file(&game, input);

                if (result == -1) {
                    printf("SUCCESS\n"); 
                } else if (result == -1) {
                    printf("ERROR: could not load game from file.\n");
                } else if (result == -2) {
                    printf("Invalid command: too many/few arguments in input.\n");
                } else {
                    printf("ERROR: could not add one of the characters from the file.\n");
                }

                break;

            case 'Q':
                // free characters
                for (int i = 0; i < game.amount; i++) {
                    game.characters[i].equipment.name = NULL;
                    game.characters[i].name = NULL;
                }
                free(game.characters);

                // end loop/game
                ongoing = false;
                break;

            default:
                printf("Invalid command: %s\n", input);
        }
    }

    return 0;
}


/**
 * @brief Adds a character to the game. 
 * The function gets the character information from the paramenter.
 * 
 * 1) checks whether the input is correct
 * 2) allocates memory for the character in the game.characters array
 * 3) creates character and assigns each characteristic
 * 
 * @param game - game being played.
 * @param A - user input. Format: A <name> <hit-points> <weapon-name> <weapon-damage>
 * 
 * @return int 0 if it succeeds
 * @return int -1 if it fails due to input
 * @return int -2 if it fails due to repeated character name
 */
int add_character(Game *game, char *A) {

    //check if input is correct
    int spaces = 0;
    int len = strlen(A);
    for(int a = 0; a < len; a++) {
        if (A[a] == ' ') {
            spaces++;
        }
    }
    if (spaces != 4) return -2;

    // allocate space for the character
    Character *fixed = realloc(game->characters, sizeof(Character)*(game->amount+1));
    game->characters = fixed;
    Character *new_character = fixed + game->amount;

    // add characteristics according to input A
    char *token = strtok(A, " ");
    
    int i = 0;

    while (token) {
        switch (i) {
            case 1:
                // check if name already exists
                if (game->amount != 0) {
                    for (int j = 0; j < game->amount; j++) {
                        if (strcmp(game->characters[j].name, token) == 0) {
                            return -1;
                        }
                    }
                }

                // set the name of the character
                new_character->name = malloc(strlen(token)+1);
                strcpy(new_character->name, token);

                break;
            
            case 2:
                // set the hp & xp of the character
                new_character->hp = atoi(token);
                new_character->xp = 0;

                break;
            
            case 3:
                // set equipment name
                new_character->equipment.name = malloc(strlen(token)+1);
                strcpy(new_character->equipment.name, token);

                break;

            case 4:
                // set weapon damage
                new_character->equipment.damage = atoi(token);

                break;
        }
        i++;
        token = strtok(NULL, " ");
    }

    return 0;
}


/**
 * @brief Attack another character.
 * The function executes an attack.
 * 
 * 1) checks whether the input is correct
 * 2) adds experience points to the attacker
 * 3) reduces health points from the victim
 * 4) prints an explanation of what happened
 * 
 * @param H - user input. Format: H <attacker-name> <target-name>
 * 
 * @return int 0 if it succeeds
 * @return int -1 if it fails due to one of the characters not being on the game
 * @return int -2 if it fails due to wrong input format
 * @return int 1 if it fails due to impossible attack
 */
int attack(Game *game, char *H) {

    //check if input is correct
    int spaces = 0;
    int len = strlen(H);
    for(int x = 0; x < len; x++) {
        if (H[x] == ' ') {
            spaces++;
        }
    }

    if (spaces != 2) return -2;

    // find the attacker and target from input H
    char *token = strtok(H, " ");
    int i = 0;
    int a = 0;
    int b = 0;

    int existing_characters = 0;
    int amount = 0;

    while (token) {
        switch (i) {
            case 1:
                //getting the key to the attacker & adding xp
                for ( ; game->characters != NULL; a++) {
                    if (strcmp(game->characters[a].name, token) == 0) { 
                        amount = game->characters[a].equipment.damage;                      
                        existing_characters++;
                        break;
                    }
                }
                break;
            
            case 2:
                //getting the key to the target & reducing hp
                for ( ; game->characters != NULL; b++) {
                    if (strcmp(game->characters[b].name, token) == 0) {
                        existing_characters++;
                        break;
                    }
                }
                break;
        }
        i++;
        token = strtok(NULL, " ");
    }

    if (strcmp(game->characters[a].name, game->characters[b].name) == 0){
        printf("%s cannot hit themselves.\n", game->characters[a].name);
        return 1;
    } 
    if ((game->characters[b].hp - amount) < 0) {
        printf("%s cannot have hp smaller than zero. You must attack them with a weapon no stronger than %d damage points.\n", game->characters[b].name, game->characters[b].equipment.damage);
        return 1;
    } 
    
    game->characters[a].xp = game->characters[a].xp + amount;
    game->characters[b].hp = game->characters[b].hp - amount;

    if (existing_characters != 2) return -1;

    // printing attack description
    printf("%s swinged at %s and hit them in the head! %s's weapon was a %s with power damage %d.\n", game->characters[a].name, game->characters[b].name, game->characters[a].name, game->characters[a].equipment.name, amount);
    printf("%s now has %d xp.\n", game->characters[a].name, game->characters[a].xp);
    printf("%s has %d hp left.\n", game->characters[b].name, game->characters[b].hp);

    return 0;
}


/**
 * @brief Display the game content.
 * The function prints the characters' informations.
 * 
 * 1) reorganize list of characters by decreasing experience level
 * 2) printing format: <name> <hit-points> <experience> <weapon-name> <weapon-damage>
 * 
 */
void print_game(Game *game) {
    int i = 0;
    int j = 0;
    Character *temp = malloc(sizeof(Character));

    for ( ; i < game->amount; i++) {
        for (j = i + 1; j < game->amount; j++) {
            if (game->characters[i].xp < game->characters[j].xp) {
                *temp = game->characters[i];
                game->characters[i] = game->characters[j];
                game->characters[j] = *temp;
            }
        }
    }

    free(temp);

    for (int x = 0; x < game->amount; x++) {
        printf("%s %d %d %s %d\n", game->characters[x].name, game->characters[x].hp, game->characters[x].xp, game->characters[x].equipment.name, game->characters[x].equipment.damage);
    }
}


int save_to_file(Game *game, char *W) {
    //check if input is correct
    int spaces = 0;
    int len = strlen(W);
    for(int x = 0; x < len; x++) {
        if (W[x] == ' ') {
            spaces++;
        }
    }

    if (spaces != 1) return -2;

    //open file
    W += 2;     // getting only the file name

    FILE *file = fopen(W, "wb");
    if(!file) return -1;

    for (int x = 0; x < game->amount ; x++) {
        fprintf(file, "%s %d %d %s %d\n", game->characters[x].name, game->characters[x].hp, game->characters[x].xp, game->characters[x].equipment.name, game->characters[x].equipment.damage);
    }

    fclose(file);
    return 0;
}

/**
 * @brief This function reads characters from a file and adds them to the game (substituing existing characters)
 * 
 * @param game - the game being played
 * @param O - user input in format: O <filename>
 * 
 * @return int 0 indicating success
 * @return int -1 if it fails due to an error when opening the file
 * @return int -2 if it fails due to incorrect input format
 * @return int -3 if it fails to create and add one of the characters due to repeated names
 */
int load_from_file(Game *game, char *O) {
    //check if input is correct
    int spaces = 0;
    int len = strlen(O);
    for(int x = 0; x < len; x++) {
        if (O[x] == ' ') {
            spaces++;
        }
    }

    if (spaces != 1) return -2;

    //open file
    O += 2;     // getting only the file name

    FILE *file = fopen(O, "r");
    if(!file) return -1;

    // free existing characters
    for (int i = 0; i < game->amount; i++) {
        free(game->characters[i].equipment.name);
        free(game->characters[i].name);
    }
    free(game->characters);

    //get number of lines
    int lines = 0;
    char ch;
    while (!feof(file)) {
        ch = fgetc(file);
        if (ch == '\n') {
            lines++;
        }
    }

    // go back the beginning of the file
    fseek(file, 0, SEEK_SET);

    int done = 0;
    int success = 0;
    char *line = malloc(1000*sizeof(char));
    
    for (int i = 0; i < lines; i++) {
        success = add_character_from_file(game, fgets(line, 1000, file), done);
        if (success != 0) return -3;

        done++;
    }
    free(line);

    game->amount = done;    // fix the amount of characters in the game

    return 0;
}

/**
 * @brief Extra function to add the characters written from a file to the game.
 * This function is required because the original add_characters function does not take into account 
 * different values of XP. This function:
 * 
 * 1) allocates memory in the game.characters array to add the new character
 * 2) create the new character with the correct attribute characters
 * 
 * @param game - the game which is being played
 * @param line - the line of the file that must be read
 * @param count - the amount of characters on the current array
 * 
 * @return int 0 to indicate successful completion
 *         int -1 if a character with the same name is being added (so the function failes)
 */
int add_character_from_file(Game *game, char *line, int count) {
    // allocate space for the character
    Character *fixed = realloc(game->characters, sizeof(Character)*(count+1));
    game->characters = fixed;
    Character *new_character = fixed + count;

    // add characteristics according to input A
    char *token = strtok(line, " ");
    
    int i = 1;

    while (token) {
        switch (i) {
            case 1:
                // check if name already exists
                if (count != 0) {
                    for (int j = 0; j < count; j++) {
                        if (strcmp(game->characters[j].name, token) == 0) {
                            return -1;
                        }
                    }
                }

                // set the name of the character
                new_character->name = malloc(strlen(token)+1);
                strcpy(new_character->name, token);

                break;
            
            case 2:
                // set the hp & xp of the character
                new_character->hp = atoi(token);

                break;
            
            case 3:
                // set up the hp of the character
                new_character->xp = atoi(token);

                break;

            case 4:
                // set equipment name
                new_character->equipment.name = malloc(strlen(token)+1);
                strcpy(new_character->equipment.name, token);

                break;

            case 5:
                // set weapon damage
                new_character->equipment.damage = atoi(token);

                break;
        }
        i++;
        token = strtok(NULL, " ");
    }

    return 0;
}
