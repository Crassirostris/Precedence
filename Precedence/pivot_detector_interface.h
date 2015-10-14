#pragma once

#include <vector>

class PivotDetectorInterface {
public:
	virtual ~PivotDetectorInterface() { }

	virtual bool IsPivot(std::vector<char> &stack, std::vector<char> &input, int &length, char &substisution) const = 0;
};
