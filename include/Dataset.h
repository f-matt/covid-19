/*
 * CustomDataset.h
 *
 *  Created on: Apr 10, 2020
 *      Author: fernando
 */
#ifndef COVID19_DATASET_H_
#define COVID19_DATASET_H_

#include <iostream>
#include <torch/torch.h>
#include <pqxx/pqxx>

namespace covid19 {

	class Dataset : public torch::data::Dataset<Dataset> {

	public:
		Dataset(unsigned experiment_id,
				unsigned input_size,
				unsigned output_size,
				std::string sample_type,
				float normalization);

		virtual ~Dataset();

		torch::data::Example<> get(size_t index) override;

		torch::optional<size_t> size() const override;

	private:
		torch::Tensor inputs;

		torch::Tensor labels;

		unsigned n_samples;

		float normalization;

	};

}

#endif /* COVID19_DATASET_H_ */
