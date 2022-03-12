// 
// File: bracetopia.c 
// A simulation that creates a 'city' of programmer 'agents' who have a 
// preference braces location. The agents are happiest when surrounded
// by other agents who have similar coding styles. The program simulates
// tbe movements of these agents as they seek to increase individual 
// happiness.
//
// @author Omar Ramirez olr8350
//
// // // // // // // // // // // // // / // // // // // // // // // // // 

#define _DEFAULT_SOURCE
#define GETYX( W, NUMROWS, NUMCOLS);
#define GETCURY( W );

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <ncurses.h>

#include "bracetopia.h"

/**
 * print_usage(): Prints the usage message in stderr.
 */

void print_usage(){
   fprintf( stderr, "usage:"
            "\nbracetopia [-h] [-t N] [-c N] [-d dim] [-s %%str] "
            "[-v %%vac] [-e %%end]\n");
   return;
}

/**
 * print_help(): Prints the help usage message.
 */

void print_help(){
   print_usage();
   fprintf( stderr, "%-12s %-10s %-10s %s\n", "Option", "Default",
                   "Example", "Description");
   fprintf( stderr, "%-12s %-10s %-10s %s\n", "'-h'", "NA",
                   "-h", "print this usage message.");
   fprintf( stderr, "%-12s %-10s %-10s %s\n", "'-t N'", "900000", "-t 5000",
                   "microseconds cycle delay.");
   fprintf( stderr, "%-12s %-10s %-10s %s\n", "'-c N'", "NA", "-c4",
                   "count cycle maximum value.");
   fprintf( stderr, "%-12s %-10s %-10s %s\n", "'-d dim'", "15", "-d 7",
                   "width and height dimension.");
   fprintf( stderr,"%-12s %-10s %-10s %s\n", "'-s %str'", "50", "-s 30",
                   "strength of preference.");
   fprintf( stderr, "%-12s %-10s %-10s %s\n", "'-v %vac'", "20", "-v30",
                   "percent vacancies.");
   fprintf( stderr,"%-12s %-10s %-10s %s\n", "'-e %endl'", "60", "-e75",
                   "percent Endline braces. Others want Newline.");
   return;
}

/**
 * create_array(): creates the inital array with correct proportions of 
 * 'agents' and vacant cells.
 */

void create_array(char* pointer, int size, int vacancy, int endline) {
   //getting correct proportions for each char type
   double vacancy_percent = (double) vacancy * .01;
   double endline_percent = (double) endline * .01;
   int vacant_cells = size * vacancy_percent;
   int endline_cells = (size - vacant_cells) * endline_percent; 

   //inputting each char type into array
   for(int i = 0; i < vacant_cells; i++){
      pointer[i] = '.';
   }
   for(int j = vacant_cells; j < endline_cells + vacant_cells; j++) {
      pointer[j] = 'e';
   }
   for(int k = endline_cells + vacant_cells; k < size; k++) {
      pointer[k] = 'n';
   }
   return;
}

/**
 * shuffle_array(): shuffles the array with Fisher-Yates shuffle algorithm.
 */

void shuffle_array(char* pointer, int size) {
   long int num = 0;
   int temp = 0;
   //Fisher-Yates algorithm: from low to high index
   for(int i = 0; i < size - 2; i++) { 
      num = (random() % (size - i)) + i;   //random number i <= j < size
      temp = pointer[i];
      pointer[i] = pointer[num];
      pointer[num] = temp;
   }
   return;
}

/**
 * fill_neighborhood(): fills the city with randomized values and correct
 * proportions.
 */

void fill_neighborhood(char* pointer, int size,
	       	int dimension, char grid[][dimension]) {
   int counter = 0;
   while(counter < size) {
      for(int i = 0; i < dimension; i++){
         for(int j = 0; j < dimension; j++){
            grid[i][j] = pointer[counter];
            counter++;
         }
      }
   }   
   return;
}   

/**
 * is_valid(): determines whether a neighboring cell is a valid neighbor.
 */

bool is_valid(int rows, int cols, int size){
   if(rows < 0 || rows > size - 1 || cols < 0 || cols > size - 1){
      return false;
   } else {
      return true;
   }
}

/**
 * return_happiness(): returns the current happiness of an individual 'agent'.
 */

