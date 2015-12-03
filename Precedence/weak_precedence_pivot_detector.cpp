#include <stdexcept>
#include <set>
#include <sstream>

#include "string_helper.h"
#include "weak_precedence_pivot_detector.h"

WeakPrecedencePivotDetector WeakPrecedencePivotDetector::CreateFromGrammar(Grammar &grammar) {
	std::unordered_map<char, std::unordered_set<std::string>> rules_bucketed;
	for (auto j = grammar.rules.begin(); j != grammar.rules.end(); ++j)
		rules_bucketed[j->nonterminal].insert(j->product);
	auto head_plus = CreateHeadPlus(grammar, rules_bucketed);
	auto tail_plus = CreateTailPlus(grammar, rules_bucketed);
	auto head = CreateHead(grammar, head_plus);

	std::unordered_map<char, std::unordered_map<char, std::set<Precedence>>> precedence_candidates;
	for (auto j = grammar.rules.begin(); j != grammar.rules.end(); ++j) {
		for (auto i = j->product.begin(); i + 1 != j->product.end(); ++i) {
			precedence_candidates[*i][*(i + 1)].insert(Equal);
			for (auto k = head_plus[*(i + 1)].begin(); k != head_plus[*(i + 1)].end(); ++k) {
				precedence_candidates[*i][*k].insert(Less);
			}
			for (auto k = tail_plus[*i].begin(); k != tail_plus[*i].end(); ++k)
				for (auto l = head[*(i + 1)].begin(); l != head[*(i + 1)].end(); ++l)
					precedence_candidates[*k][*l].insert(Greater);
		}
	}

	std::unordered_map<char, std::unordered_map<char, Precedence>> precedence_table;
	for (auto i = precedence_candidates.begin(); i != precedence_candidates.end(); ++i)
		for (auto j = i->second.begin(); j != i->second.end(); ++j) {
			auto set = j->second;
			if (set.size() == 1) {
				precedence_table[i->first][j->first] = *set.begin();
			} else if (set.size() > 1) {
				if (set.size() == 2 && set.find(Less) != set.end() && set.find(Equal) != set.end())
					precedence_table[i->first][j->first] = LessEqual;
				else
					throw std::invalid_argument(std::string("Not a Wirth–Weber precedence grammar, symbols ")
						+ i->first + " and " + j->first + " have multiple relations");
			}
		}

    auto result = WeakPrecedencePivotDetector(precedence_table, grammar);

    if (!result.IsSimplePrecedence() && !result.AdditionalRuleSatisfies())
        throw std::invalid_argument(std::string("Not a weak precedence grammar"));

    return result;
}

bool WeakPrecedencePivotDetector::IsPivot(std::vector<char> &stack, std::vector<char> &input, int &length, char &substisution) const {
	if (stack.size() == 0)
		return false;
	if ((input.size() > 0 && GetPrecedence(stack.back(), input.back()) == Greater)
			|| (input.size() == 0 && (stack.size() > 1 || stack.back() != grammar_.axiom))) {
		FindPivot(stack, length, substisution);
		return true;
	}
	return false;
}

std::string WeakPrecedencePivotDetector::Serialize() {
    std::ostringstream ss;
    ss << (IsSimplePrecedence() ? "Simple" : "Weak") << " precedence grammar" << std::endl;
    std::set<char> alphabet;
    for (char ch : grammar_.terminals)
        alphabet.insert(ch);
    for (char ch : grammar_.nonterminals)
        alphabet.insert(ch);
    ss << "  ";
    for (auto j = alphabet.begin(); j != alphabet.end(); ++j)
        ss << ' ' << *j;
    ss << std::endl;
    for (auto j = alphabet.begin(); j != alphabet.end(); ++j) {
        ss << ' ' << *j;
        for (auto k = alphabet.begin(); k != alphabet.end(); ++k) {
            if (precedence_table_.find(*j) == precedence_table_.end()
                    || precedence_table_[*j].find(*k) == precedence_table_[*j].end()) {
                ss << "  ";
                continue;
            }
            switch (precedence_table_[*j][*k]) {
                case None:
                    ss << "  ";
                    break;
                case Less:
                    ss << " <";
                    break;
                case Greater:
                    ss << " >";
                    break;
                case Equal:
                    ss << " =";
                    break;
                case LessEqual:
                    ss << " ?";
                    break;
            }
        }
        ss << std::endl;
    }
    return ss.str();
}

bool WeakPrecedencePivotDetector::IsSimplePrecedence() {
    for (auto j = precedence_table_.begin(); j != precedence_table_.end(); ++j)
        for (auto k = j->second.begin(); k != j->second.end(); ++k)
            if (k->second == LessEqual)
                return false;
    return true;
}

