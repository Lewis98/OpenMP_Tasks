# OpenMP_Tasks
Parallel Distributed Programming using OpenMP and MPI for 6004CEM module at Coventry University achieving a Grade of 99% for this submission.


## Distribution
Distributed programming using MPI on a node cluster

### Node_Info
Program for the collecting of data from all nodes within the cluster.
Collected data consists of:

* Node Count (Only printed on head node)
* Node Name
* Number of cores and clock speed(s)
* System RAM

Data is finally printed to the console of the head node.


### Poem_task
Program that loads text specified in poem.txt (A Woman's Heart by Marietta Holley) into a vector of of lines. 
Lines are then sent to individual nodes using a MPI and OpenMP where they are stored for retrieval.
Once all lines are sent, each node returns a response to the main node of [[total lines held],[original index of line 1],[original index of line 2],[original index of line 3], . . . ].
Finally, the head node requests each line back from each node, to display the original poem in it's original form.

A Custom MPI data struct is used in the passing of messages

## Parallelism
Program that initialises ten 3-Dimensional vectors, and performs a time series using OpenMP to provide parallel programming functionality.

Each step, each vector is altered by n dimensions, defined by a particle position alteration variable (default is 2).
Altering a vector dimension consists of adding a randomised number between -5 and 5, in order to move it in a random direction.
After 50 steps, the central point of all vectors is located. For the final 50 steps, all vectors are moved towards the central point, 
using the same method of moving n dimensions by a random number from 0 to 5 (in the direction of the central point).
