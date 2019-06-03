/*
 * parser.cpp
 * Combined header and source file template for assignment 6.
 *Combined due to unknown errors and works this way.
 *
 * You can modify the classes here. You will also need to add a .cpp containing
 * the implementations of the two required functions.
 .cpp file is this file
 * 
 * You can add other methods/members to the classes if you need to, but you
 * have to keep the existing ones.
 */

#include <string>
#include <vector>
#include <iostream>
#include "template.hpp"

using namespace std;

/*
struct exp { 
    // You will have to implement the print method on all the subclasses.
    virtual void print() = 0; 
};

// Precedence 100
struct exp_num : public exp {
    int value;

    exp_num(int v) : value(v) { }

    void print(){ cout << value;}
};


// Precedence 100
struct exp_var : public exp {
    std::string name;

    exp_var(std::string n) : name(n) { }

    void print(){ cout << name;}
};


// Precdence 100
struct exp_paren : public exp {
    exp* inner;

    exp_paren(exp* i) : inner(i) { }

    void print(){ cout << "( "; inner->print(); cout << ")";}
};

// Precedence 100 
struct exp_minus : public exp {
    exp* inner;

    exp_minus(exp* i) : inner(i) { }

    void print(){ cout << "-"; inner->print();}
};

// Precedence 100
struct exp_func : public exp {
    std::string name;
    exp* arg;

    exp_func(std::string n, exp* a) : name(n), arg(a) { }

    void print(){ cout << name;
    			  cout << "-->";
    			  cout << "(";
    			  arg->print();
    			  cout << ")";}
};

// Precedence:
// +  50
// -  50
// *  75
// /  75
struct exp_op : public exp {
    char op;
    exp* left;
    exp* right;

    exp_op(char o, exp* l, exp* r) : op(o), left(l), right(r) { }

    void print(){ left->print();
    			  cout << op;
    			  right->print();}
};
*/
// *** You need to implement the following two functions ***

bool is_num(string s) {
	return s.find_first_not_of("0123456789") == string::npos;
}

bool is_var(string s) {
	return s.front() >= 'a' && s.front() <= 'z' && s.find('_') >= s.length();
}

int op_prec(string op) {
	if (op == "+" || op == "-")
		return 50;
	else if (op == "*" || op == "/")
		return 75;
	else
		return 0;
}

bool is_op(string s) {
	return s == "+" || s == "-" || s == "*" || s == "/";
}

bool is_fname(string s) {
	return s.front() >= 'a' && s.front() <= 'z';
}

int find_char_state(char c) {
	const string token_chars[6] = {
		" \t\n",
		"0123456789",
		"+-*/",
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_",
		"(",
		")"
	};
	for (int i = 0; i < 6; i++)
		if (token_chars[i].find_first_of(c) != string::npos)
			return i;

	return -1;
}

vector<string> tokenize(string s) {
	vector<string> tokens;
	const int space = 0;
	const int operation = 2;
	int mode = space;
	string token;
	for (char c : s) {
		if (mode == space) {
			int new_mode = find_char_state(c);

			if (new_mode != mode) {	// runs if the mode == space
				token.push_back(c);
				mode = new_mode;
			}
			else {
				continue;
			}
		}
		else {	// finds the type of character
			int new_mode = find_char_state(c);
			if (new_mode != mode || mode == operation) {
				tokens.push_back(token);
				token.clear();
				if ((new_mode != space && new_mode != mode) || new_mode == operation) {
					token.push_back(c);
				}
				mode = new_mode;
			}
			else {
				token.push_back(c);
			}
		}
	}
	if (!token.empty()) {
		tokens.push_back(token);
	}
	return tokens;
}

exp* parse(vector<string>::iterator start, vector<string>::iterator finish, int& prec) {
	if (0 == finish - start || start > finish || start == finish) {
		return nullptr;
	}
	else if (1 == finish - start) {
		if (is_num(*start)) {
			prec = 100;
			return new exp_num{ stoi(*start) };
		}
		else if (is_var(*start)) {
			prec = 100;
			return new exp_var{ *start };
		}
		else
			return nullptr;
	}
	else {
		if (*start == "(" && *(finish - 1) == ")") {
			exp* e = parse(start + 1, finish - 1, prec);
			if (e) {
				prec = 100;
				return new exp_paren{ e };
			}
		}
		if (is_fname(*start) && *(start + 1) == "(" && *(finish - 1) == ")") {
			exp* e = parse(start + 2, finish - 1, prec);
			if (e) {
				prec = 100;
				return new exp_func{ *start, e };
			}
		}
		if (*start == "-") {
			int iprec;
			exp* e = parse(start + 1, finish, iprec);
			if (e && iprec >= 90) {
				prec = 90;
				return new exp_minus{ e };
			}
		}
		for (vector<string>::iterator operation = start + 1; operation <= finish - 2; ++operation) {
			int lprec, rprec;
			exp* l = parse(start, operation, lprec);
			exp* r = parse(operation + 1, finish, rprec);
			if (is_op(*operation) && l != nullptr && r != nullptr && op_prec(*operation) <= lprec && op_prec(*operation) <= rprec) {
				prec = op_prec(*operation);
				return new exp_op{ (*operation)[0], l, r };
			}
		}
		return nullptr;
	}
}
