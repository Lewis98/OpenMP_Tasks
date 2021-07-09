
#include <iostream>
#include <string.h>
#include "mpi.h"
#include "omp.h"


void getSysInfo(int id){
	// Prints system information to console std out
	// includes name of node, total RAM of node, number of cores, name of each core and its clock speed
	
	int name_len; // length of name
	char node_name[MPI_MAX_PROCESSOR_NAME]; // Variable for name to be stored
	MPI_Get_processor_name(node_name, &name_len); // Get name with MPI

	memset(node_name + name_len, 0, MPI_MAX_PROCESSOR_NAME - name_len); // Fill rest of name variable with null data

	

	std::cout << "\n - - - - - " << node_name << " info: - - - - - \n" << std::endl; // Print node name to screen
	
	// Print RAM and cores using grep to extract data from relevant proc files
	system("grep MemTotal /proc/meminfo | sed 's/MemTotal:/Total Ram:/'");
	system("grep 'cpu cores' /proc/cpuinfo | head -1 | sed 's/cpu cores\t:/Cores:           /'");

	// Print out title for cores
	std::cout << "\n\nProcessors : " << std::endl;
	// Print core name and clock speeds from cpuinfo file for each processor
	system("cat /proc/cpuinfo | grep 'cpu MHz\\|model name'");

};


int main(int argc, char** argv){

	// Initialise MPI and pass along program arguments
	MPI_Init(&argc, &argv);

	int node_rank, cluster_size; //Initialiise rank variable and cluster size
	MPI_Comm_rank(MPI_COMM_WORLD, &node_rank); // get rank and assign to node_rank on all nodes
	MPI_Comm_size(MPI_COMM_WORLD, &cluster_size); // Retrieve size of cluster only on main node

	// define main_node as 0 for clarity in MPI related logic
	int main_node = 0;

	// If node is main node
	if (node_rank == main_node){
	
		// Output total nodes in cluster to console
		std::cout << "\n\nTotal nodes in cluster : " << cluster_size <<  std::endl;
		
		
		// Get system info for main node
		getSysInfo(node_rank);

		// For all other nodes
		for (int n = 1; n < cluster_size; n++){

			int sig = 1; // Signal variable

			// Send signal to print sys info
			MPI_Send(&sig, 1, MPI_INT, n, 0, MPI_COMM_WORLD);
			

			int rec = 0; // Signal variable

			// Wait for response from node before calling next node
			MPI_Recv(&rec, 1, MPI_INT, n, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

	}else{
		// All other nodes

		int rec = 0; // Signal variable

		// Wait for main node to signal this node
		MPI_Recv(&rec, 1, MPI_INT, main_node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		getSysInfo(node_rank); // call function to print sys info to screen
		
		// Send signal back to main node to inform completion of task
		MPI_Send(&rec, 1, MPI_INT, main_node, 0, MPI_COMM_WORLD);

	}

	MPI_Finalize();
}