double return_happiness(int dimension, char grid[][dimension],
	       	int row, int col){
   double happiness = 1;
   int real_neighbors = 0, empty = 0;
   int good_neighbor = 0, invalid_neighbor = 0;
   char type = grid[row][col];

   //array depiciting values in array of neighbors
   int rows[] = { 0, 0, -1, -1, -1, 1, 1, 1 };
   int cols[] = { -1, 1, 0, -1, 1, -1, 0, 1 }; 

   //does not apply to empty spots 
   if(type == '.') {
      return 0;
   }

   for(int i = 0; i < 8; i++) {
      if(is_valid(row + rows[i], col + cols[i], dimension)) {
         if(grid[row + rows[i]][col + cols[i]] == '.') {
            empty++;
	 } else if(grid[row + rows[i]][col + cols[i]] == type) {
            good_neighbor++;	
	 }
      } else {
         invalid_neighbor++;
      }
   }
   
   real_neighbors = 8 - invalid_neighbor;
   //case if cell has no real neighbors, happiness is one
   if(real_neighbors == empty){
      return happiness;
   }
   real_neighbors -= empty;
   happiness = (double) good_neighbor / (double) real_neighbors;
   return happiness;
}

/**
 * check_happiness(): checks the happiness of an agent 
 */

bool check_happiness(double happiness, int strength){
   if ((happiness * 100) < strength) {
      return false;
   } 
   return true;
}

/**
 * look_for_vacancy(): given that an agent is unhappy, and there are
 * spots available for it to move, it will relocate the agent.
 */

void look_for_vacancy(int dimension, char grid[][dimension],
		int row, int col, char curr){
   for(int i = 0; i < dimension; i++){
      for(int j = 0; j < dimension; j++){
         if(grid[i][j] == '.'){
            grid[i][j] = toupper(curr);   //prevent cell from relocating 2x 
	    grid[row][col] = ' ';
	    return;
	 }
      }
   }
   return;
}
            
/**
 * move_rules(): attempt to relocate an agent if they are unhappy.
 * Calls the look_for_vacancy method to move them.
 */

int move_rules(int dimension, char grid[][dimension], 
		bool happy[][dimension]) {
   char curr = ' ';
   int count = 0;

   for(int i = 0; i < dimension; i++){
      for(int j = 0; j < dimension; j++){
         if((grid[i][j] == 'e' || grid[i][j] == 'n') && happy[i][j] == false){
	    curr = grid[i][j];
            look_for_vacancy(dimension, grid, i, j, curr);
	 }
      }
   }

   //getting the next gen of matrix
   for(int row = 0; row < dimension; row++){
      for(int col = 0; col < dimension; col++){
         if(grid[row][col] == ' '){
            grid[row][col] = '.';
	    count++;
	 } else if (grid[row][col] == 'E'){
            grid[row][col] = tolower('E');
	 } else if (grid[row][col] == 'N'){
	    grid[row][col] = tolower('N');
	 }
      }
   }
   return count;
}

/**
 * get_sum(): calculates the total happiness amongst all cells
 */
double get_sum(int dimension, char grid[][dimension],
		bool happy[][dimension], int strength){
   double ind_happiness, sum;
   for(int i = 0; i < dimension; i++) {
      for(int j = 0; j < dimension; j++) {
         ind_happiness = return_happiness(dimension, grid, i, j);
         happy[i][j] = check_happiness(ind_happiness, strength);
         sum += ind_happiness;
      }
   }
   return sum;
}


/**
 * Main function for the program to run the simulation.
 *
 * @param argc    integer number of commmand line arguments
 * @param argv    strings: program-name commandline-argument
 * @returns EXIT_SUCCESS if no error, or EXIT_FAILURE on error
 */

