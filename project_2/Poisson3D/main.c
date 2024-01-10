/* main.c - Poisson problem in 3D
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "alloc3d.h"
#include "cube_utils.h"
#include "print.h"

#ifdef _JACOBI
#include "jacobi.h"
#endif

#ifdef _GAUSS_SEIDEL
#include "gauss_seidel.h"
#endif

#define N_DEFAULT 20

int
main(int argc, char *argv[]) {

    // Default parameters
    int 	N = N_DEFAULT;
    int 	iter_max = 1000;
    double	tolerance = 0.01;
    double	start_T = 10; // mid of 0 and 20
    int		output_type = 4;

    char	*output_prefix = "poisson_res";
    char        *output_ext    = "";
    char	output_filename[FILENAME_MAX];
    double 	***u = NULL;
    double ***f =  NULL;


    /* get the paramters from the command line */
    // N         = atoi(argv[1]);	// grid size
    // iter_max  = atoi(argv[2]);  // max. no. of iterations
    // tolerance = atof(argv[3]);  // tolerance
    // start_T   = atof(argv[4]);  // start T for all inner grid points
    // if (argc == 6) {
	// output_type = atoi(argv[5]);  // ouput type
    // }

    // allocate memory
    if ( (u = malloc_3d(N, N, N)) == NULL ) {
        perror("array u: allocation failed");
        exit(-1);
    }
    // Create cube u and force f
    if ( (f = malloc_3d(N, N, N)) == NULL ) {
        perror("array f: allocation failed");
        exit(-1);
    }

    // Init the u cube
    init_cube(u, N, start_T);
    
    // init force
    init_force(f, N);

    #ifdef _GAUSS_SEIDEL
    gauss_seidel(u,f, N,iter_max, tolerance);
    #else
    jacobi(input, output, f, N, iter_max, tolerance);

    // dump  results if wanted 
    switch(output_type) {
	case 0:
	    // no output at all
	    break;
	case 3:
	    output_ext = ".bin";
	    sprintf(output_filename, "%s_%d%s", output_prefix, N, output_ext);
	    fprintf(stderr, "Write binary dump to %s: ", output_filename);
	    print_binary(output_filename, N, u);
	    break;
	case 4:
	    output_ext = ".vtk";
	    sprintf(output_filename, "%s_%d%s", output_prefix, N, output_ext);
	    fprintf(stderr, "Write VTK file to %s:\n", output_filename);
	    print_vtk(output_filename, N, u);
	    break;
	default:
	    fprintf(stderr, "Non-supported output type!\n");
	    break;
    }

    // de-allocate memory
    free_3d(u);

    return(0);
}
