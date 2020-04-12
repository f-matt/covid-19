#include "matplotlibcpp.h"
#include "Dataset.h"
#include "Net.h"

#include <torch/torch.h>
#include <pqxx/pqxx>

#include <iostream>

using namespace torch;
using namespace std;
using namespace pqxx;
using namespace covid19;


constexpr unsigned experiment_id = 2;
constexpr unsigned input_size = 4;
constexpr unsigned output_size = 1;
constexpr unsigned batch_size = 4;

constexpr unsigned max_epochs = 1000;

constexpr float normalization = 1e6;

constexpr float learning_rate = 0.001;

constexpr unsigned early_stopping = 50;

const string snapshot_file = "snapshots/3-fcr64-x5.pt";


void train() {

	// Create net
	auto net = make_shared<Net>(input_size, output_size);

	// Dataset
	auto train_data_set = Dataset(experiment_id, input_size, output_size, "T", normalization).map(data::transforms::Stack<>());
	auto train_data_loader = data::make_data_loader<data::samplers::SequentialSampler>(
			move(train_data_set),
			batch_size);

	auto val_data_set = Dataset(experiment_id, input_size, output_size, "V", normalization).map(data::transforms::Stack<>());
	auto val_data_loader = data::make_data_loader<data::samplers::SequentialSampler>(
			move(val_data_set),
			batch_size);

	auto test_data_set = Dataset(experiment_id, input_size, output_size, "X", normalization).map(data::transforms::Stack<>());
	auto test_data_loader = data::make_data_loader<data::samplers::SequentialSampler>(
			move(test_data_set),
			batch_size);

	cout << "Loaded " << train_data_set.size().value() << " train, " << val_data_set.size().value() << " validation "
			<< " and " << test_data_set.size().value() << " test samples\n";

	optim::Adam optimizer(net->parameters(), learning_rate);

	unsigned batches_per_epoch = train_data_set.size().value() / batch_size;
	unsigned val_batches_per_epoch = val_data_set.size().value() / batch_size;
	unsigned test_batches_per_epoch = test_data_set.size().value() / batch_size;

	vector<float> train_loss;
	vector<float> val_loss;

	unsigned epochs_without_improvement = 0;
	float best_val_loss = 1e100;

	for (size_t epoch = 0; epoch < max_epochs; ++epoch) {

		float epoch_train_loss = 0;
		float epoch_val_loss = 0;

		for (auto& batch : *train_data_loader) {
			optimizer.zero_grad();
			Tensor prediction = net->forward(batch.data);
			Tensor loss = mse_loss(prediction, batch.target);
			loss.backward();
			optimizer.step();
			epoch_train_loss += loss.item<float>();
		}

		train_loss.push_back(epoch_train_loss / batches_per_epoch);

		for (auto& batch : *val_data_loader) {
			Tensor prediction = net->forward(batch.data);
			Tensor loss = mse_loss(prediction, batch.target);
			epoch_val_loss += loss.item<float>();
		}

		val_loss.push_back(epoch_val_loss / val_batches_per_epoch);

		cout << "[Epoch " << epoch + 1 << "] Train: " << train_loss[epoch] << " Val: " << val_loss[epoch] << ".\n";

		if (val_loss[epoch] < best_val_loss) {
			epochs_without_improvement = 0;
			best_val_loss = val_loss[epoch];
			save(net, "snapshots/model.pt");
		} else {
			epochs_without_improvement++;

			if (epochs_without_improvement == early_stopping) {
				cout << "No improvement after " << early_stopping << " consecutive epochs. Stopping training.\n";
				break;
			}
		}

	}

	float test_loss = 0;

	load(net, "snapshots/model.pt");

	for (auto& batch : *test_data_loader) {
		Tensor prediction = net->forward(batch.data);
		Tensor loss = mse_loss(prediction, batch.target);

		test_loss += loss.item<float>();
	}

	test_loss /= test_batches_per_epoch;

	cout << "Test loss: " << test_loss << '\n';

	// Plot training results
	matplotlibcpp::named_plot("Train loss", train_loss);
	matplotlibcpp::named_plot("Val loss", val_loss);

	matplotlibcpp::legend();
	matplotlibcpp::show();


}


void test() {

	// Create net
	auto net = make_shared<Net>(input_size, output_size);

	load(net, snapshot_file);

	auto test_data_set = Dataset(experiment_id, input_size, output_size, "X", normalization).map(data::transforms::Stack<>());
	auto test_data_loader = data::make_data_loader<data::samplers::SequentialSampler>(
			move(test_data_set),
			1);

	float test_loss = 0;

	for (auto& batch : *test_data_loader) {
		Tensor prediction = net->forward(batch.data);
		Tensor loss = mse_loss(prediction, batch.target);

		float *data  = (float*) batch.data.data_ptr();
		cout << "Inputs: ";

		for(int i = 0; i < input_size; ++i)
			cout << data[i] << " ";

		cout << "Ground truth: " << batch.target.item<float>()
				<< " Predicted: " << prediction.item<float>() << '\n';

		test_loss += loss.item<float>();
	}

	test_loss /= test_data_set.size().value();

	cout << "Test loss: " << test_loss << '\n';

}


void evaluate(string code) {

	try {
		connection conn("dbname=covid19 user=postgres password=123456 hostaddr=127.0.0.1 port=5432");
		if (conn.is_open()) {
			work txn(conn);

			result rset = txn.exec(
				"select "
				"cases "
				"from notifications "
				"where code = " + txn.quote(code) +
				" and cases > 100 "
				"order by notification_date");

			vector<int> ground_truth;
			for (auto row : rset)
				ground_truth.push_back(row["cases"].as<int>());

			vector<int> predicted;
			auto net = make_shared<Net>(input_size, output_size);

			load(net, snapshot_file);

			for (int i = 0; i < 5; ++i)
				predicted.push_back(ground_truth[i]);

			while (predicted.size() < ground_truth.size()) {

				vector<float> v_input;

				for (int i = input_size - 1; i >= 0; --i)
					v_input.push_back( *(predicted.rbegin() + i) / normalization);

				Tensor input = torch::from_blob(v_input.data(), {1, input_size}); // @suppress("Invalid arguments")
				Tensor prediction = net->forward(input).mul(1e6).round();

				predicted.push_back(prediction.item<int>());
			}

			matplotlibcpp::named_plot("Ground truth", ground_truth);
			matplotlibcpp::named_plot("Prediction", predicted);

			matplotlibcpp::legend();
			matplotlibcpp::show();


		} else {
			cerr << "Error opening database.\n";
		}
		conn.disconnect();
	} catch (const std::exception &e) {
		cerr << e.what() << endl;
	}






}


int main() {

	// train();

	test();

	// evaluate("BRA");


}
