### Command Line Interface

Program runs with following command line parameters:

`./Precedence.exe [<input_file> [<output_file>]]`

Both parameters are optional.

* `<input_file>` — input filename, by default "input.txt"
* `<output_file>` — output filename, by default "output.txt"

### Input Format

```
<axiom> <number_of_rules> <terminals> <nonterminals>
<rule_1_left_side> <rule_1_right_side>
...
<rule_n_left_side> <rule_n_right_side>

<string_to_parse>
```

Example:
```
E 6 ()*+x ETP
E E+T
E T
T T*P
T P
P (E)
P x

(x+x)*x+x*(x+(x*x))
```