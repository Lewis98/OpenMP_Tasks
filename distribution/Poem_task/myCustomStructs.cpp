#include "myCustomStructs.hpp" // Declarations for this file

#include "mpi.h"


/*
 *	     poem_line declaration in header
 *	(for reference in MPI datatype creation)
 *
 *	struct poem_line {
 *		int index;
 *		int line_len;
 *		char text[50];
 *	}
 * */


MPI::Datatype MPI_poem_line(){

	const int varCount = 3; // How many variables are in the struct
	MPI::Datatype vars[varCount] = { // Struct variables
		MPI::INT, // index
		MPI::INT, // line_len
		MPI::CHAR // text[]
	};

	const int varSpace[varCount] = {1,1,50}; // elements per variable

	MPI::Aint addrs[varCount + 1]; // [objaddr, var_1, var_2, . . .]
	MPI::Aint arrDisplacements[varCount]; // Used to identify memory displacement caused by variables

	poem_line buff; // Buffer object to retrieve memory addresses

	// Get address for struct and each of it's variables
	addrs[0] = MPI::Get_address(&buff);
	addrs[1] = MPI::Get_address(&buff.index);
	addrs[2] = MPI::Get_address(&buff.line_len);
	addrs[3] = MPI::Get_address(&buff.text);

	// For each variable in struct
	for (int i = 0; i < varCount; i++){
		// Calculate memory displacement from address of struct instance
		arrDisplacements[i] = addrs[i + 1] - addrs[0];
	};

	MPI::Datatype mpi_return_obj; // Instantiate an MPI Datatype object to return

	mpi_return_obj = MPI::Datatype::Create_struct( // Create a struct of type MPI Datatype that is compatible with poem_line struct
		varCount,
		varSpace,
		arrDisplacements,
		vars
	);

	mpi_return_obj.Commit(); // Commit edits made by Create_struct function
	
	return mpi_return_obj; // Return newly created datatype

};

/*
 *	     node_data declaration in header
 *	(for reference in MPI datatype creation)
 *
 *	struct node_info {
 *		int owner;
 *		int tot_lines;
 *		int lines[];
 *	}
 * */


MPI::Datatype MPI_node_data(){

	const int varCount = 3; // How many variables are in the struct
	MPI::Datatype vars[varCount] = { // Struct variables
		MPI::INT, // owner
		MPI::INT, // tot_lines
		MPI::INT  // lines[]
	};

	const int varSpace[varCount] = {1,1,25}; // elements per variable

	MPI::Aint addrs[varCount + 1]; // [objaddr, var_1, var_2, . . .]
	MPI::Aint arrDisplacements[varCount];

	node_data buff; // Buffer object to retrieve memory addresses
	addrs[0] = MPI::Get_address(&buff);
	addrs[1] = MPI::Get_address(&buff.owner);
	addrs[2] = MPI::Get_address(&buff.tot_lines);
	addrs[3] = MPI::Get_address(&buff.lines);

	// For each variable in struct
	for (int i = 0; i < varCount; i++){
		// Calculate memory displacement from arrays
		arrDisplacements[i] = addrs[i + 1] - addrs[0];
	};

	MPI::Datatype mpi_return_obj; // Object to return

	mpi_return_obj = MPI::Datatype::Create_struct(
		varCount,
		varSpace,
		arrDisplacements,
		vars
	);

	mpi_return_obj.Commit();
	
	return mpi_return_obj;

};

