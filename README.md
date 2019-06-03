# CPP-lexical-analysis
In this project, I implemented a tokenization function for a hypothetical language with the following syntactical elements:

Floating point numbers in the following formats: 10, 0.1, -10, 10.12. A number consists of an integer part, followed (optionally) by a decimal point and a decimal part. Note that the integer part is required – .1 is not a valid floating-point number – and likewise, if a decimal point is present that at least one fractional digit must follow it – 0. is also not a valid floating-point number.

Variables, which must start with an alphabetic character but may be followed by alphanumeric characters.

The operators +-*/ and parentheses (), each of which should be a single token.

The comparison operators < <= > and >=.

You will need nine states to tokenize this language:

INTEGER: state for the integer part of a FP number.

DECIMAL: state for the decimal point of a FP number.

FRAC: state for the fractional part of a FP number.

ALPHA: state for the first character of a variable.

ALPHANUM: state for the following characters of a variable.

OP_PAREN: state for the operator/paren tokens

LT1, LT2, GT1, GT2: states for the first/second characters in less-than (or equal to), greater-than (or equal to).

SPACE: sequences of whitespace.
