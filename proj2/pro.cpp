/**
 * Author: René Rešetár (xreset00)
 * Mail: xreset00@stud.fit.vutbr.cz
 * PRL project number 2: Assign preorder order to vertices
 */
#include <chrono>
#include <iostream>
#include <fstream>
#include <mpi.h>

using namespace std;

/**
 * Function computes next edge e' in Euler Tour for edge e
 * For the computation it defines first half of the edges as forward edges
 * and the rest as the reverse edges
 * @Param edge_number number of current edge
 * @Param number_of_vertices number of all vertices
 */
int get_next_in_eltour(int edge_number, int number_of_vertices){
    // The last edge that leads to the root of the graph
    if(edge_number == number_of_vertices + 1)
        return number_of_vertices + 1;
    // The reverse edges in the graph
    if(edge_number >= number_of_vertices ){
        // The reverse edges of the left node that have right node in which to continue
        if(((edge_number-number_of_vertices) % 2 == 0) && (edge_number < 2 * number_of_vertices - 2))
            return edge_number - number_of_vertices + 2;
            // The reverse edges of the right nodes or the left nodes that have no right node
        else
            return (number_of_vertices - 2) / 2 + (edge_number / 2);
    }
        // The forward edges (first half of all). So for 7 vertices we have 12 edges where first 6 will be forward
    else if(edge_number < number_of_vertices && edge_number >= number_of_vertices/2)
    {
        // The forward edges that lead to the leaf nodes
        return (edge_number + number_of_vertices) - 1;
    }else if(edge_number < number_of_vertices){
        // The forward edges that lead to the sub-roots
        return edge_number * 2 + 1;
    }
}

int main(int argc, char**argv) {

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // If there are less or equal vertices than 3 there is no point in continuing
    if (size <= 3) {
        if (rank == 0)
            cout << argv[1] << endl;
        MPI_Finalize();
        return 0;
    }

    string myString = argv[1];
    int vertices = myString.length();
    int edge = rank + 1;
    int *next = new int;
    int *weight = new int;

    // Every process (is representing one edge) gets edge that follows after
    *next = get_next_in_eltour(edge, vertices);

    // The weight array will have 1s in the first half and 0s in the second as we defined for our purposes
    *weight = edge < vertices ? 1 : 0;

    int *euler = new int[size];
    int *weights = new int[size];

    // Every process sends the edge that follows after it to the process num 0
    // And also sends its weight
    if (rank != 0) {
        MPI_Send(next, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
        MPI_Send(weight, 1, MPI_INT, 0, size + rank, MPI_COMM_WORLD);
    } else {
        // Process num 0 gets all the following edges and stores them in euler array that represents Euler tour
        // Same for weights in weights array where 1 is for forward edge and 0 for reverse
        for (int i = 1; i < size; i++) {
            MPI_Recv(&euler[i], 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
            MPI_Recv(&weights[i], 1, MPI_INT, i, size + i, MPI_COMM_WORLD, &status);
        }
        // The first edge sets his next edge and weight in array by itself
        euler[rank] = *next;
        weights[rank] = *weight;
    }

    // Send euler and weights arrays to ecery process
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Send(euler, size, MPI_INT, i, size + i, MPI_COMM_WORLD);
            MPI_Send(weights, size, MPI_INT, i, 2*size + i, MPI_COMM_WORLD);
        }
    } else {
        // Process num 0 gets all the following edges and stores them in euler array that represents Euler tour
        // Same for weights in weights array where 1 is for forward edge and 0 for reverse
        MPI_Recv(euler, size, MPI_INT, 0, size + rank, MPI_COMM_WORLD, &status);
        MPI_Recv(weights, size, MPI_INT, 0, 2*size + rank, MPI_COMM_WORLD, &status);
        // The first edge sets his next edge and weight in array by itself
    }

    int next_edge = edge;
    int forward_edges = 0;
    int *suffix_sum = new int[size];

    // Compute sum of suffix
    while(next_edge != vertices + 1){
        forward_edges = forward_edges + weights[next_edge - 1];
        next_edge = euler[next_edge - 1];
    }

    // Save all the sums to the array
    if (rank != 0){
        MPI_Send(&forward_edges, 1, MPI_INT, 0, size + rank, MPI_COMM_WORLD);
    }else{
        for (int i = 1; i < size; i++) {
            MPI_Recv(&suffix_sum[i], 1, MPI_INT, i, size + i, MPI_COMM_WORLD, &status);
        }
        suffix_sum[rank] = forward_edges;
    }


    // Create PreOrder path
    if(rank == 0){
        char* fin = new char[vertices+1];
        // For all the forward edges
        for(int i=0; i < vertices - 1; i++){
            fin[vertices-suffix_sum[i]] = myString[i+1];
        }
        // For the root (first node)
        fin[0] = myString[0];
        fin[vertices] = '\0';
        // Print final order
        cout<<fin<<endl;
        delete fin;
    }

    // clean up
    delete next;
    delete weight;
    delete weights;
    delete suffix_sum;
    delete euler;

    MPI_Finalize();



    return EXIT_SUCCESS;
}