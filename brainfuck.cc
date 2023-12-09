#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <unordered_map>

class BrainFuck {
  public:
    BrainFuck(uint64_t stacksize, bool should_print_stack = false)
        : stacksize_(stacksize), should_print_stack_(should_print_stack) {
        initialize_commands_();
        stack_ = new uint8_t[stacksize_];
        stack_top_ = std::next(stack_, stacksize_);
        reset_();
        std::iostream::sync_with_stdio(false);
    }
    BrainFuck() : BrainFuck(16) {}
    ~BrainFuck() {
        if (stack_ != nullptr) {
            delete[] stack_;
        }
    }

  public:
    void command_line() {
        while (true) {
            std::string source;
            std::cout << ">>> ";
            std::cin >> source;
            parse(source);
            is_first_print_stack_ = true;
            std::cout << std::endl;
        }
    }

    void parse(const std::string &source) {
        const uint64_t source_length = source.size();
        for (source_index_ = 0; source_index_ < source_length;
             source_index_++) {
            const char token = source[source_index_];
            if (command_map_.find(token) != command_map_.end()) {
                command_map_.at(token)();
                if (should_print_stack_) {
                    print_stack_(token);
                }
            }
        }
    }

  private:
    void reset_() {
        memset(stack_, 0, stacksize_);
        current_ = stack_;
        std::stack<uint64_t> temp;
        loop_mark_.swap(temp);
        source_index_ = 0;
    }

    void initialize_commands_() {
        std::unordered_map<char, std::function<void(void)>> command_map{
            {'>', [this]() { process_ref_add_(); }},
            {'<', [this]() { process_ref_sub_(); }},
            {'+', [this]() { process_val_add_(); }},
            {'-', [this]() { process_val_sub_(); }},
            {'.', [this]() { process_val_put_(); }},
            {',', [this]() { process_val_get_(); }},
            {'[', [this]() { process_loop_start_(); }},
            {']', [this]() { process_loop_end_(); }},
        };
        command_map_.swap(command_map);
    }

    void process_ref_add_() {
        if (current_ < stack_top_) {
            std::advance(current_, 1);
        } else {
            current_ = stack_;
        }
    }

    void process_ref_sub_() {
        if (current_ == stack_) {
            current_ = stack_top_;
        } else {
            std::advance(current_, -1);
        }
    }

    void process_val_add_() { (*current_)++; }

    void process_val_sub_() { (*current_)--; }

    void process_val_put_() {
        if (should_print_stack_) {
            put_char_ = *current_;
        } else {
            std::cout << static_cast<char>(*current_);
        }
    }

    void process_val_get_() {
        uint8_t value_to_set = 0;
        std::string user_input;
        std::cin >> user_input;
        value_to_set = static_cast<uint8_t>(std::stoi(user_input));
        (*current_) = value_to_set;
    }

    void process_loop_start_() { loop_mark_.push(source_index_); }

    void process_loop_end_() {
        if ((*current_) != 0) {
            source_index_ = loop_mark_.top();
        } else {
            loop_mark_.pop();
        }
    }

    void print_stack_(char token) {
        if (!is_first_print_stack_) {
            std::cout << std::endl;
        }
        is_first_print_stack_ = false;
        std::cout << "[" << token << "] ";
        for (uint64_t i = 0; i < stacksize_; i++) {
            // cout each byte as hex with 2 digits
            if (current_ == std::next(stack_, i)) {
                std::cout << "(";
            }
            std::cout << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<uint32_t>(stack_[i]);
            if (current_ == std::next(stack_, i)) {
                std::cout << ")";
            }
        }
        std::cout << "    " << static_cast<char>(put_char_);
        put_char_ = 0;
    }

  private:
    uint64_t stacksize_{0};
    uint8_t *stack_{nullptr};
    uint8_t *stack_top_{nullptr};
    uint8_t *current_{nullptr};

    std::stack<uint64_t> loop_mark_;
    uint64_t source_index_{0};
    std::unordered_map<char, std::function<void(void)>> command_map_;

    uint8_t put_char_{0};
    bool should_print_stack_{false};
    bool is_first_print_stack_{true};
};

struct Params {
    uint64_t stacksize{16};
    std::string source_file{""};
    bool use_cli{false};
    bool should_print_stack{false};
};

Params parse_params(int32_t argc, char **argv) {
    // -s --stack <size> set stack size
    // -f --file <path> set source file
    // -c --cli use command line mode
    // -d --debug show debug info
    // -h --help show help

    Params params;
    for (int32_t i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stack") == 0) {
            params.stacksize = std::stoull(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0 ||
                   strcmp(argv[i], "--file") == 0) {
            params.source_file = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0 ||
                   strcmp(argv[i], "--cli") == 0) {
            params.use_cli = true;
        } else if (strcmp(argv[i], "-d") == 0 ||
                   strcmp(argv[i], "--debug") == 0) {
            params.use_cli = true;
            params.should_print_stack = true;

        } else if (strcmp(argv[i], "-h") == 0 ||
                   strcmp(argv[i], "--help") == 0) {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -s --stack <size> set stack size" << std::endl;
            std::cout << "  -f --file <path> set source file" << std::endl;
            std::cout << "  -c --cli use command line mode" << std::endl;
            std::cout << "  -h --help show help" << std::endl;
            exit(0);
        } else if (i == 1) {
            params.source_file = argv[i];
        } else {
            // do nothing
        }
    }

    return params;
}

std::string read_source_file(std::string filepath) {
    std::string source_code;
    {
        std::ifstream source_file(filepath);
        if (source_file.is_open()) {
            std::string line;
            while (std::getline(source_file, line)) {
                source_code += line;
            }
        } else {
            throw std::runtime_error("cannot open file");
        }
    }
    return source_code;
}

int32_t main(int32_t argc, char **argv) {
    Params params = parse_params(argc, argv);
    BrainFuck brainfuck(params.stacksize, params.should_print_stack);

    if (params.source_file != "") {
        brainfuck.parse(read_source_file(params.source_file));
    } else if (params.use_cli) {
        brainfuck.command_line();
    }

    return 0;
}