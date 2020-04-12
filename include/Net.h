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
	Net(unsigned input_size, unsigned output_size);

	virtual ~Net();

	torch::Tensor forward(torch::Tensor x);


private:
	torch::nn::Linear fc1{nullptr};
	torch::nn::Linear fc2{nullptr};
	torch::nn::Linear fc3{nullptr};
	torch::nn::Linear fc4{nullptr};
	torch::nn::Linear fc5{nullptr};

	torch::nn::Linear output{nullptr};

};


#endif /* NET_H_ */
