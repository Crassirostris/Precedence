#pragma once
#include <unordered_map>
#include "pivot_detector_interface.h"
#include "grammar.h"

enum Precedence {
	None,
	Less,
	Greater,
	Equal,
	LessEqual,
	Invalid,
};

class WeakPrecedencePivotDetector : public PivotDetectorInterface {
public:
	static WeakPrecedencePivotDetector CreateFromGrammar(Grammar &grammar);

	bool IsPivot(std::vector<char> &stack, std::vector<char> &input, int &length, char &substisution) const override;

	void AssertCorrectness();

	std::string Serialize();
private:
	bool IsSimplePrecedence();

	bool AdditionalRuleSatisfies();

	WeakPrecedencePivotDetector(std::unordered_map<char, std::unordered_map<char, Precedence>> precedence_table, Grammar grammar)
		: precedence_table_(precedence_table), grammar_(grammar) { }

	static std::unordered_map<char, std::unordered_set<char>> CreateHeadPlus(Grammar &grammar,
		std::unordered_map<char, std::unordered_set<std::string>> &rules_bucketed);
	static std::unordered_map<char, std::unordered_set<char>> CreateTailPlus(Grammar &grammar,
		std::unordered_map<char, std::unordered_set<std::string>> &rules_bucketed);
	static std::unordered_map<char, std::unordered_set<char>> CreateHead(Grammar &grammar,
		std::unordered_map<char, std::unordered_set<char>> &head_plus);

	void FindPivot(std::vector<char> &stack, int &length, char &substisution) const;
	Precedence GetPrecedence(const char left, const char right) const;

	std::unordered_map<char, std::unordered_map<char, Precedence>> precedence_table_;
	Grammar grammar_;
};