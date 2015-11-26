#include <iostream>
#include <fstream>
#include "common.h"
#include "grammar.h"
#include "weak_precedence_pivot_detector.h"
#include "shift_reduce_algorithm.h"

void check_stream(const char *filename, std::basic_ios<char> *stream) {
	if (stream->bad()) {
		std::cerr << "Failed to open file " << filename << std::endl;
		exit(1);
	}
}

void initialize_input(int argc, char **argv, std::istream *&input, std::ostream *&output) {
	auto input_filename = argc < 2 ? kDefaultInputFilename : argv[1];
	input = new std::ifstream(input_filename);
	check_stream(input_filename, input);
    output = &std::cout;
    if (argc >= 3) {
        auto output_filename = argv[2];
        output = new std::ofstream(output_filename);
    }
	check_stream(input_filename, output);
}

void show_help(const char *filename) {
    std::cout << "Usage: " << filename << " [-h|--help] [<input_filename> [<output_filename>]]" << std::endl;
    std::cout << "input_filename - name of the file with input. Default to 'input.txt'." << std::endl;
    std::cout << "output_filename - name of the file with output. Default to stdout." << std::endl;
}

bool show_help_message_if_needed(int argc, char** argv) {
    if (argc < 2)
        return false;
    if (strcmp(argv[1], "-h") || strcmp(argv[1], "--help") || strcmp(argv[1], "help")) {
        show_help(argv[0]);
        return true;
    }
    return false;
}

int main(int argc, char **argv) {
    if (show_help_message_if_needed(argc, argv))
        return 0;

	std::istream *input;
	std::ostream *output;
	initialize_input(argc, argv, input, output);

	try {
		auto grammar = InputStreamGrammarSerializer::Deserialize(*input);
		auto detector = WeakPrecedencePivotDetector::CreateFromGrammar(grammar);
		ShiftReduceAlgorithm algorithm(detector, grammar);

        *output << detector.Serialize() << std::endl;

		std::string input_string;
		while (*input >> input_string) {
            auto deriviation = algorithm.Parse(input_string);

            for (auto j = deriviation.begin(); j != deriviation.end(); ++j)
                *output << *j << std::endl;
            *output << std::endl;
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		*output << e.what() << std::endl;
		return 1;
	}

	return 0;
}