#include <iostream>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <math.h>


// Token stuff
// Token “kind” values:
char const number = '8';
char const quit = 'q';
char const print = ';';
char const pi = 'p';         // Represent π
char const func = 'f';
class token
{
    char kind_;       // what kind of token
    double value_;    // for numbers: a value
    std::string name_;

public:
    token(char ch) : kind_(ch), value_(0) {}
    token(double val) : kind_(number), value_(val) {}
    token(char ch, std::string const& name) : kind_(ch), name_(name) {}

    char kind() const { return kind_; }
    double value() const { return value_; }
    std::string name() const { return name_; }
};

// User interaction strings:
std::string const prompt = "> ";
std::string const result = "= ";    // indicate that a result follows

class token_stream
{
    // representation: not directly accessible to users:
    bool full;       // is there a token in the buffer?
    token buffer;    // here is where we keep a Token put back using
    // putback()
public:
    // user interface:
    token get();            // get a token
    void putback(token);    // put a token back into the token_stream
    void ignore(char c);    // discard tokens up to and including a c

    // constructor: make a token_stream, the buffer starts empty
    token_stream()
            : full(false)
            , buffer('\0')
    {}
};

// single global instance of the token_stream
token_stream ts;

void token_stream::putback(token t)
{
    if (full)
        throw std::runtime_error("putback() into a full buffer");
    buffer = t;
    full = true;
}

token token_stream::get()    // read a token from the token_stream
{
    // check if we already have a Token ready
    if (full)
    {
        full = false;
        return buffer;
    }

    // note that >> skips whitespace (space, newline, tab, etc.)
    char ch;
    std::cin >> ch;

    switch (ch)
    {
        case '(':
        case ')':
        case ';':
        case 'q':
        case '+':
        case '-':
        case '*':
        case '/':
            return token(ch);    // let each character represent itself
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            std::cin.putback(ch);    // put digit back into the input stream
            double val;
            std::cin >> val;    // read a floating-point number
            return token(val);
        }
        default:
            std::cin.putback(ch);
            std::string s;
            std::cin >> s;
            if (s == "pi") return token(pi, s);
            if (s == "sin" || s == "cos" || s == "tan") return token(func, s);
            throw std::runtime_error("Bad token");
    }
}

// discard tokens up to and including a c
void token_stream::ignore(char c)
{
    // first look in buffer:
    if (full && c == buffer.kind())    // && means 'and'
    {
        full = false;
        return;
    }
    full = false;    // discard the contents of buffer

    // now search input:
    char ch = 0;
    while (std::cin >> ch)
    {
        if (ch == c)
            break;
    }
}

// declaration so that primary() can call expression()
double expression();

double primary()    // Number or ‘(‘ Expression ‘)’
{
    token t = ts.get();
    switch (t.kind()) {
        case '(': {
            double d = expression();
            t = ts.get();
            if (t.kind() != ')') throw std::runtime_error("')' expected");
            return d;
        }
        case number:
            return t.value();
        case pi:
            return M_PI;
        case func: {
            double arg = primary(); // Get the argument for the function
            if (t.name() == "sin") return std::sin(arg);
            if (t.name() == "cos") return std::cos(arg);
            if (t.name() == "tan") return std::tan(arg);
            throw std::runtime_error("Unsupported function");
        }
        default:
            throw std::runtime_error("Primary expected");
    }
}

// exactly like expression(), but for * and /
double term()
{
    double left = primary();    // get the Primary
    while (true)
    {
        token t = ts.get();    // get the next Token ...
        switch (t.kind())
        {
            case '*':
                left *= primary();
                break;
            case '/':
            {
                double d = primary();
                if (d == 0)
                    throw std::runtime_error("divide by zero");
                left /= d;
                break;
            }
            default:
                ts.putback(t);    // <<< put the unused token back
                return left;      // return the value
        }
    }
}

// read and evaluate: 1   1+2.5   1+2+3.14  etc.
// 	 return the sum (or difference)
double expression()
{
    double left = term();    // get the Term
    while (true)
    {
        token t = ts.get();    // get the next token…
        switch (t.kind())      // ... and do the right thing with it
        {
            case '+':
                left += term();
                break;
            case '-':
                left -= term();
                break;
            default:
                ts.putback(t);    // <<< put the unused token back
                return left;      // return the value of the expression
        }
    }
}

void clean_up_mess()
{
    ts.ignore(print);
}

