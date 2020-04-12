/*
 * Net.h
 *
 *  Created on: Apr 10, 2020
 *      Author: fernando
 */
#ifndef NET_H_
#define NET_H_

#include <torch/torch.h>

class Net : public torch::nn::Module {

public:
	Net(unsigned input_size, unsigned output_size, unsigned n_hidden_layers, unsigned neurons);

	virtual ~Net();

	torch::Tensor forward(torch::Tensor x);


private:
	std::vector<torch::nn::Linear> hidden_layers;

	torch::nn::Linear output{nullptr};

};


#endif /* NET_H_ */
