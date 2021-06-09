
#include <iostream>
#include <string.h>
#include "mpi.h"
#include "omp.h"

#include <fstream>
#include <vector>


#include "myCustomStructs.hpp" // Header file declaring custom structs for MPI data transfer
			       // (Definitions of methods can be found in myCustomStructs.cpp)



std::vector<std::string> loadPoem(char fname[]){
	// Reads poem from file and splits into a vector of strings for each line
	// Takes file name as only argument
	
	// Open filename as stream
	std::ifstream file(fname);

	std::string line; // Line variable to hold each line as string

	std::vector<std::string> result; // Instantiate return object
	
	// Parse file stream by line
	while (getline(file, line)){

		result.push_back(line); // Add line to return object

	};

	file.close(); // Close file stream

	return result; // Return vector containing lines of poem

};

int findOwner(int index, std::vector<node_data> data){
	// Find owner of line by index (returns rank of node which owns line)
	// Takes index to search for, and vector of node_data structs to search through
	
	int result = -1; // set result to -1 to indicate failed to find
	
	// For each set of data retrieved from each node
	#pragma omp parallel for 
	for (int node = 0; node < data.size(); node++){
		// For each line in set of data
		for (int i = 0; i < data[node].tot_lines; i++){
			if (index == data[node].lines[i]){ // If line index held by node matches search index argument
				result = data[node].owner; // Set result to owner of node_data
			} // No break / exit condition to allow parallelisation
		}
	}

	return result; // Return owner of line if found, -1 if not
}

