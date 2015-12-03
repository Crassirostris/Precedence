#include "shift_reduce_algorithm.h"

void ShiftReduceAlgorithm::Reduce(std::vector<char> &stack, std::vector<char> &input, int pivot_length, char substitute, std::vector<std::string> &deriviation) const {
	stack.resize(stack.size() - pivot_length);
	stack.push_back(substitute);
	std::string rform;
	rform.resize(stack.size() + input.size());
	std::copy(stack.begin(), stack.end(), rform.begin());
	std::copy(input.begin(), input.end(), rform.rbegin());
	deriviation.push_back(rform);
}

std::vector<std::string> ShiftReduceAlgorithm::Parse(std::string string) const {
	std::vector<char> stack, input;
	input.resize(string.length());
	std::copy(string.begin(), string.end(), input.begin());
	std::reverse(input.begin(), input.end());

	std::vector<std::string> deriviation = { string };
	int pivot_length;
	char substitute;
	bool is_pivot;
	while ((is_pivot = pivot_detector_.IsPivot(stack, input, pivot_length, substitute)) || input.size() > 0) {
		if (is_pivot) {
			Reduce(stack, input, pivot_length, substitute, deriviation);
		} else {
			stack.push_back(input.back());
			input.pop_back();
		}
	}
	if (!(stack.size() == 1 && stack.back() == grammar_.axiom))
		throw std::invalid_argument("Gived string cannot be produced in given grammar");
	return deriviation;
}