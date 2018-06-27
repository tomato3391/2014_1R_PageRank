#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double*		allocateVector(int, double);
double**	allocateMatrix(int, double);
void		loadAdjacencyMatrix(char*, double**);
void		generateTPMatrix(double**, int);
double*		calculatePageRank(double*, double**, int, double, int);
void		writeVector(char*, double*, int);
void		printTopK(double*, int, int);
void		printVector(double*, int);
void		printMatrix(double**, int);

int main(int argc, char *argv[]) {
	// argv[0] = [프로그램 이름]
	// argv[1] = 인접행렬을 포함하는 파일 이름
	// argv[2] = 주어진 Web graph에 있는 페이지 수
	// argv[3] = damping factor 값 (=0.85)
	// argv[4] = 반복 횟수
	// argv[5] = PageRank 점수를 기록할 파일 이름
	// argv[6] specifies top K
	
	char* inputFile = argv[1];
	int numPages = atoi(argv[2]);
	double d = atof(argv[3]);
	int numIteration = atoi(argv[4]);
	char* outputFile = argv[5];
	int topK = atoi(argv[6]);
	
	time_t beginTime = 0, terTime = 0;
	
	double* vector = allocateVector(numPages, 1.0/numPages);
	
	double** matrix = allocateMatrix(numPages, 0.0);
	
	loadAdjacencyMatrix(inputFile, matrix);

	generateTPMatrix(matrix, numPages);
	
	beginTime = clock();
	
	vector = calculatePageRank(vector, matrix, numPages, d, numIteration);
	
	terTime = clock();
	
	printf("Elapsed time is: %f sec\n", (double)(terTime-beginTime)/(double)CLOCKS_PER_SEC);
	
	writeVector(outputFile, vector, numPages);
	
	printTopK(vector, numPages, topK);
	
	free(vector);
	free(matrix);
	
	return 0;
}

double* allocateVector(int n, double initValue) {
	int i;
	
	// Allocate a vector
	double* vector = (double*)malloc(sizeof(double)*n);
	
	// For each element, assign the initial value
	for (i = 0; i < n; i++) vector[i] = initValue;
	
	// Return the vector
	return vector;
}

double** allocateMatrix(int n, double initValue) {
	int i, j;
	
	// A pointer for n * n matrix
	double** matrix;
	
	// Allocate n number of rows in the matrix
	matrix = (double**)malloc(sizeof(double*)*n);
	
	// For each row
	for (i = 0; i < n; i++) {
		// Allocate n number of columns in the matrix
		matrix[i] = (double*)malloc(sizeof(double)*n);
		
		// For each element, assign the initial value
		for (j = 0; j < n; j++) matrix[i][j] = initValue;
	}
	
	// Return the matrix
	return matrix;
}

void loadAdjacencyMatrix(char* inputFile, double** matrix) {
	// A string to get each row in [inputFile]
	char row[15000];
	FILE *stream;
	int i = 0, j = 0;
	char* element;
	
	// Open file stream from [inputFile]
	stream = fopen(inputFile, "r");
	if (stream == NULL) {
		printf("File open error\n");
		exit(1);
	}
	
	// Load the adjacency matrix
	while (!feof(stream)) {
		if (fgets(row, 15000, stream) == NULL) break;
		
		j = 0;
		element = strtok(row, " ");
		matrix[i][j++] = (double)atoi(element);
		while (element = strtok(NULL, " ")) {
			matrix[i][j++] = (double)atoi(element);			
		}
		
		i++;
	}
	
	// Close the file stream
	fclose(stream);
}

void generateTPMatrix(double** matrix, int n)
{
	int count, i, j;
	for (i = 0; i < n; i++)
	{
		count = 0;
		for (j = 0; j < n; j++)
		{
			if (matrix[i][j] == 1)
				count++;
		}
		if (count == 0)
		{
			for (j = 0; j < n; j++)
				matrix[i][j] = 1.0 / n;
		}
		else
		{
			for (j = 0; j < n; j++)
				matrix[i][j] = matrix[i][j] / count;
		}
	}
}

double* calculatePageRank(double* vector, double** matrix, int n, double d, int numIteration)
{
	double temp;
	int i, j, k;
	for (i = 0; i < numIteration; i++)
	{
		for (j = 0; j < n; j++)
		{
			temp = 0;
			for (k = 0; k < n; k++)
				temp += (vector[k] * matrix[k][j]);
			vector[j] = d * temp + (1 - d) * (1.0 / n);
		}
	}
	return vector;
}

void writeVector(char* outputFile, double* vector, int n) {
	char strElement[32];
	FILE *stream;
	int i;
	
	// Open file stream withe [outputFile] to write the vector
	stream = fopen(outputFile, "wb");
	if (stream == NULL) {
		printf("Output stream is NULL. Program terminates.\n");
		exit(1);
	}
	
	// For each element in the vector
	for (i = 0; i < n; i++) {
		// Generate a string that represents the element
		sprintf(strElement, "%f\n", vector[i]);
	
		// Write the string
		if (fputs(strElement, stream) == EOF) {
			printf("fputc() returns NULL. Program terminates.\n");
			exit(1);
		}
	}
	
	// Close the file stream
	fclose(stream);
}

void printTopK(double* vector, int n, int topK) {
	// Define a structure for sorting
	typedef struct _ID_SCORE {
		int pageID;
		double score;
	} ID_SCORE;
	
	int i, j;
	ID_SCORE* rank;
	
	// If topK is not positive or is greater than n,
	// then do nothing
	if ((topK <= 0) || (n < topK)) return;
	
	// Allocate an array of ID_SCORE
	rank = (ID_SCORE*)malloc(sizeof(ID_SCORE)*n);
	
	// Initilize the array
	for (i = 0; i < n; i++) {
		rank[i].pageID = i;
		rank[i].score = vector[i];
	}
	
	// Selection Sort
	for (i = 0; i < n; i++) {
		ID_SCORE temp;
		int index = i;
		
		for (j = i; j < n; j++) {
			if (rank[index].score < rank[j].score) index = j;
		}
		
		temp = rank[i];
		rank[i] = rank[index];
		rank[index] = temp;
	}
	
	// Print the top k ranked pages and the corresponding scores
	printf("Top-%d [pageID: score]\n", topK);
	for (i = 0; i < topK; i++) {
		printf("%4d: %f\n", rank[i].pageID, rank[i].score);
	}
	
	// Free the allocated rank vector
	free(rank);
}

void printVector(double* vector, int n) {
	int i;
	
	for (i = 0; i < n; i++) printf("%f\n", vector[i]);
}

void printMatrix(double** matrix, int n) {
	int i, j;
	
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%f ", matrix[i][j]);
		}
		printf("\n");
	}
}