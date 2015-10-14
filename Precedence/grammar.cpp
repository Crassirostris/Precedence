#include "grammar.h"
#include <algorithm>
#include <string>

bool Grammar::ContainsRuleFor(std::string string, char& substisution) const {
	auto r = std::find_if(rules.begin(), rules.end(),
		[string](Rule rule) { return rule.product == string; });
	if (r == rules.end())
		return false;
	substisution = r->nonterminal;
	return true;
}

Grammar InputStreamGrammarSerializer::Deserialize(std::istream& input) {
	char axiom;
	std::string termials, nonterminals;
	int rules_count;
	if (!(input >> axiom >> rules_count >> termials >> nonterminals))
		throw std::invalid_argument("Failed to read grammar meta information");
	Grammar grammar;
	grammar.axiom = axiom;
	for (auto j = termials.begin(); j != termials.end(); ++j)
		grammar.terminals.insert(*j);
	for (auto j = nonterminals.begin(); j != nonterminals.end(); ++j)
		grammar.nonterminals.insert(*j);
	for (int i = 0; i < rules_count; ++i) {
		char nonterminal;
		std::string product;
		if (!(input >> nonterminal >> product))
			throw std::invalid_argument("Failed to read rule");
		grammar.rules.push_back(Rule(nonterminal, product));
	}
	return grammar;
}