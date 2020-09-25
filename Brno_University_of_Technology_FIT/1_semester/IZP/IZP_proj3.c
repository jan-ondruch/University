/*
 * File:        proj3.c
 * Date:        2014/12/13
 * Author:      Jan Ondruch, xondru14@stud.fit.vutbr.cz
 * Project:     Maze passage
 * Description: Tests if an entered text file is a valid maze,
                if positive, finds a way through it.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define HELP    1
#define WRONG_ARG   -2
#define INVALID 3
#define WRONG_FILE 4
#define ALLOC_FAIL 5
#define CLOSE_FILE_FAIL 6
#define WRONG_POSITION 7

#define TEST 2
#define L_PATH 0
#define R_PATH 4

#define ARGV_2 2
#define ARGV_4 4

#define MATRIX_NUMBERS 2
#define ASCII_DEC_0 48
#define ASCII_DEC_7 55

#define INCREMENTS 3

// ======= STRUCTURES ======= //

typedef struct
{   // stores function triggers and operations
    int mode;
    int message;
    FILE *fp;
    int openfile;
    int row_ent;    // entered row
    int col_ent;    // entered col
} Toperation;

typedef struct
{   // map for 2D array transformed into 1D array
    int rows;
    int cols;
    unsigned char *cells;
} Map;

// ======= FUNCTION PROTOTYPES ======= //

void print_help (void);
void err_msgs (Toperation *operation);
int test_file (char**argv);
int close_file (Toperation *operation);
int parse_int (char *argv[]);
int parse_args (int argc, char **argv, Toperation *operation);
void alloc_map (Map *map, Toperation *operation);
void dealloc_map (Map *map);
int load_map (Map *map, char **argv, Toperation *operation);
int check_borders (Map *map);
int check_entered_values (Map *map, Toperation *operation);
bool isborder (Map *map, int r, int c, int border);
int start_border (Map *map, int r, int c, int leftright);
int passage (Map *map, Toperation *operation);


// ======= PRINTING MESSAGES ======= //

const char *HELP_MSG =
"\nProgram:            Maze passage\n"
"Author:             Jan Ondruch, xondru14@stud.fit.vutbr.cz\n"
"Usage:\n\n"
"Programm receives a maze on stdin in a form of text table and if possible,\n"
"finds the way out of it starting from a given entering position.\n\n"

"proj3 [options] file\n\n"
"   --help                  Display this information\n\n"
"proj3 --test file.txt\n"
"   --test checks, if the file given by the second argument is a valid maze map.\n"
"   If so, 'Valid' is printed out.\n"
"   If the file contains invalid data such as illegal characters or wrong values, 'Invalid' is printed out.\n"
"proj3 --rpath R C bludiste.txt\n"
"   --rpath finds a way through the maze from the starting point given by row R and column C.\n"
"   The way is found following the right hand rule (Right hand always on the wall).\n"
"   --lpath finds a way through the maze from the starting point given by row R and column C.\n"
"   The way is found following the left hand rule (Left hand always on the wall).\n";

void print_help (void)
{
    printf("%s\n", HELP_MSG);
}

void err_msgs (Toperation *operation)
{
    if (operation->message == WRONG_ARG)
    {
        fprintf(stderr, "Wrong arguments passed\n");
    }
    else if (operation->message == INVALID)
    {
        fprintf(stderr, "Invalid\n");
    }
    else if (operation->message == WRONG_FILE)
    {
        fprintf(stderr, "Wrong file name\n");
    }
    else if (operation->message == ALLOC_FAIL)
    {
        fprintf(stderr, "Memory allocation problem\n");
    }
    else if (operation->message == CLOSE_FILE_FAIL)
    {
        fprintf(stderr, "Unable to close the file\n");
    }
    else if (operation->message == WRONG_POSITION)
    {
        fprintf(stderr, "Wrong starting position entered\n");
    }
}

// ======= ARGUMENT PARSING CONVERSIONS & TESTS ======= //

int test_file (char **argv)
{   // test if .txt file has been parsed
    FILE *fp;
    if (!(fp = fopen(*argv, "r")))
        return 1;
    return 0;
}

int close_file (Toperation *operation)
{   //  closes the opened .txt file
    if (fclose(operation->fp) == EOF)
    {
        return CLOSE_FILE_FAIL;
    }
    return 0;
}

int parse_int (char *argv[])
{   // converts char argument to integer
    char *endptr;
    double x;     // passed argument

    x = strtod(*argv, &endptr);     // converting passed argument to double

    if ((*endptr != '\0') || x < 0)
    {
        return WRONG_ARG;      // failed conversion
    }
    else
    {
        return (int)x;       // returns converted typecasted value
    }
}

// ======= ARGUMENTS PARSING ======= //

int parse_args (int argc, char **argv, Toperation *operation)
{   // parses arguments given by user on stdin
    if (argc >= 2)
    {
        if (argc == 2 && (!strcmp("--help", argv[1])))
        {
            operation->message = HELP;     // help msg is printed out
            return EXIT_SUCCESS;
        }
        else if (argc == 3 && (!strcmp("--test", argv[1])))
        {
            if (test_file(&argv[2]) == 1)
            {   // tests file if valid
                operation->message = WRONG_FILE;
                return EXIT_FAILURE;
            }
            else
            {
                operation->mode = TEST;
                operation->openfile = ARGV_2;
                return EXIT_SUCCESS;
            }
        }
        else if (argc == 5)
        {
            if ((!strcmp("--rpath", argv[1])) || (!strcmp("--lpath", argv[1])))
            {
                operation->row_ent = parse_int(&argv[2]);   // integer conversion
                operation->col_ent = parse_int(&argv[3]);

                if ((operation->row_ent == WRONG_ARG) || (operation->col_ent == WRONG_ARG))
                {
                    operation->message = WRONG_ARG;
                    return EXIT_FAILURE;
                }

                if (test_file(&argv[4]) == 1)
                {
                    operation->message = WRONG_FILE;
                    return EXIT_FAILURE;
                }
                else
                {
                    operation->openfile = ARGV_4;

                    if (!strcmp("--rpath", argv[1]))
                        operation->mode = R_PATH;
                    else
                        operation->mode = L_PATH;

                    return EXIT_SUCCESS;
                }
            }
        }
    }
    operation->message = WRONG_ARG;

    return EXIT_FAILURE;
}

// ======= MATRIX INITIALIZATION & CHECKING VALIDITY ======= //

void alloc_map (Map *map, Toperation *operation)
{   // allocates memory, size of the entered matrix
    if (!(map->cells = malloc (map->rows * map->cols * sizeof(char))))
    {
        operation->message = ALLOC_FAIL;
    }
}

void dealloc_map (Map *map)
{   // deallocates the previously allocated memory
    free(map->cells);
    map->cells = NULL;
}

int load_map (Map *map, char **argv, Toperation *operation)
{   // reads the input file and stores read chars + cols + rows count to the Map structure
    int c;
    int sizes = 0;      // sizes of the matrix
    bool alloc = true;  // trigger for just one allocation
    int status = 0;
    int i = 0;          // index of a matrix element

    operation->fp = fopen(argv[operation->openfile], "r");

    while ((status = (fscanf(operation->fp, "%d", &c))) != EOF)
    {   // ends if a read character is not a single char or reaches EOF
        if (!status)
        {   // wrong character read
            return INVALID;
        }
        if (sizes < MATRIX_NUMBERS)
        {   // first gets the 2 sizes of the matrix
            if (sizes == 0)
            {
                map->rows = c;      // matrix rows size
                sizes++;
            }
            else
            {
                map->cols = c;      // matrix cols size
                sizes++;
            }
        }
        else
        {
            if (alloc == true)
            {   // then allocates space using their values
                alloc_map(map, operation);
                alloc = false;
            }
            char ch = c + ASCII_DEC_0;
            if (ch < ASCII_DEC_0 || ch > ASCII_DEC_7)
            {
                return INVALID;
            }

            map->cells[i] = ch; // stores chars (numbers 0 - 7) into the cells matrix
            i++;
        }
    }

    if (i != map->rows * map->cols)
    {   // size of matrix does not match number of the chars
        return INVALID;
    }
    return EXIT_SUCCESS;
}

int check_borders (Map *map)
{   // checks if borders of the matrix are valid
    int c, d, e;
    bool even_row = false;      // even and odd row trigger
    bool comparison = true;
    for (int i = 0; i < (map->rows*map->cols - 1); i++)
    {
        c = map->cells[i];
        d = map->cells[i + 1];
        e = map->cells[i + map->cols];

        c = c - ASCII_DEC_0;
        d = d - ASCII_DEC_0;
        e = e - ASCII_DEC_0;

        if (i == ((map->rows*map->cols) - map->cols))
        {
            comparison = false;
        }

        /* binary comparision checking bases of 2 triangles 
         * above each other and sides of neighbouring triangles
         */

        if (((map->cols % 2) == 0) && (even_row == true))
        {
            if ((i % 2 == 0) && (comparison == true))
            {
                if (((c < 4) && (e < 4)) || ((c > 3) && (e > 3)))
                    ;
                else
                    return INVALID;
            }
        }
        else if ((i % 2 == 1) && (comparison == true))
        {
            if (((c < 4) && (e < 4)) || ((c > 3) && (e > 3)))
                ;
            else
                return INVALID;
        }
        if (((c == 0 || c == 1 || c == 4 || c == 5) && (d == 0 || d == 2 || d == 4 || d == 6))
            || ((c == 2 || c == 3 || c == 6 || c == 7) && (d == 1 || d == 3 || d == 5 || d == 7)))
            ;
        else
        {
            if (((i + 1) % (map->cols)) == 0)
            {
                even_row = !even_row;
                continue;
            }
            else
                return INVALID;
        }
        if (((i + 1) % (map->cols)) == 0)
        {
            even_row = !even_row;
        }
    }
    return EXIT_SUCCESS;
}