bool WeakPrecedencePivotDetector::AdditionalRuleSatisfies() {
    for (auto &rule : grammar_.rules) {
        for (auto &other_rule: grammar_.rules)
            if (StringHelper::EndsWith(other_rule.product, rule.product)
                    && rule.product.length() < other_rule.product.length()) {
                auto x = other_rule.product[other_rule.product.length() - rule.product.length() - 1];
                auto row = precedence_table_.find(x);
                if (row == precedence_table_.end())
                    continue;
                auto precedence_it = row->second.find(rule.nonterminal);
                if (precedence_it == row->second.end())
                    continue;
                if (precedence_it->second != None && precedence_it->second != Greater)
                    return false;
            }
    }
    return true;
}

std::unordered_map<char, std::unordered_set<char>> WeakPrecedencePivotDetector::CreateHeadPlus(Grammar& grammar,
		std::unordered_map<char, std::unordered_set<std::string>>& rules_bucketed) {
	std::unordered_map<char, std::unordered_set<char>> head_plus;
	for (auto j = grammar.rules.begin(); j != grammar.rules.end(); ++j)
		head_plus[j->nonterminal].insert(j->product.front());

	bool changed = true;
	while (changed) {
		changed = false;
		for (auto i = head_plus.begin(); i != head_plus.end(); ++i) {
			std::unordered_set<char> to_add;
            for (auto j = i->second.begin(); j != i->second.end(); ++j) {
                if (rules_bucketed.find(*j) == rules_bucketed.end())
                    continue;
                for (auto r = rules_bucketed[*j].begin(); r != rules_bucketed[*j].end(); ++r)
                    to_add.insert(r->front());
            }
			for (auto j = to_add.begin(); j != to_add.end(); ++j) {
				if (i->second.find(*j) == i->second.end()) {
					i->second.insert(*j);
					changed = true;
				}
			}
		}
	}

	return head_plus;
}

std::unordered_map<char, std::unordered_set<char>> WeakPrecedencePivotDetector::CreateTailPlus(Grammar& grammar,
		std::unordered_map<char, std::unordered_set<std::string>>& rules_bucketed) {
	std::unordered_map<char, std::unordered_set<char>> tail_plus;
	for (auto j = grammar.rules.begin(); j != grammar.rules.end(); ++j)
		tail_plus[j->nonterminal].insert(j->product.back());

	bool changed = true;
	while (changed) {
		changed = false;
		for (auto i = tail_plus.begin(); i != tail_plus.end(); ++i) {
			std::unordered_set<char> to_add;
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
				for (auto r = rules_bucketed[*j].begin(); r != rules_bucketed[*j].end(); ++r)
					to_add.insert(r->back());
			for (auto j = to_add.begin(); j != to_add.end(); ++j)
				if (i->second.find(*j) == i->second.end()) {
					i->second.insert(*j);
					changed = true;
				}
		}
	}

	return tail_plus;
}

std::unordered_map<char, std::unordered_set<char>> WeakPrecedencePivotDetector::CreateHead(Grammar &grammar,
		std::unordered_map<char, std::unordered_set<char>> &head_plus) {
	std::unordered_map<char, std::unordered_set<char>> head;
	for (auto j = grammar.terminals.begin(); j != grammar.terminals.end(); ++j)
		head[*j] = std::unordered_set<char>{ *j };
	for (auto j = grammar.nonterminals.begin(); j != grammar.nonterminals.end(); ++j)
		head[*j] = std::unordered_set<char>();
	for (auto j = head_plus.begin(); j != head_plus.end(); ++j) {
		auto set = head.find(j->first);
		for (auto k = j->second.begin(); k != j->second.end(); ++k)
			if (grammar.terminals.find(*k) != grammar.terminals.end())
				set->second.insert(*k);
	}
	return head;
}

void WeakPrecedencePivotDetector::FindPivot(std::vector<char> &stack, int& length, char& substisution) const {
	std::string accumulated_string;
	for (auto j = stack.rbegin(); j != stack.rend(); ++j) {
		accumulated_string += *j;
		if (j + 1 != stack.rend() && GetPrecedence(*(j + 1), *j) == Less)
			break;
	}
	
	std::reverse(accumulated_string.begin(), accumulated_string.end());
	for (auto j = accumulated_string.begin(); j != accumulated_string.end(); ++j) {
		if ((j == accumulated_string.begin() || GetPrecedence(*(j - 1), *j) == LessEqual)
				&& grammar_.ContainsRuleFor(accumulated_string.substr(j - accumulated_string.begin()), substisution)) {
			length = accumulated_string.length() - (j - accumulated_string.begin());
			return;
		}
	}

	throw std::invalid_argument("Failed to find pivot, invalid input string");
}

Precedence WeakPrecedencePivotDetector::GetPrecedence(const char left, const char right) const {
	if (precedence_table_.find(left) == precedence_table_.end())
		throw std::invalid_argument(std::string("Invalid symbol encountered: ") + left);
	auto precidence_table_left = precedence_table_.at(left);
	if (precidence_table_left.find(right) == precidence_table_left.end())
		throw std::invalid_argument(std::string("Invalid sequence of symbols encountered: ") + left + right);
	return precidence_table_left[right];
}