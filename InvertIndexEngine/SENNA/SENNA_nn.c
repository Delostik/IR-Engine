#include "SENNA_nn.h"
#include "SENNA_utils.h"

#include <string.h>
#include <float.h>


void SENNA_nn_lookup(float *dest, int dest_stride, const float *wordweights, int wordsize, int maxwordidx, const int *wordindices, int nword, int padidx, int npad) {
	int i;

	if (padidx < 0 || padidx >= maxwordidx)
		SENNA_error("lookup: padding index out of range");

	for (i = 0; i < npad; i++)
		memcpy(dest + i*dest_stride, wordweights + padidx*wordsize, wordsize*sizeof(float));

	for (i = 0; i < nword; i++) {
		int wordidx = wordindices[i];
		if (wordidx < 0 || wordidx >= maxwordidx)
			SENNA_error("lookup: index out of range");

		memcpy(dest + (i + npad)*dest_stride, wordweights + wordidx*wordsize, wordsize*sizeof(float));
	}

	for (i = 0; i < npad; i++)
		memcpy(dest + (i + npad + nword)*dest_stride, wordweights + padidx*wordsize, wordsize*sizeof(float));
}

void SENNA_nn_hardtanh(float *output, float *input, int size) {
	int i;

	for (i = 0; i < size; i++) {
		float z = input[i];
		if (z >= -1 && z <= 1)
			output[i] = z;
		else if (z < -1)
			output[i] = -1;
		else
			output[i] = 1;
	}
}

void SENNA_nn_linear(float *output, int output_size, float *weights, float *biases, float *input, int input_size) {
	int i, j;

	for (i = 0; i < output_size; i++) {
		float z = (biases ? biases[i] : 0);
		float *weights_row = weights + i*input_size;
		for (j = 0; j < input_size; j++)
			z += input[j] * weights_row[j];
		output[i] = z;
	}
}


#define NN_MIN(a,b) ((a) < (b) ? (a) : (b))
#define NN_MAX(a,b) ((a) > (b) ? (a) : (b))


void SENNA_nn_viterbi(int *path, float *init, float *transition, float *emission, int N, int T) {
	float *delta, *deltap;
	int *phi;
	int i, j, t;


	delta = SENNA_malloc(sizeof(float), N);
	deltap = SENNA_malloc(sizeof(float), N);
	phi = SENNA_malloc(sizeof(float), N*T);


	for (i = 0; i < N; i++)
		deltap[i] = init[i] + emission[i];


	for (t = 1; t < T; t++) {
		float *deltan = delta;
		for (j = 0; j < N; j++) {
			float maxValue = -FLT_MAX;
			int maxIndex = 0;
			for (i = 0; i < N; i++) {
				float z = deltap[i] + transition[i + j*N];
				if (z > maxValue) {
					maxValue = z;
					maxIndex = i;
				}
			}
			delta[j] = maxValue + emission[j + t*N];
			phi[j + t*N] = maxIndex;
		}
		delta = deltap;
		deltap = deltan;
	}


	float maxValue = -FLT_MAX;
	int maxIndex = 0;
	for (j = 0; j < N; j++) {
		if (deltap[j] > maxValue) {
			maxValue = deltap[j];
			maxIndex = j;
		}
	}
	path[T - 1] = maxIndex;


	for (t = T - 2; t >= 0; t--)
		path[t] = phi[path[t + 1] + (t + 1)*N];

	SENNA_free(delta);
	SENNA_free(deltap);
	SENNA_free(phi);
}
