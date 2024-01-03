#ifndef _PROJECT__H_
#define _PROJECT__H_

/**
 * @brief Definition of a weapon type (name and damage)
 */
typedef struct {
    char *name;      // name of the weapon
    int damage;         // damage points
} Weapon;

/**
 * @brief Definition of a character
 */
typedef struct {
    char *name;      // name of the character
    int hp;             // health level
    int xp;             // experience level
    Weapon equipment;   // weapon
} Character;

/**
 * @brief Definition of a Game, which stores the added characters
 * 
 */
typedef struct {
    Character *characters;   // dynamic array of characters in the game
    int amount;              // amount of characters in the game
} Game;


// functions in project.c
int add_character(Game *game, char *A);
int attack(Game *game, char *H);
void print_game(Game *game);
int save_to_file(Game *game, char *W);
int load_from_file(Game *game, char *O);
int add_character_from_file(Game *game, char *line, int count);


#endif //! _PROJECT__H_