#pragma once

#include <vector>
#include "pivot_detector_interface.h"
#include "grammar.h"

class ShiftReduceAlgorithm {
public:
	explicit ShiftReduceAlgorithm(PivotDetectorInterface &pivot_detector, Grammar &grammar)
		: pivot_detector_(pivot_detector), grammar_(grammar) { }

	std::vector<std::string> ShiftReduceAlgorithm::Parse(std::string string) const;

private:
	void Reduce(std::vector<char> &stack, std::vector<char> &input, int pivot_length, char substitute, std::vector<std::string> &deriviation) const;

	PivotDetectorInterface &pivot_detector_;
	Grammar &grammar_;
};