// 
// File: bracetopia.h
// Functions used for bracetopia.c that will assist in the algorithm to 
// simulate a city of programmers who have a preference for braces. 
// Given that the agents prefer to live with other agents with similar
// coding styles.
//
// @author Omar Ramirez olr8350
// // // // // // // // // // // // // // // // // // // // // // // // //

#ifndef _BRACETOPIA_H_
#define _BRACETOPIA_H_

/**
 * Prints the stderr usage message.
 */

void print_usage();

/**
 * Prints the help usage message. Provides standard usage message
 * with additional helpl lines. 
 */

void print_help();

/**
 * Creates an array filled with the correct proportions of each char.
 * Array is to be used to shuffle and transfer into matrix.
 *
 * @param pointer   reference to the char array
 * @param size      size of the array
 * @param vacancy   user inputted vacancy percentage
 * @param endline   user inputted endline percentage
 */

void create_array(char* pointer, int size, int vacancy, int endline);

/**
 * Shuffles the array to randomly change the order in which the chars
 * are placed. The loop executes from the lowest index to highest, and
 * follows the Fisher-Yates shuffle algorithm. 
 *
 * @param pointer   reference to the char array
 * @param size      size of the array
 */

void shuffle_array(char* pointer, int size);

/**
 * Given the shuffled array, fill_neighborhood will take the inital array
 * values and store them in a size * size matrix. The size will be determined
 * by user input or the default dimension.
 *
 * @param pointer     reference to the char array
 * @param size        size of the array
 * @param dimension   size of the new matrix
 * @param grid        new matrix
 */

void fill_neighborhood(char* pointer, int size, 
		int dimension, char grid[][dimension]);

/**
 * Determines whether the neighbor of a certain cell is a valid location.
 * A valid location is only within the indeces of the matrix.
 *
 * @param rows   the current row
 * @param cols   the current col
 * @param size   the dimension size
 * @returns      'true' if the neighboring cell is valid, else false
 */

bool is_valid(int rows, int cols, int size);

/**
 * Determines the individual happiness value of a given cell by calculating
 * the amount of valid neighbors and their given preference of brackets.
 *
 * @param dimension  size of the matrix
 * @param grid       the matrix
 * @param row        current row of cell
 * @param col        current col of cell
 * @return           the happiness as a decimal
 */

double return_happiness(int dimension, char grid[][dimension],
		int row, int col);

/**
 * Determines whether a cell is happy or not given the current happiness
 * percentage determined by return_happiness() and the strength of preference
 * that was given through command line args. 'true' denotes cell is happy, 
 * whereas 'false' is not happy.
 *
 * @param happiness  current happiness of cell
 * @param strength   strength of preference from command line args
 * @returns          'false' if happiness is less than strength, else 'true'
 */

bool check_happiness(double happiness, int strength);

/**
 * Determines the first place in the matrix that is available for a cell
 * to move into given that they are unhappy. An empty cell is denoted by '.'.
 * During the move cycle, as long as there is an open spot left, a cell 
 * will move, otherwise, will remain put despite unhappy.
 *
 * @param dimension  size of matrix
 * @param grid       matrix
 * @param row        current row of cell
 * @param col        current col of cell
 * @param curr       value of cell at index
 */

void look_for_vacancy(int dimension, char grid[][dimension], 
		int row, int col, char curr);

/**
 * Determines whether the current cell at [row][col] is eligible to move.
 * If current cell is not vacant and is unhappy, it will try to relocate, 
 * using the look_for_vacancy(). Cells that already relocated within the 
 * current move cycle will not be able to move again regardless of happiness
 * status, and if no empty spots left in current cylce, no cells will move.
 *
 * @param dimension   size of matrix
 * @param grid        matrix
 * @param happy       array of bool values
 * @return            the number of moves this move cycle
 */

int move_rules(int dimension, char grid[][dimension], 
		bool happy[][dimension]);

/**
 * Determines the total happiness amongst all cells that are not empty.
 * Puts the bool value of check_happiness in array to later assist
 * with the move rules.
 *
 * @param dimension   size of matrix
 * @param grid        matrix
 * @param happy       bool matrix
 * @param strength    preference values
 * @returns           total happiness amongst non-empty cells
 */

double get_sum(int dimension, char grid[][dimension], 
		bool happy[][dimension], int strength);

#endif
