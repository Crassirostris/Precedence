#pragma once

#include <vector>
#include <unordered_set>
#include <string>

struct Rule {	
	Rule(char nonterminal, std::string product) : nonterminal(nonterminal), product(product) { }

	char nonterminal;
	std::string product;
};

struct Grammar {
	bool ContainsRuleFor(std::string string, char &substisution) const;

	std::unordered_set<char> terminals;
	std::unordered_set<char> nonterminals;
	std::vector<Rule> rules;
	char axiom;
};

class InputStreamGrammarSerializer {
public:
	static Grammar InputStreamGrammarSerializer::Deserialize(std::istream& input);
};