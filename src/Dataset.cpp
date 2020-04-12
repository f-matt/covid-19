/*
 * CustomDataset.cpp
 *
 *  Created on: Apr 10, 2020
 *      Author: fernando
 */
#include "Dataset.h"

using namespace std;
using namespace torch;
using namespace pqxx;
using namespace covid19;

namespace covid19 {

	Dataset::Dataset(unsigned experiment_id,
			unsigned input_size,
			unsigned output_size,
			std::string sample_type,
			float normalization) : normalization(normalization) {

		try {
			connection conn("dbname=covid19 user=postgres password=123456 hostaddr=127.0.0.1 port=5432");
			if (conn.is_open()) {
				work txn(conn);

				result rset = txn.exec(
					"select "
					"sample_id, seq, value "
					"from samples "
					"where experiment_id = " + txn.quote(experiment_id) +
					" and type = " + txn.quote(sample_type) +
					" order by sample_id, seq");

				n_samples = rset.size() / (input_size + output_size);

				inputs = torch::zeros({n_samples, input_size}); // @suppress("Invalid arguments")
				labels = torch::zeros({n_samples, output_size}); // @suppress("Invalid arguments")

				unsigned idx = -1;
				unsigned last_sample_idx = -1;

				for (auto row : rset) {
					int sample_idx = row["sample_id"].as<int>() - 1;
					int seq = row["seq"].as<int>() - 2;
					float value = row["value"].as<float>() / normalization;

					if (sample_idx != last_sample_idx) {
						last_sample_idx = sample_idx;
						idx++;
					}

					if (seq < 0)
						labels[idx][0] = value;
					else
						inputs[idx][seq] = value;
				}

			} else {
				cerr << "Error opening database.\n";
			}
			conn.disconnect();
		} catch (const std::exception &e) {
			cerr << e.what() << endl;
		}

	}


	Dataset::~Dataset() {

	}


	data::Example<> Dataset::get(size_t index) {

		return {inputs[index], labels[index]};

	}


	optional<size_t> Dataset::size() const {

		return n_samples;

	}

}
