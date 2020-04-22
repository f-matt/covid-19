/*
 * Net.h
 *
 *  Created on: Apr 10, 2020
 *      Author: fernando
 */
#ifndef NET_H_
#define NET_H_

#include <torch/torch.h>
#include <sstream>

class Net : public torch::nn::Module {

public:
	Net(unsigned input_size,
			unsigned output_size,
			unsigned n_conv_layers,
			unsigned n_hidden_layers,
			unsigned n_filters,
			unsigned n_neurons);

	virtual ~Net();

	torch::Tensor forward(torch::Tensor x);


private:
	std::vector<torch::nn::Conv1d> conv_layers;

	torch::nn::Flatten flatten{nullptr};

	std::vector<torch::nn::Linear> hidden_layers;

	torch::nn::Linear output{nullptr};

};


#endif /* NET_H_ */
