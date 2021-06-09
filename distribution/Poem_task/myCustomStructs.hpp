#ifndef MyCustStruct
#define MyCustStruct

#include "mpi.h" // Required for MPI Context

struct poem_line{
	// Used to store a signluar line from poem
	// Including index of line and length of array
	
	int index; // Index of poem line
	int line_len; // Number of meaningful characters in array
	char text[50]; // Value greater than max number of characters in one line
};

MPI::Datatype MPI_poem_line(); // Method for conversion of poem_line struct into MPI datatype



struct node_data{
	// Used to store data relating to what lines are held by each node

	int owner = -1; // Rank id of node that instance of this struct belongs to
	int tot_lines = 0; // Total lines held by owner node
	int lines[25]; // Array of indexes indicating which lines are held by owner
		       // 25 is total lines of poem
};

MPI::Datatype MPI_node_data(); // Method for converting node_data struct into MPI datatype

#endif