// ======= FUNCTIONS FOR PASSING THE MATRIX ======= //

enum direction { BASE = 0, RIGHT, LEFT};

struct increment
{   // increment structure - inc. row, inc. col (delta row/col)
    int dr;
    int dc;
};

struct increment increments[INCREMENTS] =
{   // increments based on a chosen direction
    // BASE
    {-1, 0},
    // RIGHT
    {0, 1},
    // LEFT
    {0, -1},
};

bool isborder (Map *map, int r, int c, int border)
{   // returns true if the entered border is solid (is a wall)
    bool result = true;
    char ch;    // current number

    ch = map->cells[r * map->cols + c];

    if ((r % 2 == 1 && c % 2 == 1) || (r % 2 == 0 && c % 2 == 0))
    {   // base is up, must switch left and right border 
        if (border == 1)
            border = 2;
        else if (border == 2)
            border = 1;
    }

    /* binary comparision checking if current border is a wall or free */

    if (border == 2 && ((ch & 1) == 1)) return result;
    else if ((border == 1) && ((ch & 2) == 2)) return result;
    else if (border == 0 && ((ch & 4) == 4)) return result;

    result = false;
    return result;
}

int start_border (Map *map, int r, int c, int leftright)
{   // checks walls according to rpath/lpath, returns wall, which the path will follow
    int direction;
    if (leftright <= L_PATH)
    {
        direction = L_PATH - leftright;
        leftright = leftright + direction;
    }
    else
    {
        direction = R_PATH - leftright;
        leftright = leftright + direction;
    }

    bool isfree = true;

    if (leftright == R_PATH)
        direction++;    // rotates right
    else
        direction--;    // rotates left

    if (direction > LEFT)   // avoid rotation out of range
        direction = BASE;
    else if (direction < BASE)  // avoid rotation out of range
        direction = LEFT;

    while (isfree == 1)
    {   // rotate and check borders until free border is found
        if ((isfree = isborder(map, r, c, direction)) == 1)
        {
            if (leftright == R_PATH)
                direction++;
            else
                direction--;
        }
        if (direction > LEFT)
            direction = BASE;
        else if (direction < BASE)
            direction = LEFT;
    }
    return direction;
}

