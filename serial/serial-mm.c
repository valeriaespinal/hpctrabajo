
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
	if (argc!=2){
		printf("uso: %s <n>\n", argv[0]);
		exit(1);
	}
	int n = atoi(argv[1]);
	int **a,**b, **c;
	int i,j,k;
	float dot;
	//Asignar memoria para las matrices a,b y c
	a = (int **)malloc(n * sizeof(int *));
	b = (int **)malloc(n * sizeof(int *)); 
	c = (int **)malloc(n * sizeof(int *));
	for (i = 0; i < n; i++) {
		a[i] = (int *)malloc(n * sizeof(int));
		b[i] = (int *)malloc(n * sizeof(int));
		c[i] = (int *)malloc(n * sizeof(int));
	}
	srand(time(NULL));
	for (i = 0; i < n; i++)
		{
		for(j=0; j < n; j++)
			{
			a[i][j] = (rand()%4)+1;
			b[i][j] = (rand()%4)+1;
		}
	}
	for (i=0;i<n;i++) {
		for(j=0;j<n;j++) {
			dot=0.0;
			for (k=0;k<n;k++) {
				dot += a[i][k]*b[k][j];
			}
			c[i][j]= (int)dot;
		}
	}
	for (i=0; i<n;i++) {
		free(a[i]);
		free(b[i]);
		free(c[i]);
	}
	free(a);
	free(b);
	free(c);
}
