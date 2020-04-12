/*
 * Net.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: fernando
 */

#include "Net.h"

using namespace torch;
using namespace std;


Net::Net(unsigned input_size, unsigned output_size, unsigned n_hidden_layers, unsigned neurons) {

	unsigned last_layer_output = input_size;

	for (int i = 0; i < n_hidden_layers; ++i) {
		stringstream ss;
		ss << "fc" << (i+1);

		hidden_layers.push_back(register_module(ss.str(), nn::Linear(last_layer_output, neurons)));
		last_layer_output = neurons;
	}

	output = register_module("output", nn::Linear(last_layer_output, output_size));

}


Net::~Net() {

}


// Forward pass
Tensor Net::forward(Tensor x) {

	for (nn::Linear &layer : hidden_layers)
		x = relu(layer->forward(x));

	return output->forward(x);

}