int main( int argc, char * argv[] ) {
   //variables determening city and population aspects
   int dimension = 15, strength = 50, vacancy = 20, endline = 60;
   int count_cycle = -1, time_delay = 900000;
   
   int numrows, numcols;

   //getting the command line arguments and adjusting variables accordingly
   int opt;
   while ( (opt = getopt( argc, argv, "ht:c:d:s:v:e:") ) != -1 ) {
      switch ( opt ) {
      case 'h':
         print_help();
         exit( EXIT_FAILURE );

      case 't':
         time_delay = (int)strtol( optarg, NULL, 10 );
         if (time_delay < 0) {
            time_delay = 900000;
         }
         break;

      case 'c':
         count_cycle = (int)strtol( optarg, NULL, 10 );
         if (count_cycle < 0) {
            fprintf( stderr, "count (%d) must be a non-negative integer.\n",
                     count_cycle);
            print_usage();
            exit( EXIT_FAILURE );
         }
         break;

      case 'd':
         dimension = (int)strtol( optarg, NULL, 10 );
         if (dimension < 5 || dimension > 39) {
            fprintf( stderr, "dimension (%d) must be a value in [5...39]\n",
                     dimension);
            print_usage();
            exit( EXIT_FAILURE );
         }
         break;

      case 's':
         strength = (int)strtol( optarg, NULL, 10 );
         if (strength < 1 || strength > 99) {
            fprintf( stderr, "preference strength (%d) must be a value "
                     "in [1...99]\n", strength);
            print_usage();
            exit ( EXIT_FAILURE );
         }
         break;

      case 'v':
         vacancy =  (int)strtol( optarg, NULL, 10 );
         if (vacancy < 1 || vacancy > 99) {
            fprintf( stderr, "vacancy (%d) must be a value in [1...99]\n",
                     vacancy);
            print_usage();
            exit ( EXIT_FAILURE );
         }
         break;

      case 'e':
         endline = (int)strtol( optarg, NULL, 10 );
         if (endline < 1 || endline > 99) {
            fprintf( stderr, "endline proportion (%d) must be a value in"
                     " [1...99]\n" ,endline);
            print_usage();
            exit( EXIT_FAILURE );
         }
         break;

      default:
         print_usage();
         exit( EXIT_FAILURE );
      }
   }

   //to find the actual population size [getting the correct group happiness]
   int empty_spots = (dimension * dimension) * ((double) vacancy * 0.01);
   int real_life = (dimension * dimension) - empty_spots; 

   //creating the array and city to fill in the population
   int size = dimension * dimension;
   char filled_array[size];
   char neighborhood[dimension][dimension];

   //variables to determine happiness and assist in output
   double overall_happiness = 0, sum = 0;
   bool happy[dimension][dimension];
   int count = 0, moves = 0;


   //creating and randomly filling the array and city population
   create_array(filled_array, size, vacancy, endline);
   srandom( time(NULL) );
   shuffle_array(filled_array, size);
   fill_neighborhood(filled_array, size, dimension, neighborhood);

   if(count_cycle == -1) {
      WINDOW * w = initscr();
      refresh();

      while(1){ 
	 //prints the inital city populated
         for(int i = 0; i < dimension; i++){
            for(int j = 0; j < dimension; j++) {
	       getyx(w, numrows, numcols);
               mvprintw(numrows, numcols, "%c ", neighborhood[i][j]);
            }
	    getyx(w, numrows, numcols);
            mvprintw(numrows, numcols, "\n");
         }

	 //gets the average happiness
	 sum = get_sum(dimension, neighborhood, happy, strength);
         overall_happiness = sum / real_life;
        
	 //prints the data
	 getcury( w );
         mvprintw( numrows, 0, "cycle: %d\n", count);
         mvprintw( numrows + 1, 0, "moves this cycle: %d\n", moves);
         mvprintw( numrows + 2, 0, "teams' \"happiness\": %.4f\n",
	           overall_happiness);
         mvprintw( numrows + 3, 0, "dim: %d, %%strength of preference: %d%%,"
                   " %%vacancy: %d%%, %%end: %d%%\n",
                   dimension, strength, vacancy, endline);
	 mvprintw( numrows + 4, 0, "Use Control-C to quit."); 

         moves = move_rules(dimension, neighborhood, happy);
         refresh();

	 //essentially a reset
         mvprintw(0,0, "");
         count++;
         sum = 0;
         usleep(time_delay);
      }
      endwin();

   } else {
      //printing matrix
      while(count < count_cycle + 1){
         for(int i = 0; i < dimension; i++){
            for(int j = 0; j < dimension; j++){
               printf("%c", neighborhood[i][j]);
            }
            printf("\n");
         }
	 
	 //getting average happiness
	 sum = get_sum(dimension, neighborhood, happy, strength);
         overall_happiness = sum / real_life;
  
	 //printing the data
         printf("cycle: %d\nmoves this cycle: %d\nteams' \"happiness\": %.4f\n"
                "dim: %d, %%strength of preference: %d%%, %%vacancy: %d%%,"
                " %%end: %d%%\n", count, moves, overall_happiness, dimension,
                strength, vacancy, endline);
         moves = move_rules(dimension, neighborhood, happy);
         count++;
         sum = 0;
      }
   }
   return 0;
}

