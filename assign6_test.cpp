/* 
 * assign6_test.cpp
 * Assignment 6 test runner
 */
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::cout;
using std::endl;

#include "assign6.hpp"

template<typename T>
void print(const std::vector<T>& vs) {
    for(auto v : vs)
        cout << v << ",";    
}

bool test_tokenize() {

    cout << "Testing tokenization...\n";

    // Tokenize empty string (into no tokens)
    auto t1 = tokenize(""); 
    if(!t1.empty()) {
        cout << "FAILED: tokenize(\"\") should result in 0 tokens.\n";
        return false;
    }

    // Tokenize each token type by itself, with some spaces thrown in.
    vector<string> single_tokens = {
        "a_c", 
        "a_c   ",
        "   a_c",
        "   a_c   ",
        "123",
        "   123",
        "123   ",
        "   123   ",
        " + ", " * ", " / ", " - ", " ( ", " ) "
    };
    for(auto t : single_tokens) {
        auto t2 = tokenize(t);
        if(t2.size() != 1) {
            cout << "FAILED: \"" << t << "\" should tokenize into one token.\n";
            print(t2); cout << endl;
            return false;
        }
    }

    // Test all possible pairs of tokens
    vector<string> tokens = {"a_b", "123", "+", "*", "-", "/", "-", "(", ")"};
    for(auto tok1 : tokens) {
        for(auto tok2 : tokens) {

            // Don't test any token type with itself
            if(tok1 == tok2)
                continue; 

            auto ts = tokenize(tok1 + tok2);
            if(ts.size() != 2) {
                cout << "FAILED: \"" << tok1 << tok2 << "\" should tokenize into two tokens.\n";
                print(ts); cout << endl;
                return false;
            }

            if(ts.front() != tok1) {
                cout << "FAILED: First token of \"" << tok1 << tok2 << "\" should be "
                     << "\"" << tok1 << "\"\n";
                print(ts); cout << endl;
                return false;
            }
            if(ts.back() != tok2) {
                cout << "FAILED: Second token of \"" << tok1 << tok2 << "\" should be "
                     << "\"" << tok2 << "\"\n";
                print(ts); cout << endl;
                return false;
            }

        }
    }
    return true;
}

enum class tree_type {
    NUM,
    VAR,
    PAREN,
    MINUS,
    FUNC,
    OPERATOR,
    ERROR
};

tree_type get_type(exp* a) {
    if(dynamic_cast<exp_num*>(a))
        return tree_type::NUM;
    else if(dynamic_cast<exp_var*>(a))
        return tree_type::VAR;
    else if(dynamic_cast<exp_paren*>(a))
        return tree_type::PAREN;
    else if(dynamic_cast<exp_minus*>(a))
        return tree_type::MINUS;
    else if(dynamic_cast<exp_func*>(a))
        return tree_type::FUNC;
    else if(dynamic_cast<exp_op*>(a))
        return tree_type::OPERATOR;
    else
        return tree_type::ERROR;
}

/* compare_trees(a,b)
   Compares expression trees for equality.
*/
bool compare_trees(exp* a, exp* b) {
    if(get_type(a) != get_type(b))
        return false;
    else {
        exp_func* af;
        exp_func* bf;
        exp_op* ao;
        exp_op* bo;

        switch(get_type(a)) {
            case tree_type::NUM:
                return dynamic_cast<exp_num*>(a)->value == 
                       dynamic_cast<exp_num*>(b)->value;

            case tree_type::VAR:
                return dynamic_cast<exp_var*>(a)->name == 
                       dynamic_cast<exp_var*>(b)->name;

            case tree_type::PAREN:
                return compare_trees(dynamic_cast<exp_paren*>(a)->inner,
                                     dynamic_cast<exp_paren*>(b)->inner);

            case tree_type::MINUS:
                return compare_trees(dynamic_cast<exp_minus*>(a)->inner,
                                     dynamic_cast<exp_minus*>(b)->inner);

            case tree_type::FUNC:
                af = dynamic_cast<exp_func*>(a);
                bf = dynamic_cast<exp_func*>(b);

                return af->name == bf->name && 
                       compare_trees(af->arg, bf->arg);

            case (tree_type::OPERATOR):
                ao = dynamic_cast<exp_op*>(a);
                bo = dynamic_cast<exp_op*>(b);

                return ao->op == bo->op && 
                       compare_trees(ao->left,bo->left) &&
                       compare_trees(ao->right,bo->right);

            default:
                return false;
        }
    }
}

