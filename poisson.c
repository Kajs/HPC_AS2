#include <stdio.h>
#include <stdlib.h>
struct tuple {
  double v0;
  double v1;
  int using;
};

void printMatrix(int N, double matrix[N][N]) {
  int r, c;
  for(c = 0; c < N; c++) {
    for(r = 0; r < N; r++) {
      printf("%lf ", matrix[r][c]);
    }
    printf("\n\n");
  }
}

void writeMatrix(int N, double matrix[N][N]) {
  FILE *fpout;
  fpout = fopen("data.dat", "wb+");
    if (fpout == NULL) {
      printf("Error opening outfile!\n");
      exit(1);
  }

  int r, c;
  fprintf(fpout, "#x y z\n");
  for(c = 0; c < N; c++) {
    for(r = 0; r < N; r++) {
      double x = r * 2.0/((double) (N-1)) - 1;
      double y = c * 2.0/((double) (N-1)) - 1;
      fprintf(fpout, "%lf %lf %lf\n", x, y, matrix[r][c]);
    }
  }
  fclose(fpout);
}

void jacobi(int N, int loops, double matrix[N][N]) {
  double spacing = 2.0/((double) N);
  printf("spacing=%lf\n", spacing);

  //printMatrix(N, matrix);
  writeMatrix(N, matrix);
}

int main(int argc, char *argv[]) {
  int N = 3;
  if(argc >= 2) { N = atoi(argv[1]); }
  printf("N=%i\n", N);

  int loops = 3;
  if(argc >= 3) { loops = atoi(argv[2]); }
  printf("loops=%i\n", loops);

  double matrix[N][N];

  //Initialization Outer
  int r,c;
  for(c = 0; c < N; c++) {
    matrix[0][c] = 20.0;
    matrix[N-1][c] = 20.0;
  }

  for(r = 0; r < N; r++) {
    matrix[r][0] = 0.0;
    matrix[r][N-1] = 20.0;
  }

  //Initialization Inner
  double initialGuess = -10.0;
  for(r = 1; r < N-1; r++) {
    for(c = 1; c < N-1; c++) {
      matrix[r][c] = initialGuess;
    }
  }
  jacobi(N, loops, matrix);
  return 0;
}