int check_entered_values (Map *map, Toperation *operation)
{   // checks if the entered coordinates are valid 
    char c; // entered starting position
    int i;  // its index

    i = ((operation->row_ent - 1) * map->cols) + (operation->col_ent - 1);
    c = map->cells[i];

    /* check if the entered coordinates are edges (4) */

    if ((operation->row_ent == 1 && operation->col_ent == 1) || 
        (operation->row_ent == map->rows && operation->col_ent == 1))
    {
        if (((c & 1) == 1) && (c & 4) == 4) return EXIT_FAILURE;
    }
    else if ((operation->row_ent == 1 && operation->col_ent == map->cols) || 
        (operation->row_ent == map->rows && operation->col_ent == map->cols))
    {
        if (((c & 2) == 2) && (c & 4) == 4) return EXIT_FAILURE; 
    }

    /* check if the entered coordinates are borders */

    else if  ((((operation->row_ent < 1) || (operation->row_ent > map->rows)) ||
        ((operation->col_ent < 1) || (operation->col_ent > map->cols))) ||
        (((operation->row_ent != 1) && (operation->row_ent != map->rows)) &&
        ((operation->col_ent != 1) && (operation->col_ent != map->cols))) || 
        ((operation->col_ent % 2 == 0) && (operation->row_ent == 1 || operation->row_ent == map->rows)))
    {   // entered position is not a border
        return EXIT_FAILURE;
    }
    else if (operation->col_ent == 1)
    {
        // left border exists - no possible entrance
        if ((c & 1) == 1) return EXIT_FAILURE;
    }
    else if (operation->col_ent == map->cols)
    {
        // right border exists - no possible entrance
        if ((c & 2) == 2) return EXIT_FAILURE;
    }
    else if (operation->row_ent == 1 || operation->row_ent == map->rows)
    {
        // up/down border exist - no possible entrance
        if ((c & 4) == 4) return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int passage (Map *map, Toperation *operation)
{   // moves trough the matrix
    int cr;  // current row
    int cc;  // current column

    int leftright;  // rpath or lpath are assigned to it 
    int direction;  // direction number used for rotation

    if (check_entered_values(map, operation))
    {   // exit if entered values are invalid
        operation->message = WRONG_POSITION;
        return EXIT_FAILURE;
    }

    cr = operation->row_ent - 1; // starting position
    cc = operation->col_ent - 1; // starting position

    leftright = operation->mode;    // left/right hand rule is followed

    /* leftright starting borders */

    if ((cc == 0 && cr % 2 == 1) || ((cc == map->cols - 1) && (cr % 2 == 1)))
    {
        if (leftright == R_PATH)
            direction = BASE;
        else
            direction = LEFT;
    }
    else if ((cc == 0 && cr % 2 == 0) || (cr == map->rows - 1))
    {
        if (leftright == R_PATH)
            direction = RIGHT;
        else
            direction = BASE;
    }
    else if ((cr == 0) || ((cc == map->cols - 1) && (cr % 2 == 0)))
    {
        if (leftright == R_PATH)
            direction = LEFT;
        else
            direction = BASE;
    }

    leftright = (leftright - direction);
    printf("%d,%d\n", cr + 1, cc + 1);

    while (1)
    {   // calculate coordinates until the right end of the maze is reached
        direction = start_border(map, cr, cc, leftright);

        if ((cr % 2 == 1 && cc % 2 == 0) || (cr % 2 == 0 && cc % 2 == 1))
        {   // base is down
            cr = cr - increments[direction].dr;
            cc = cc + increments[direction].dc;
        }
        else
        {   // base is up, has to switch directions (borders) again
            if (direction == 1)
                direction = 2;
            else if (direction == 2)
                direction = 1;

            cr = cr + increments[direction].dr;
            cc = cc + increments[direction].dc;

            if (direction == 1)
                direction = 2;
            else if (direction == 2)
                direction = 1;
        }

        leftright = operation->mode;
        leftright = (leftright - direction);

        if (!((cr < 0 || cc < 0) || (cr > map->rows - 1 || cc > map->cols - 1)))
            printf("%d,%d\n", cr + 1, cc + 1);  // prints the coordinates
        else
            return EXIT_SUCCESS;
    }
}

int main (int argc, char **argv)
{   // main function
    Toperation operation;
    Map map;

    if (!parse_args(argc, argv, &operation))
    {   // wrong arguments passed
        if (operation.message == HELP)
            print_help();
        else if ((operation.mode == TEST) || (operation.mode == R_PATH) || (operation.mode == L_PATH))
        {   // choose between the 3 possible modes
            if ((operation.message = load_map(&map, argv, &operation)))
            {   // invalid map entered
                err_msgs(&operation);
                close_file(&operation);
            }
            else
            {
                if ((operation.message = check_borders(&map)))
                {   // invalid map (borders)
                    err_msgs(&operation);
                    close_file(&operation);
                }
                else
                {
                    if (operation.mode == TEST)
                        printf("Valid\n");
                    else
                    {    
                        if (passage(&map, &operation))
                            err_msgs(&operation);
                    }
                    dealloc_map(&map);
                    close_file(&operation);
                }
            }
        }
    }
    else
        err_msgs(&operation);

    return EXIT_SUCCESS;
}

// ======= END OF THE PROGRAM ======= //