exp* parse(std::string s) {
    std::vector<std::string> tokens = tokenize(s);
    int prec = 0;
    return parse(tokens.begin(), tokens.end(), prec);
}

bool test_parse() {
    cout << "Testing parsing\n  (invalid expressions...)\n";

    vector<string> bad_exps = {
        "",
        "a 1",
        "1 1",
        "a b",
        "a + + 2",
        "a_b + 3", // Variable names cannot include _
        "()",
        "a + ",
        "- + a",
        "sin(1 +)",
        "sin()"
    };

    for(string s : bad_exps)
        if(parse(s) != nullptr) {
            cout << "FAILED: '" << s << "' should not parse to a tree.\n";
            return false;
        }

    cout << "  (valid expressions...)\n";

    // Test some simple expression to make sure the tree root is of the 
    // correct sub-class.
    vector<string> root_exp = {
        "1",    // num
        "a",    // var
        "(1)",  // paren
        "-a",   // minus
        "f(a)", // function    
    };

    vector<tree_type> root_types = {
        tree_type::NUM,
        tree_type::VAR,
        tree_type::PAREN,
        tree_type::MINUS,
        tree_type::FUNC
    };

    vector<exp*> root_trees = {
        new exp_num(1),
        new exp_var("a"),
        new exp_paren(new exp_num(1)),
        new exp_minus(new exp_var("a")),
        new exp_func("f", new exp_var("a"))
    };

    for(unsigned i = 0; i < root_exp.size(); ++i) {

        exp* e = parse(root_exp.at(i));

        if(e == nullptr) {
            cout << "FAILED: '" << root_exp.at(i) << "' did not parse to an expression tree.\n";
            return false;
        }

        if(root_types.at(i) != get_type(e)) {
            cout << "FAILED: '" << root_exp.at(i) << "'s parse tree has the wrong root node type.\n";
            return false;
        }

        if(!compare_trees(e, root_trees.at(i))) {
            cout << "FAILED: got the wrong tree for '" << root_exp.at(i) << "'.\n";
            cout << "  expected: ";
            root_trees.at(i)->print();
            cout << " got ";
            e->print();
            cout << endl;

            return false;
        }
    }

    // Test some compound expressions
    vector<string> cmp_str = {
        "1 + 2",
        "-a",
        "-1 - 2",
        "(a * b)",
        "sin(2 * x)",
        "1 + 2 * 3",
        "1 / 2 - 3"
    };

    vector<exp*> cmp_exps = {
        new exp_op('+', new exp_num(1), new exp_num(2)),
        new exp_minus(new exp_var("a")),
        new exp_op('-', 
            new exp_minus(new exp_num(1)),
            new exp_num(2)),
        new exp_paren(new exp_op('*', new exp_var("a"), new exp_var("b"))),
        new exp_func("sin", new exp_op('*', new exp_num(2), new exp_var("x"))),
        new exp_op('+', new exp_num(1),
            new exp_op('*', new exp_num(2), new exp_num(3))),
        new exp_op('-', 
            new exp_op('/', new exp_num(1), new exp_num(2)),
            new exp_num(3))
    };

    for(unsigned i = 0; i < cmp_str.size(); ++i) {
        exp* e = parse(cmp_str.at(i));
        if(!e) {
            cout << "FAILED: " << cmp_str.at(i) << " should parse.\n";
            return false;
        }

        if(!compare_trees(e, cmp_exps.at(i))) {
            cout << "FAILED: " << cmp_str.at(i) << " did not parse correctly.\n";
            cout << "  expected "; cmp_exps.at(i)->print(); cout << endl;
            cout << "  found: ";   e->print(); cout << endl;

            return false;
        }
    }

    return true;
}

int main(int argc, char**) {

    if(argc > 1) { 
        while(true) {
            // Interactive mode
            string input;
            cout << "> ";
            std::getline(std::cin, input);

            exp* e = parse(input);
            if(e) {
                cout << "= ";
                e->print();
                cout << endl;
            }
            else
                cout << "  (could not parse)\n";
        }
    }
    else {
        // Run tests as usual
        std::cout << "--- Starting tests ---\n";
        if(test_tokenize() && test_parse()) {
            cout << "--- All tests completed successfully! ---\n";
            return 0;
        }
        else
            return 1;
    }
}