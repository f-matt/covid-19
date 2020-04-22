/*
 * Net.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: fernando
 */
#include "net.h"

using namespace torch;
using namespace nn;
using namespace std;

const unsigned initial_kernel_size = 5;


Net::Net(unsigned input_size,
		unsigned output_size,
		unsigned n_conv_layers,
		unsigned n_hidden_layers,
		unsigned n_filters,
		unsigned n_neurons)
{
	unsigned last_layer_output = input_size;
	unsigned last_n_filters = 1;
	unsigned kernel_size = initial_kernel_size;

	for (int i = 0; i < n_conv_layers; ++i) {
		stringstream ss;
		ss << "conv" << (i+1);

		conv_layers.push_back(register_module(ss.str(), nn::Conv1d(last_n_filters, n_filters, kernel_size)));

		last_layer_output = last_layer_output - (kernel_size - 1);

		if (kernel_size > 3)
			kernel_size -= 2;

		last_n_filters = n_filters;
	}

	if (!conv_layers.empty()) {
		flatten = register_module("flatten", nn::Flatten());
		last_layer_output *= last_n_filters;
	}

	for (int i = 0; i < n_hidden_layers; ++i) {
		stringstream ss;
		ss << "fc" << (i+1);

		hidden_layers.push_back(register_module(ss.str(), nn::Linear(last_layer_output, n_neurons)));

		last_layer_output = n_neurons;
	}

	output = register_module("output", nn::Linear(last_layer_output, output_size));
}



Net::~Net()
{

}


// Forward pass
Tensor Net::forward(Tensor x)
{
	if (!conv_layers.empty()) {
		x = x.view({x.sizes()[0], 1,  x.sizes()[1]});

		for (Conv1d &layer : conv_layers)
			x = relu(layer->forward(x));

		x = flatten->forward(x);
	}

	for (Linear &layer : hidden_layers)
		x = relu(layer->forward(x));

	return output->forward(x);
}
