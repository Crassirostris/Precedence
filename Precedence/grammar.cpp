#include <algorithm>
#include <string>
#include <sstream>

#include "grammar.h"

bool Grammar::ContainsRuleFor(std::string string, char& substisution) const {
	auto r = std::find_if(rules.begin(), rules.end(),
		[string](Rule rule) { return rule.product == string; });
	if (r == rules.end())
		return false;
	substisution = r->nonterminal;
	return true;
}

Grammar InputStreamGrammarSerializer::Deserialize(std::istream& input) {
	std::string line;
	if (!std::getline(input, line) || line.length() < 1)
		throw std::invalid_argument("Failed to read axiom");

	Grammar grammar;
	grammar.axiom = line[0];
	while (std::getline(input, line) && line.length() > 2) {
		std::istringstream in(line);
		char nonterminal;
		std::string product;
		if (!(in >> nonterminal >> product))
			throw std::invalid_argument("Failed to read rule " + line);
		grammar.rules.push_back(Rule(nonterminal, product));
	}

	for (auto &rule : grammar.rules)
		grammar.nonterminals.insert(rule.nonterminal);
	for (auto &rule : grammar.rules)
		for (auto ch : rule.product)
			if (grammar.nonterminals.find(ch) == grammar.nonterminals.end())
				grammar.terminals.insert(ch);

	return grammar;
}