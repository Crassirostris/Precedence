#include <iostream>
#include <fstream>
#include "common.h"
#include "grammar.h"
#include "weak_precedence_pivot_detector.h"
#include "shift_reduce_algorithm.h"

void check_stream(const char *filename, const std::basic_ios<char> &stream) {
	if (stream.bad()) {
		std::cerr << "Failed to open file " << filename << std::endl;
		exit(1);
	}
}

void initialize_input(int argc, char **argv, std::ifstream &input, std::ofstream &output) {
	auto input_filename = argc < 2 ? kDefaultInputFilename : argv[1];
	input = std::ifstream(input_filename);
	check_stream(input_filename, input);
	auto output_filename = argc < 3 ? kDefaultOutputFilename : argv[2];
	output = std::ofstream(output_filename);
	check_stream(input_filename, output);
}

int main(int argc, char **argv) {
	std::ifstream input;
	std::ofstream output;
	initialize_input(argc, argv, input, output);

	try {
		auto grammar = InputStreamGrammarSerializer::Deserialize(input);
		auto detector = WeakPrecedencePivotDetector::CreateFromGrammar(grammar);
		ShiftReduceAlgorithm algorithm(detector, grammar);
		std::string input_string;
		input >> input_string;
		auto deriviation = algorithm.Parse(input_string);

		for (auto j = deriviation.begin(); j != deriviation.end(); ++j)
			output << *j << std::endl;
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		output << e.what() << std::endl;
		return 1;
	}

	return 0;
}