int main(int argc, char** argv){

	// Initialise MPI and pass along program arguments
	MPI_Init(&argc, &argv);

	int node_rank, cluster_size; //Initialiise rank variable and cluster size
	MPI_Comm_rank(MPI_COMM_WORLD, &node_rank); // get rank and assign to node_rank on all nodes
	MPI_Comm_size(MPI_COMM_WORLD, &cluster_size); // Retrieve size of cluster only on main node

	// Custom MPI data transfer types
	MPI::Datatype mpi_poem_struct = MPI_poem_line();
	MPI::Datatype mpi_node_data = MPI_node_data();


	std::vector<poem_line> my_lines; // Used by each node to hold sent lines
	node_data my_data; // Instantiate my_data struct for each node to store (and transmit) poem related data

	int main_node = 0; // For clarity in send / recieve / MPI related logic

	if (node_rank == main_node){
		// Only run following code on main node
		
		// Load poem into vector of strings
		std::vector<std::string> poem = loadPoem("poem.txt");

		// For each line in vector
		#pragma omp parallel for firstprivate(mpi_poem_struct)
		for (int i = 0; i < poem.size(); i++){
		
			poem_line line; // Create struct to hold line data

			line.index = i; // Store index in struct
			line.line_len = poem[i].length(); // Get length of line and store in struct
			
			strcpy(line.text, poem[i].c_str()); //Copy string from poem vector into line struct

			// Get random node from cluster (excluding main node)
			int dst_node = (rand() % (cluster_size - 1) + 1);


			// Send line to randomly selected node
			#pragma omp critical // Ensure only one thread can access MPI_Send at a time
			MPI_Send(&line, 1, mpi_poem_struct, dst_node, 0, MPI_COMM_WORLD);
			
		}
		
		// * - * - * - * Other nodes perform actions between these lines * - * - * - * \\
	
		// For each node in cluster (excluding main node)
		for (int i = 1; i < cluster_size; i++){
			// Create line object
			poem_line line;
			line.index = -1; // Set index to -1 as improvised signal to cease
			MPI_Send(&line, 1, mpi_poem_struct, i, 0, MPI_COMM_WORLD); // Send to node
		}

	} else {
	
		my_data.owner = node_rank; // Set owner of data to this node
					   // (Variable is defined at start of program as needs to be Global scope)
		
		// Set exit condition to false
		bool cease = false; 
		while(!cease){ // While exit condition is unfulfilled
			
	  		poem_line l; // instantiate poem_line struct to recieve data
	
			MPI_Recv(&l, 1, mpi_poem_struct, main_node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Get line sent from main node

			// If line sent from main node is cease signal
			if (l.index == -1){
				cease = true; // Complete exit condition for while loop
			}else{
				// Else process line
				my_data.lines[my_data.tot_lines] = l.index; // Add index of line struct to store line indexs
				my_data.tot_lines += 1; // Increment line counter by 1
				my_lines.push_back(l); // Add line object to list of this node's lines
			}
		}
		
		// After cease signal is sent, send data back to main node to inform of contents
		MPI_Send(&my_data, 1, mpi_node_data, main_node, 0, MPI_COMM_WORLD); // Send node_info struct to main node for printing
	}
	

	if (node_rank == main_node){	
		// On main node
	
		// Create vector of node_data structs to store data from each node
		std::vector<node_data> node_results;
		
		// For each node in cluster (excluding main node)
		for (int i = 1; i < cluster_size; i++){
			node_data node; // Instantiate node_data struct to fill
			MPI_Recv(&node, 1, mpi_node_data, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Fill with data recieved from node
			node_results.push_back(node); // add node_data struct to vector
		}
	

		// Print recieved data to console from main node
		for (int i = 0; i < node_results.size(); i++){

			std::cout << "Node '" << node_results[i].owner << "' holds '" << node_results[i].tot_lines << "' lines : ";
			
			for (int j = 0; j < node_results[i].tot_lines; j++){ // For each line held by node
				std::cout << node_results[i].lines[j]; // Print line index to screen
				if (j < node_results[i].tot_lines - 1) { // And if not last line held
					std::cout << ", ";               // print a comma to deliminate
				}
			}
		
			std::cout << std::endl; // Finally, complete line by printing a new line
		}

		std::cout << "\n\n\n" << std::endl; // Add space between reported data back and task C results for clarity


		// * - - - - - - - PART C Main Node - - - - - - - * 

		int tot_lines_in_poem = 24; // Number of lines in poem

		for (int i = 0; i < tot_lines_in_poem; i++){ // For each line in poem

			int line_owner = findOwner(i, node_results); // Get owner from retrieved node data

			MPI_Send(&i, 1, MPI_INT, line_owner, 0, MPI_COMM_WORLD); // Send index to node that owns line

			poem_line l; // Instantiate poem line to hold data
			MPI_Recv(&l, 1, mpi_poem_struct, line_owner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // retrieve line from node and fill struct
			
			std::cout << l.text << std::endl; // Print text stored in poem line struct (on only main node)
			
		}

		// Once done send cease signal to all nodes
		for (int i = 1; i < cluster_size; i++){ // For all nodes execpt main node
			int cease_signal = -1; // Set cease signal to -1
			MPI_Send(&cease_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Send cease signal to node i
		}
	
	} else {

		// * - - - - - - - PART C Other Nodes - - - - - - - *
		
		bool cease = false; // Set exit condition to false
		while(!cease){ // While exit condition is false
			
			int req; // Request from main node
			MPI_Recv(&req, 1, MPI_INT, main_node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // get request main node

			if (req == -1){ // if request is -1 (exit code)
				cease = true; // Fulfill exit condition to break loop
			} else {
				// Otherwise find and send requested line

				poem_line response; // Instantiate poem_line struct to send in repsonse to main node
				
				for (int line = 0; line < my_lines.size(); line++){ // for each line held by this node
					if (req == my_lines[line].index){ // If line index matches requested index
						response = my_lines[line]; // Set response to line
						break; // Break loop
					};
				};
	
				MPI_Send(&response, 1, mpi_poem_struct, 0, 0, MPI_COMM_WORLD); // Send line back to main node

			}

		}

	}

	MPI_Finalize();

}
