#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>
struct twoPointers {
  double** pointers[2];
  int using;
};


double f(double x, double y) {
  if(x >= 0 && x <= 1.0/3.0 && y >= -2.0/3.0 && y <= -1.0/3.0) { return 200.0; }
  return 0.0;
}

void printMatrix(int N, double** matrix) {
  int r, c;
  for(r = 0; r < N; r++) {
    for(c = 0; c < N; c++) {
      printf("%lf ", matrix[r][c]);
    }
    printf("\n\n");
  }
}

void writeMatrix(int N, struct twoPointers mydata) {
  FILE *fpout;
  fpout = fopen("data.dat", "wb+");
    if (fpout == NULL) {
      printf("Error opening outfile!\n");
      exit(1);
  }

  int r, c;
  fprintf(fpout, "#x y z\n");
  for(r = 0; r < N; r++) {
    for(c = 0; c < N; c++) {
      double x = r * 2.0/((double) (N-1)) - 1;
      double y = c * 2.0/((double) (N-1)) - 1;
      fprintf(fpout, "%lf %lf %lf\n", x, y, mydata.pointers[mydata.using][r][c]);
    }
  }
  fclose(fpout);
}

int jacobi(int N, struct twoPointers mydata, int nthreads) {
  double spacing = 2.0/((double) N);
  int i, r, c, t;
  int new = mydata.using;
  int old;
  int loops = N * N * log(N);
  double epsilon = 0.0001;
  double comparison[nthreads];
  //printf("jac: spacing=%lf\n", spacing);
  

  for (i = 0; i < loops; i++) {
    old = mydata.using;
    if (new == 0) { mydata.using = 1; }
    else { mydata.using = 0; }
    new = mydata.using;
    
    for(t=0; t < nthreads; t++) { comparison[t] = 0; }

    #pragma omp parallel for private(r) private(c)
    for(r = 1; r < N-1; r++) {
      /* TESTING */
      //if(i==0) { printf("thread %i, r=%i\n", omp_get_thread_num(), r); }
      /* ___TESTING */
      int threadId = omp_get_thread_num();
      for(c = 1; c < N-1; c++) {
        double val;
        val = mydata.pointers[old][r-1][c];
        val += mydata.pointers[old][r+1][c];
        val += mydata.pointers[old][r][c-1];
        val += mydata.pointers[old][r][c+1];
        double x = r * 2.0/((double) (N-1)) - 1;
        double y = c * 2.0/((double) (N-1)) - 1;
        val += spacing*spacing*f(x, y);
        mydata.pointers[new][r][c] = val * 0.25;
        double delta = mydata.pointers[new][r][c] - mydata.pointers[old][r][c];
        comparison[threadId] += delta*delta;
      }
    }
    //printf("delta=%lf, delta^2=%lf\n", delta, delta*delta);
    double combinedComparison = 0;
    for(t = 0; t < nthreads; t++) { combinedComparison += comparison[t]; }
    if(i > 0 && combinedComparison < epsilon) { 
      printf("jac:too small difference: combinedComparison = %lf, i=%i\n", combinedComparison, i);
      writeMatrix(N, mydata);
      return i; 
    }
  }
  //printMatrix(N, matrix);
  writeMatrix(N, mydata);
  return loops;
}

void seidel(int N, struct twoPointers mydata) {
  double spacing = 2.0/((double) N);
  int i, r, c;
  int new = mydata.using;
  int old;
  int loops = N * log(N)/log(2) + 1;
  //printf("sei: spacing=%lf\n", spacing);

  for (i = 0; i < loops; i++) {
    old = mydata.using;
    if (new == 0) { mydata.using = 1; }
    else { mydata.using = 0; }
    new = mydata.using;

    for(r = 1; r < N-1; r++) {
      for(c = 1; c < N-1; c++) {
        double val;
        val = mydata.pointers[new][r-1][c];
        val += mydata.pointers[old][r+1][c];
        val += mydata.pointers[new][r][c-1];
        val += mydata.pointers[old][r][c+1];
        
        double x = r * 2.0/((double) (N-1)) - 1;
        double y = c * 2.0/((double) (N-1)) - 1;
        val += spacing*spacing*f(x, y);
        mydata.pointers[new][r][c] = val * 0.25;
      }
    }
  }
  //printMatrix(N, matrix);
  writeMatrix(N, mydata);
}

int main(int argc, char *argv[]) {
  struct timeval t1, t2;
  double elapsedTime;
  gettimeofday(&t1, NULL);

  int N = 3;
  int i;
  int nthreads;
  int completedLoops = -1;
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0) { nthreads = omp_get_num_threads(); }
  } //end parallel
  if(argc >= 2) { N = atoi(argv[1]); }
  //printf("N=%i\n", N);

  char* funcname = "jac";
  if(argc >= 3) { funcname = argv[2]; }
  //printf("using function %s\n", funcname);

  struct twoPointers mydata;
  mydata.using = 0;
  double **matrix;

  if ( (matrix = calloc( N, sizeof(double*) )) == NULL ) {
	perror("main(__LINE__), allocation failed");
	exit(1);
  }
  
  for(i = 0; i < N; i++) {
    if ( (matrix[i] = calloc( N, sizeof(double) )) == NULL ) {
	perror("main(__LINE__), allocation failed");
	exit(1);
    }
  }

  double **matrix2;

  if ( (matrix2 = calloc( N, sizeof(double*) )) == NULL ) {
	perror("main(__LINE__), allocation failed");
	exit(1);
  } 
  for(i = 0; i < N; i++) {
    if ( (matrix2[i] = calloc( N, sizeof(double) )) == NULL ) {
	perror("main(__LINE__), allocation failed");
	exit(1);
    }
  }

  mydata.pointers[0] = matrix;
  mydata.pointers[1] = matrix2;
  mydata.using = 0;

  double hotWall = 20.0;
  double coldWall = 0.0;
  //Initialization Outer
  int r,c;
  for(c = 0; c < N; c++) {
    matrix[0][c] = hotWall;
    matrix[N-1][c] = hotWall;
    matrix2[0][c] = hotWall;
    matrix2[N-1][c] = hotWall;
  }

  for(r = 0; r < N; r++) {
    matrix[r][0] = coldWall;
    matrix[r][N-1] = hotWall;
    matrix2[r][0] = coldWall;
    matrix2[r][N-1] = hotWall;
  }

  //Initialization Inner
  double initialGuess = 5.0;
  for(r = 1; r < N-1; r++) {
    for(c = 1; c < N-1; c++) {
      matrix[r][c] = initialGuess;
    }
  }

  if(strcmp(funcname, "jac") == 0) { completedLoops = jacobi(N, mydata, nthreads); }
  else{ if(strcmp(funcname, "sei") == 0) { seidel(N, mydata); } }
  for(i = 0; i < N; i++) {
    free(matrix[i]);
    free(matrix2[i]);
  }
  free(matrix);
  free(matrix2);

  gettimeofday(&t2, NULL);
  elapsedTime = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1000000.0;
  printf("%i %lf %i %s %i\n", nthreads, elapsedTime, N, funcname, completedLoops);
  return 0;
}
