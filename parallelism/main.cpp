#include "omp.h"
#include <iostream>
#include <cmath>
#include <cstdlib>


struct particle {
	int pos[3]; // Vector 3 - x,y,z
};




float getDist(particle A, particle B) {
// Gets distance between 2 points in a 3d space

	int x_calc = pow(B.pos[0] - A.pos[0], 2); // X Axis distance squared
	int y_calc = pow(B.pos[1] - A.pos[1], 2); // Y Axis distance squared
	int z_calc = pow(B.pos[2] - A.pos[2], 2); // Z Axis distance squared

	// Get square root of sum to find distance in 3d space
	float dist = sqrt(x_calc + y_calc + z_calc);

	return dist; // Return result as distance between A and B
};

bool allTrue(bool arr[], int len){

	// Simple loop to check all items in boolean array are true
	
	// For items in array
	for (int i = 0; i < len; i++){
		if (!arr[i]){ // If item is false
			return false; // Return false
		}
	}

	return true; // Else if none are false return true

}


int main(int argc, char** argv) {

	// - - - - - Setup and configurable paramteres - - - - - \\

	// Initialise particles into p_list
	particle p_list[] = {
		{5, 14, 10},
		{7, -8, -14},
		{-2, 9, 8},
		{15, -6, 3},
		{12, 4, -5},
		{4, 20, 17},
		{-16, 5, -1},
		{-11, 3, 16},
		{3, 10, -19},
		{-16, 7, 4}
	};

	// Length of array
	int p_len = 10;

	// Particle alteration variable (How many axis are altered per step)
	int alt = 2;

	// Number of steps in time series
	int step_limit = 50;
	

	// - - - - - First time series - - - - - \\
	
	// Used to set each threads rand seed only once:
	bool seed_not_set;

	std::cout << "\n\n\n- - - - - - - - - - - - - - - - FIRST TIME SERIES - - - - - - - - - - - - - - - - \n\n\n" << std::endl; 
	
	// For each step in time series
	for (int step = 0; step < step_limit; step++) {
			
		seed_not_set = true; // Set to true before each parallel region to ensure each thread runs seed code below

		// Pass seed_not_set to all threads and set schedule type as definable at runtime
		#pragma omp parallel for firstprivate(seed_not_set) schedule(runtime)
		// For each particle in p_list
		for (int p = 0; p < p_len; p++){
			
			// If seed is not set (True for first iteration made by each thread)
			if (seed_not_set) {
				srand(omp_get_thread_num()); // Seed rand() with thread number to make thread safe and reproducable
				seed_not_set = false; // Set to false to prevent this block running twice per thread
			};


			// Set edit value of axis (x,y,z) and reset each loop to prevent editing same axis twice per step
			bool edited[] = {false, false, false};

			// For the number of alterations specified
			for (int i = 0; i < alt; i++){		
				int axis = rand() % 3; // Get random axis
				while (edited[axis]) { // If axis has been edited
					
					// If all axis have been edited
					if (allTrue(edited, 3)){
						// Reset all axis back to false
						edited[0] = false;
						edited[1] = false;
						edited[2] = false;
					}

					axis = (axis + 1) % 3; // Increment axis by 1 (mod 3 to loop around)
				};

				// Alter position of particle on selected axis by random number
				p_list[p].pos[axis] += (rand() % 10 + 1) - 5; // Select number between -5 and 5
				// Set axis to edited
				edited[axis] = true;

			}

		}

		
		// If completed halfway point (25th step by default)
		if (step == floor(step_limit / 2)) {
			// Ensure all threads have completed last step
			#pragma omp barrier
			
			// Output results to console
			
			std::cout << " - - - - - - - - Results at step '" << step << "' - - - - - - - - " << std::endl;

			std::cout << "|   Particle \t|    X pos \t|    Y pos \t|    Z pos \t|" << std::endl;

			for (int i = 0; i < 10; i++){
				std::cout << "|    " << i << "    \t|     " << p_list[i].pos[0] << "    \t|     " << p_list[i].pos[1] << "    \t|     " << p_list[i].pos[2] << "    \t|" << std::endl;
			}
		
		}
		
	}

	// Finish by outputting final position
	
	std::cout << " - - - - - - - - Results at final step (" << step_limit << ") - - - - - - - - " << std::endl;
	std::cout << "|   Particle \t|    X pos \t|    Y pos \t|    Z pos \t|" << std::endl;

	for (int i = 0; i < 10; i++){
		std::cout << "|    " << i << "    \t|     " << p_list[i].pos[0] << "    \t|     " << p_list[i].pos[1] << "    \t|     " << p_list[i].pos[2] << "    \t|" << std::endl;
	}


	// - - - - - Retrieving center point - - - - - \\
	
	std::cout << "\n\n\n- - - - - - - - - - - - - - - - FINDING CENTER - - - - - - - - - - - - - - - - \n\n\n" << std::endl; 


	// Use particle struct to represent center
	particle center = {0,0,0}; // Initialise with 0 for x,y & z

	// For each particle in p_list
	for (int i = 0; i < p_len; i++) {
		// Add to center particle x,y,z values
		center.pos[0] += p_list[i].pos[0];
		center.pos[1] += p_list[i].pos[1];
		center.pos[2] += p_list[i].pos[2];
	}

	// Divide by total particles to get avg
	center.pos[0] /= p_len;
	center.pos[1] /= p_len;
	center.pos[2] /= p_len;

	std::cout << "Center position identified as X:'" << center.pos[0] << "' Y:'" << center.pos[1] << "' Z:'" << center.pos[2] << "'" << std::endl;
	
	// - - - - - Get and print distance from center - - - - - \\ 
	
	std::cout << "Distance from center: " << std::endl;

	float old_dist[10]; // Array of distances for particles, named to distinguish from end distance

	// For each particle
	for (int i = 0; i < p_len; i++){
		// Get distance between particle and center
		old_dist[i] = getDist(p_list[i], center);
		// Output distance to console
		std::cout << "Particle " << i << " has a distance of '" << old_dist[i] << "' from center point" << std::endl;
	}
	
	/*					
	/ - - - - -  Second time series - - - - - \
	/ 	    ( Moving to center )          \
	*/

	std::cout << "\n\n\n- - - - - - - - - - - - - - - - LAST TIME SERIES - - - - - - - - - - - - - - - - \n\n\n" << std::endl; 

	for (int step = 0; step < step_limit; step++) {
		
		seed_not_set = true; // Used to run code on first iteration only for each thread
		
		// For all particles in p_list
		#pragma omp parallel for firstprivate(seed_not_set, center)
		for (int p = 0; p < p_len; p++){
			
			if (seed_not_set) {
				srand(omp_get_thread_num()); // Seed rand() with thread number to make thread safe and reproducable
				seed_not_set = false; // Only run once per thread for each for loop	
			};


			// Set edit value of axis (x,y,z) and reset each loop
			bool edited[] = {false, false, false};

			// For number of alterations specified by particle alteration variable
			for (int i = 0; i < alt; i++){		
				
				int axis = rand() % 3; // Get random axis

				while (edited[axis]) { // If axis has been edited
					
					// If all axis have been edited
					if (allTrue(edited, 3)){
						// Reset all axis back to false
						edited[0] = false;
						edited[1] = false;
						edited[2] = false;
					};

					axis = (axis + 1) % 3; // Incriment by 1 (mod 3 to loop around)
				};

				// Get random value between 1 and 5 to travel
				int travel_dist = (rand() % 5 + 1);
 
				// Alter position of particle on selected axis by random number
				if (p_list[p].pos[axis] < center.pos[axis]){
					p_list[p].pos[axis] += travel_dist; // Add value if target holds greater value
				} else {
					p_list[p].pos[axis] -= travel_dist; // Subtract if target holds lesser value
				};

				// Set axis to edited
				edited[axis] = true;
			}
		}


		// If completed halfway point (25th step with default)
		if (step == floor(step_limit / 2)) {
			// Ensure all threads have completed last step
			#pragma omp barrier
			
			// Output results to console
			
			std::cout << " - - - - - - - - Results at step '" << step << "' - - - - - - - - " << std::endl;

			std::cout << "|   Particle \t|    X pos \t|    Y pos \t|    Z pos \t|" << std::endl;

			for (int i = 0; i < 10; i++){
				std::cout << "|    " << i << "    \t|     " << p_list[i].pos[0] << "    \t|     " << p_list[i].pos[1] << "    \t|     " << p_list[i].pos[2] << "    \t|" << std::endl;
			}
		
		}
		
	}
	
	// Print final step positions to console in table format

	std::cout << " - - - - - - - - - - Results at final step (" << step_limit << ") - - - - - - - - - - " << std::endl;

	std::cout << "|   Particle \t|    X pos \t|    Y pos \t|    Z pos \t|" << std::endl;

	for (int i = 0; i < 10; i++){
		std::cout << "|    " << i << "    \t|     " << p_list[i].pos[0] << "    \t|     " << p_list[i].pos[1] << "    \t|     " << p_list[i].pos[2] << "    \t|" << std::endl;
	}
		

	

	// - - - - - Get and print distance from center - - - - - \\ 
	
	// Print headings of table and divider for clarity	
	std::cout << " - - - - - - - - - - - - Final results: - - - - - - - - - - - - " << std::endl;
	
        std::cout << "| Particle  \t| Old Distance \t| New Distance \t| Change \t|" << std::endl;

	// For each particle
	for (int i = 0; i < p_len; i++){
		float dist = getDist(p_list[i], center); // Get distance from center
				
		// Print table row for particle displaying old distance, new distance and change
		std::cout << "|    " << i << "  \t|   " << old_dist[i] << "    \t|   " << dist << "    \t| " << old_dist[i] - dist << " \t| " << std::endl;
		
//		std::cout << "Particle " << i << " has values x:'" << p_list[i].pos[0] << "' y:'" << p_list[i].pos[1] << "' z:'" << p_list[i].pos[2] << "'" << std::endl;
//		std::cout << "    with a distance of '" << dist << "' from center point" << std::endl;
//		std::cout << "    making a change of '" << old_dist[i] - dist << "' from its original distance \n" << std::endl;
	}

	return 0;
}
