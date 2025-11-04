#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>
#include <cmath>

enum {
    Id = 0,
    AddSub,
    MulDiv,
    Mod,
    Pow,
    Primary
};

class Formula {
public:
    Formula(const std::string& expr) : expression(expr), pos(0) {}

    double evaluate() {
        double result = 0;
        result = parse_expression();
        return result;
    }

    void add_variable(const std::string& name, double value) {
        variables[name] = value;
    }
private:
    std::string expression;
    int pos;
    std::unordered_map<std::string, double> variables;
    int tk;
    int tk_level;
    double val;

    double parse_expression() {
        next();
        return expr(0);
    }

    void next() {
        while (pos < expression.size() && isspace(expression[pos])) ++pos;
        if (pos == expression.size()) {
            tk = 0; // end of expression
            return;
        }
        char ch = expression[pos];
        if (isdigit(ch)) {
            int start = pos;
            while (pos < expression.size() && (isdigit(expression[pos]) || expression[pos] == '.')) ++pos;
            val = std::stod(expression.substr(start, pos - start));
            tk = 'N'; // number token
            tk_level = Id;
        } else if (isalpha(ch) || ch == '_') {
            int start = pos;
            while (pos < expression.size() && (isalnum(expression[pos]) || expression[pos] == '_')) ++pos;
            std::string name = expression.substr(start, pos - start);
            if (variables.find(name) != variables.end()) {
                val = variables[name];
                tk = 'N'; // variable token
                tk_level = Id;
            } else {
                std::cerr << "Unknown variable: " << name << std::endl;
                exit(-1);
            }
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == '%' || ch == '^') {
            tk = ch; // operator token
            tk_level = ch == '+' || ch == '-' ? AddSub :
                       ch == '*' || ch == '/' ? MulDiv :
                       ch == '%' ? Mod :
                       ch == '^' ? Pow :
                       ch == '(' || ch == ')' ? Primary : Id;
            ++pos;
        } else {
            std::cerr << "Unexpected character: " << ch << std::endl;
            exit(-1);
        }
    }

    double expr(int level) {
        if (!tk) {
            std::cerr << "Unexpected end of expression" << std::endl;
            exit(-1);
        }

        double left = 0;
        if (tk == 'N') {
            left = val;
            next();
        } else if (tk == '(') {
            next();
            left = expr(0);
            if (tk != ')') {
                std::cerr << "Expected ')'" << std::endl;
                exit(-1);
            }
            next();
        } else {
            std::cerr << "Bad expression" << std::endl;
            exit(-1);
        }

        while (true) {
            char current_op = tk;

            if (current_op != '+' && current_op != '-' &&
                current_op != '*' && current_op != '/' && current_op != '%' && current_op != '^') {
                break;
            }

            int current_precedence = tk_level;
            if (current_precedence < level) {
                break;
            }
            
            next();
            int next_min_precedence = current_precedence;
            next_min_precedence++;
            double right = expr(next_min_precedence);
            left = apply_operator(current_op, left, right);
        }
        return left;
    }

    double apply_operator(char op, double left, double right) {
        switch (op) {
            case '+': return left + right;
            case '-': return left - right;
            case '*': return left * right;
            case '/': 
                if (right == 0) {
                    std::cerr << "Division by zero" << std::endl;
                    exit(-1);
                }
                return left / right;
            case '%':
                if (right == 0) {
                    std::cerr << "Division by zero" << std::endl;
                    exit(-1);
                }
                return std::fmod(left, right);
            case '^':
                return std::pow(left, right);
            default:
                std::cerr << "Unknown operator: " << op << std::endl;
                exit(-1);
        }
        return 0; // should never reach here
    }
};

int main(int argc, char **argv) {
    Formula formula = Formula("30*n - 2*(50+55) + a%3 + 2^3");
    formula.add_variable("n", 20);
    formula.add_variable("a", 10);
    double result = formula.evaluate();
    std::cout << "Result: " << result << std::endl;
    return 0;
}