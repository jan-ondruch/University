/*
 * File:        proj2.c
 * Date:        2014/11/24
 * Author:      Jan Ondruch, xondru14@stud.fit.vutbr.cz
 * Project:     Iteration calculations of a tangens angle and measuring distances.
 * Description: Compares angles from different implementations or
                measures distances of an object using received data.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define COEFSIZE 13         // array size

#define WRONG_ARG -2        // wrong argument passed
#define HELP_MESSAGE    2
#define NUM_TO_FUNC 3
#define NUM_TO_DIST 4

#define ARGV_ONE 1
#define ARGV_TWO 2
#define ARGV_THREE 3
#define ARGV_FOUR 4
#define ARGV_FIVE 5
#define ARGV_SIX 6
#define ARGV_SEVEN 7

#define MAX_ITERATIONS  13
#define MIN_ITERATIONS  0

#define MAX_HEIGHT 100
#define MIN_HEIGHT 0

#define MIN_ANGLE   0.0
#define MAX_ANGLE   1.4

#define DEFAULT_HEIGHT 1.5
#define SUFFICENT_ITERATIONS 9  // iterations needed for a distance & height measurement

// ======= FUNCTIONS PROTOTYPES ======= //
double taylor_tan(double x, unsigned int n);
double cfrac_tan(double x, unsigned int n);
double parse_double(char *argv[]);
double parse_angle(char * argv[]);
double abs_value(double tan_lib, double tan_calc);
void cal_values(double angle, double N, double M);
int numbers_to_functions(char * argv[]);
void measure_distances(double measure, double alpha_angle, double beta_angle);
int numbers_to_distances(int argc, char * argv[]);
int parse_arg(int argc, char *argv[]);

// ======= MESSAGES ======= //
const char *WRONG_ARGUMENT = "Wrong arguments passed";
const char *HELP_MSG =
"\nProgram:            Iteration calculations\n"
"Author:             Jan Ondruch, xondru14@stud.fit.vutbr.cz\n"
"Usage:\n\n"
"Programm calculates and compares angles derived and calculated from different mathmematical methods,\n"
"or calculates distance (optionally height) of an object using given arguments.\n\n"

"proj2 [options] file\n\n"
"   --help                  Display this information\n\n"
"proj2 --tan A N M\n"
"   --tan compares accuracy of the tangens angle A computations (in radians) between\n"
"   a math library tangens function, a Taylor series and a continued fraction computations.\n"
"   Arguments N and M mark, in how many iterations should the comparision undertake.\n"
"   0 < N <= M < 14\n\n"
"proj2 [-c X] -m A [B]\n"
"   [] mark optional arguments\n"
"   -m computates and measures the distances\n\n"
"   The α angle is given as the A argument in radians. Program calculates and writes out the distance of the object\n"
"   0 < A <= 1.4 < π/2.\n"
"   The β angle (if given) is given as the B argument in radians. Program calculates and writes out the height of the object, too.\n"
"   0 < B <= 1.4 < π/2.\n"
"   Argument -c adjusts height of the measuring gadget c for the calculations.\n"
"   The height c is given as the argument X, (0 < X <= 100).\n"
"   This argument is optional - implicit height is set to 1.5 meters.\n";

// ======= TANGENS FUNCTIONS ======= //
double taylor_tan(double x, unsigned int n)
{   // x - states an angle, n - number of iterations. Calculates tangesns using a taylor series polynomial
    static double denom[COEFSIZE] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
    static double numer[COEFSIZE] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604};
    int i = 1;    // array position counter
    double t, s;  // t - part, s - result
    double xx = x*x;

    s = t = x;
    n--;          // one iteration back (the first part has already been calculated - one row above)

    while (n != 0)
    {
        t = t * ((xx * (*(numer + i))) / *(denom + i));
        s = s + t;
        t = t * (*(denom + i)) / (*(numer + i));    // deleting coefficient
        n-- ;
        i++;
    }
    return s;
}

double cfrac_tan(double x, unsigned int n)
{   // x - states an angle, n - number of iterations (cont.fractions). Calculates tangesns using a continued fraction method.
    n--;                                  // one iteration back (the first part is alredy calculated before entering iteration cycle)
    double cf;                            // current fraction
    double a = x*x;                       // numerator from the formula
    double b = (2.0 * (double)n) + 1;     // denominator from the formula

    cf = a / b;                           // initializing the first fraction

    for(; n >= 2; n--)
    {
        b = b - 2;
        cf = a / (b - cf);
    }
    return x / ((b - 2) - cf);    // x - first component of the cont.frac. tangens formula
}

// ======= ARGUMENT CONVERSION ======= //
double parse_double(char *argv[])
{   // converts char argument to double
    char *endptr;
    double x;     // passed argument

    x = strtod(*argv, &endptr);     // converting passed argument to double

    if (*endptr != '\0')
    {
        return WRONG_ARG;      // failed conversion
    }
    else
    {
        return x;       // returns converted value
    }
}

double parse_angle(char * argv[])
{   // angle conversion
    double angle;
    angle = parse_double(*&argv);

    if (angle <= MIN_ANGLE || angle > MAX_ANGLE)
    {
        return WRONG_ARG;
    }
    return angle;
}

// ======= COMPARISON FUNCTIONS ======= //
double abs_value(double tan_lib, double tan_calc)
{   // returns absolute value
    if ((tan_lib - tan_calc) < 0)
    {
        return tan_calc - tan_lib;
    }
    return tan_lib - tan_calc;
}

void cal_values(double angle, double N, double M)
{   // prints the calculated values from various functions on the screen in a format I M T TE C CE
    double tan_lib;
    tan_lib = tan(angle);         // tangens from math.h
    int i;

    for (i = N; i <= M; i++)
    {
        printf("%d %e %e %e %e %e\n", i, tan_lib, taylor_tan(angle, i), abs_value(tan_lib, taylor_tan(angle, i)),
               cfrac_tan(angle, i), abs_value(tan_lib, cfrac_tan(angle, i)));
    }
}

int numbers_to_functions(char * argv[])
{   // provides values from the passed arguments to tangens functions
    double alpha_angle;       // alpha angle
    double lower_border;      // lower iteration border
    double upper_border;      // upper iteration border

    lower_border = parse_double(&argv[ARGV_THREE]);
    upper_border = parse_double(&argv[ARGV_FOUR]);

    if ((alpha_angle = parse_angle(&argv[ARGV_TWO])) == WRONG_ARG)
    {
        return WRONG_ARG;
    }

    if ((lower_border <= MIN_ITERATIONS || lower_border > MAX_ITERATIONS) || (upper_border < lower_border || upper_border > MAX_ITERATIONS))
    {
        return WRONG_ARG;
    }

    cal_values (alpha_angle, lower_border, upper_border);
    return EXIT_SUCCESS;
}

// ======= DISTANCES FUNCTIONS ======= //
void measure_distances(double measure, double alpha_angle, double beta_angle)
{   // measure distances: distance from the object + if given beta angle, height of the object, too
    double d;
    double h;

    if (beta_angle == 0)
    {
        d = measure / alpha_angle;
        printf("%.10e\n", d);
    }

    if (beta_angle != 0)
    {
        d = measure / alpha_angle;
        printf("%.10e\n", d);
        h = measure + (beta_angle * d);
        printf("%.10e\n", h);
    }
}

int numbers_to_distances(int argc, char * argv[])
{   // provides values from the passed arguments in order to measure the required distances
    double measure_height = DEFAULT_HEIGHT;   // default height of the measuring gadget
    double alpha_angle;
    double beta_angle = 0;
    int i = SUFFICENT_ITERATIONS;

    if (argc >= 5)
    {
        if ((alpha_angle = parse_angle(&argv[ARGV_FOUR])) == WRONG_ARG)
        {
            return WRONG_ARG;
        }
        alpha_angle = cfrac_tan(alpha_angle, i);
        measure_height = parse_double(&argv[ARGV_TWO]);
        if (measure_height <= MIN_HEIGHT || measure_height > MAX_HEIGHT)
        {
            return WRONG_ARG;
        }
        else if (argc == 6)
        {
            if ((beta_angle = parse_angle(&argv[ARGV_FIVE])) == WRONG_ARG)
            {
                return WRONG_ARG;
            }
            beta_angle = cfrac_tan(beta_angle, i);
        }
    }
    else if (argc < 5)
    {
        if ((alpha_angle = parse_angle(&argv[ARGV_TWO])) == WRONG_ARG)
        {
            return WRONG_ARG;
        }
        alpha_angle = cfrac_tan(alpha_angle, i);
        if (argc == 4)
        {
            if ((beta_angle = parse_angle(&argv[ARGV_THREE])) == WRONG_ARG)
            {
                return WRONG_ARG;
            }
            beta_angle = cfrac_tan(beta_angle, i);
        }
    }

    measure_distances(measure_height, alpha_angle, beta_angle);
    return EXIT_SUCCESS;
}

// ======= ARGUMENTS PARSING ======= //
int parse_arg(int argc, char *argv[])
{   // funciton for parsing arguments given by user
    if (argc < 2)
    {
        return EXIT_FAILURE;                         // no argument, return appropriate message
    }
    else if (strcmp(argv[ARGV_ONE], "--help") == 0)
    {
        if (argc == 2)
        {
            return HELP_MESSAGE;                         // returns HELP message on the screen
        }
    }
    else if (strcmp(argv[ARGV_ONE], "--tan") == 0)
    {
        if (argc != 5)
        {                                             //no arguments passed after "--tan" argument
            return EXIT_FAILURE;                      // returns Warning message on the screen
        }
        else
        {
            if (((parse_double(&argv[ARGV_TWO])) == WRONG_ARG) || ((parse_double(&argv[ARGV_THREE])) == WRONG_ARG) || ((parse_double(&argv[ARGV_FOUR])) == WRONG_ARG))
            {
                return EXIT_FAILURE;
            }
        return NUM_TO_FUNC;
        }
    }
    else if (strcmp(argv[ARGV_ONE], "-c") == 0)
    {
        if (argc != 5 && argc != 6)
        {
            return EXIT_FAILURE;
        }
        else if (strcmp(argv[ARGV_THREE], "-m") == 0)
        {
            if (((parse_double(&argv[ARGV_TWO])) == WRONG_ARG) || ((parse_double(&argv[ARGV_FOUR])) == WRONG_ARG))
            {
                return EXIT_FAILURE;
            }
            if (argc == 6)
            {
                if (parse_double(&argv[ARGV_FIVE]) == WRONG_ARG)
                {
                    return EXIT_FAILURE;
                }
                return NUM_TO_DIST;
            }
        return NUM_TO_DIST;
        }
    }
    else if (strcmp(argv[ARGV_ONE], "-m") == 0)
    {
        if ((argc != 3) && (argc != 4))
        {
            return EXIT_FAILURE;
        }
        else if (parse_double(&argv[ARGV_TWO]) == WRONG_ARG)
        {
            return EXIT_FAILURE;
        }
        if (argc == 4)
        {
            if (parse_double(&argv[ARGV_THREE]) == WRONG_ARG)
            {
                return EXIT_FAILURE;
            }
            return NUM_TO_DIST;
        }
        return NUM_TO_DIST;
    }
    return EXIT_FAILURE;
}

// ======= MAIN FUNCTION ======= //
int main(int argc, char *argv[])
{
    if (parse_arg(argc, argv) == NUM_TO_FUNC)
    {
        if ((numbers_to_functions(argv)) == WRONG_ARG)
        {
            fprintf(stderr, "%s\n", WRONG_ARGUMENT);
            return EXIT_SUCCESS;
        }
    }
    else if (parse_arg(argc, argv) == NUM_TO_DIST)
    {
        if ((numbers_to_distances(argc, argv)) == WRONG_ARG)
        {
            fprintf(stderr, "%s\n", WRONG_ARGUMENT);
            return EXIT_SUCCESS;
        }
    }
    else if (parse_arg(argc, argv) == EXIT_FAILURE)
    {
        fprintf(stderr, "%s\n", WRONG_ARGUMENT);
        return EXIT_SUCCESS;
    }
    else if (parse_arg(argc, argv) == HELP_MESSAGE)
    {
        printf("%s\n", HELP_MSG);
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

// ======= END OF THE PROGRAM ======= //