void calculate()
{
    while (std::cin)
    {
        try
        {
            std::cout << prompt;    // print prompt
            token t = ts.get();

            // first discard all “prints”
            while (t.kind() == print)
                t = ts.get();

            if (t.kind() == quit)
                return;    // ‘q’ for “quit”

            ts.putback(t);

            std::cout << result << expression() << std::endl;
        }
        catch (std::runtime_error const& e)
        {
            std::cerr << e.what() << std::endl;    // write error message
            clean_up_mess();                       // <<< The tricky part!
        }
    }
}
int naiveModulus(int dividend, int divisor) {
    if (divisor == 0) {
        // Division by zero is undefined
        throw std::invalid_argument("Division by zero");
    }
    int quotient = dividend / divisor; // Perform integer division
    int result = dividend - (divisor * quotient); // Calculate the remainder
    return result;
}

std::unordered_map<std::string, double> variables;// store variables

double evaluate_expression(const std::string& expr);// evaluate the expression

double get_number(std::istringstream& iss) {// get the number
    double num;
    iss >> num;// get the number

    return num;
}

double get_term(std::istringstream& iss) {// get the term
    double result = get_number(iss);// get the first number
    char op;// store the operator
    while (iss >> op) {// while there is an operator
        if (op == '*' || op == '/') {// if the operator is * or /
            double num = get_number(iss);// get the next number
            if (op == '*')// multiply the number
                result *= num;// multiply the number
            else if (num == 0)// if the number is zero, throw an exception
                throw std::runtime_error("division by zero");// throw an exception
            else
                result /= num;// divide the number
        } else {
            iss.putback(op);// put the operator back
            break;
        }
    }
    return result;
}

double evaluate_expression(const std::string& expr) {// evaluate the expression
    std::istringstream iss(expr);// create input string stream
    double result = get_term(iss);// get the first term
    char op;// store the operator
    while (iss >> op) {// while there is an operator
        if (op == '+' || op == '-') {// if the operator is + or -
            double term = get_term(iss);// get the next term
            if (op == '+')// add or subtract the term
                result += term;// add the term
            else
                result -= term;// subtract the term
        } else if (op == '%') {// if the operator is %
            double divisor = get_term(iss);// get the divisor
            if (divisor == 0)// if the divisor is zero, throw an exception
                throw std::runtime_error("modulus by zero");// throw an exception
            result = naiveModulus(result, divisor);// calculate the modulus
        }
    }
    return result;
}
int main() {
    std::cout << "Simple Calculator" << std::endl;
    std::cout << "Supported operators: +, -, *, /, %" << std::endl;
    std::cout << "Enter expressions to evaluate or assign variables using the format: variable_name = value" << std::endl;
    std::cout << "For example, to assign the value 5 to variable x, enter: x = 5" << std::endl;

    while (true) {
        std::string input;
        std::cout << "Enter expression (or 'q' to quit): ";
        std::getline(std::cin, input);

        if (input == "q")  // Quit if the input is 'q'
            break;

        size_t assignPos = input.find('=');
        if (assignPos != std::string::npos) {
            std::string variableName = input.substr(0, assignPos);
            std::string valueStr = input.substr(assignPos + 1);

            std::istringstream iss(valueStr);
            char nextChar = iss.peek();
            if (std::isalpha(nextChar)) {
                // Check if the right-hand side is another variable
                std::string anotherVariable;
                iss >> anotherVariable;
                if (variables.find(anotherVariable) != variables.end()) {
                    // If the other variable exists, assign its value
                    variables[variableName] = variables[anotherVariable];
                    std::cout << "Assigned value " << variableName << " to variable " << anotherVariable << std::endl;
                } else {
                    std::cerr << "Error: Variable " << anotherVariable << " not found." << std::endl;
                }
            } else {
                // Otherwise, evaluate and assign the value
                try {
                    double value = evaluate_expression(valueStr);
                    variables[variableName] = value;
                    std::cout << "Assigned value " << value << " to variable " << variableName << std::endl;

                } catch (const std::exception& e) {
                    std::cerr << "Error: Invalid value for variable " << variableName << std::endl;
                }
            }
        } else {
            try {
                for (auto& pair : variables) {
                    std::string variable_name = pair.first;
                    double variable_value = pair.second;
                    size_t pos = input.find(variable_name);
                    while (pos != std::string::npos) {
                        input.replace(pos, variable_name.length(), std::to_string(variable_value));
                        pos = input.find(variable_name, pos + variable_name.length());
                    }
                }

                double result = evaluate_expression(input);
                std::cout << "Result: " << result << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

    return 0;
}