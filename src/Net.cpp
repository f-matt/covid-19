/*
 * Net.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: fernando
 */

#include "Net.h"

using namespace torch;

constexpr unsigned neurons = 64;

Net::Net(unsigned input_size, unsigned output_size) {

	fc1 = register_module("fc1", nn::Linear(input_size, neurons));
	fc2 = register_module("fc2", nn::Linear(neurons, neurons));
	fc3 = register_module("fc3", nn::Linear(neurons, neurons));
	fc4 = register_module("fc4", nn::Linear(neurons, neurons));
	fc5 = register_module("fc5", nn::Linear(neurons, neurons));

	output = register_module("output", nn::Linear(neurons, output_size));

}


Net::~Net() {

}


// Forward pass
Tensor Net::forward(Tensor x) {

	x = relu(fc1->forward(x));
	x = relu(fc2->forward(x));
	x = relu(fc3->forward(x));
	x = relu(fc4->forward(x));
	x = relu(fc5->forward(x));

	x = output->forward(x);

	return x;

}

