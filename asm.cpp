#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <set>
#include <optional>
#include <vector>

using namespace std;

enum instruction
{
    READ,
    WRITE,
    DUP,
    MUL,
    ADD,
    SUB,
    GT,
    LT,
    EQ,
    JMPZ,
    PUSH,
    POP,
    ROT
};

// keep aligned with `instruction` enum.
// computing a reverse of the `instructions` map instead
// could be worth it if they were more instructions.
string instruction_name[] = {
    "READ",
    "WRITE",
    "DUP",
    "MUL",
    "ADD",
    "SUB",
    "GT",
    "LT",
    "EQ",
    "JMPZ",
    "PUSH",
    "POP",
    "ROT"};

map<string, instruction> instructions = {
    {instruction_name[READ], READ},
    {instruction_name[WRITE], WRITE},
    {instruction_name[DUP], DUP},
    {instruction_name[MUL], MUL},
    {instruction_name[ADD], ADD},
    {instruction_name[SUB], SUB},
    {instruction_name[GT], GT},
    {instruction_name[LT], LT},
    {instruction_name[EQ], EQ},
    {instruction_name[JMPZ], JMPZ},
    {instruction_name[PUSH], PUSH},
    {instruction_name[POP], POP},
    {instruction_name[ROT], ROT}};

struct command
{
    size_t line_number;
    instruction instruction;
    optional<int32_t> value;
};

set<instruction> value_instructions = {PUSH, POP, ROT};

// vector allows us to use the `rotate` algorithm
// ensure 32-bits integer using cstdint `int32_t`
struct custom_stack : vector<int32_t>
{
    int pop()
    {
        int value = this->back();
        this->pop_back();
        return value;
    }

    void print()
    {
        cout << "[";
        for (auto value : *this)
            cout << value << " ";
        cout << "]" << endl;
    }
};

template <typename _IStream>
int32_t read_value(_IStream &input)
{
    int32_t value;
    input >> value;
    if (input.fail())
    {
        throw overflow_error("Value too high");
    }
    return value;
}

template <typename _IStream, typename _It>
void read_program(_IStream &input, _It it)
{
    size_t line_number = 0;
    while (input.rdstate() == ios_base::goodbit)
    {
        command c;
        input >> c.line_number;
        if (c.line_number != line_number)
        {
            throw runtime_error(
                "Wrong line number " + to_string(c.line_number) + " expected " + to_string(line_number));
        }
        try
        {
            string instruction;
            input >> instruction;
            c.instruction = instructions.at(instruction);
        }
        catch (const out_of_range &)
        {
            throw runtime_error("Unknown instruction at line " + to_string(line_number));
        }
        if (value_instructions.find(c.instruction) != value_instructions.end())
        {
            c.value = read_value(input);
        }
        else
        {
            c.value = nullopt;
        }
        it = c;
        line_number++;
    }
    if (input.rdstate() != ios_base::eofbit)
    {
        throw runtime_error("Error parsing program at line " + to_string(line_number));
    }
}

// verbose = true can be used for debug
// it should be linked to a proper command line option
// but, to this effect, using a proper CLI library would make more sense.
template <typename _Container>
void execute_program(const _Container &program, bool verbose = false)
{
    custom_stack stack;
    // use `line_num` for iteration so it can be modified for the JMPZ instruction
    for (size_t line_num = 0; line_num < program.size(); ++line_num)
    {
        const command &line = program[line_num];
        if (verbose)
        {
            cout << line_num << " " << instruction_name[line.instruction];
            if (line.value)
            {
                cout << " " << line.value.value();
            }
            cout << endl;
        }
        switch (line.instruction)
        {
        case READ:
        {
            stack.push_back(read_value(cin));
            break;
        }
        case WRITE:
        {
            cout << stack.pop() << endl;
            break;
        }
        case DUP:
        {
            stack.push_back(stack.back());
            break;
        }
        case MUL:
        {
            stack.push_back(stack.pop() * stack.pop());
            break;
        }
        case ADD:
        {
            stack.push_back(stack.pop() + stack.pop());
            break;
        }
        case SUB:
        {
            stack.push_back(stack.pop() - stack.pop());
            break;
        }
        case GT:
        {
            stack.push_back(stack.pop() > stack.pop());
            break;
        }
        case LT:
        {
            stack.push_back(stack.pop() < stack.pop());
            break;
        }
        case EQ:
        {
            stack.push_back(stack.pop() == stack.pop());
            break;
        }
        case JMPZ:
        {
            size_t address = stack.pop();
            if (stack.pop() == 0)
            {
                line_num = address - 1;
            }
            break;
        }
        case PUSH:
        {
            stack.push_back(line.value.value());
            break;
        }
        case POP:
        {
            for (size_t count = line.value.value(); count > 0; --count)
            {
                stack.pop();
            }
            break;
        }
        case ROT:
        {
            rotate(stack.end() - line.value.value(), stack.end() - 1, stack.end());
            break;
        }
        default:
            throw runtime_error("Unknown instruction number");
        }
        if (verbose)
        {
            stack.print();
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " "
             << "[filename]" << endl;
        return 1;
    }
    vector<command> program;
    try
    {
        ifstream infile(argv[1]);
        read_program(infile, back_inserter(program));
        execute_program(program);
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
