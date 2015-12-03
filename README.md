### Command Line Interface

Program runs with following command line parameters:

`./Precedence.exe [-h|--help] [<input_filename> [<output_filename>]]`

All parameters are optional.

* `-h|--help` — show help.
* `<input_file>` — input filename, by default "input.txt"
* `<output_file>` — output filename, by default standard output

### Input Format

```
<axiom>
<rule_1_left_side> <rule_1_right_side>
...
<rule_n_left_side> <rule_n_right_side>
_empty_string_
<string_to_parse_1>
<string_to_parse_2>
...
```

Example:
```
E
E E+T
E T
T T*P
T P
P (E)
P x

(x+x)*x+x*(x+(x*x))
x*x
```