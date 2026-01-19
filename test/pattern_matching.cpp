#include "emp-sh2pc/emp-sh2pc.h"
#include <getopt.h>
using namespace emp;
using namespace std;


// String processing namespace for pattern matching functionality
namespace StringProcessing {
    
  // Convert string to vector of uint8_t values (ASCII)
  std::vector<uint8_t> string_to_integers(const std::string& str) {
    std::vector<uint8_t> result;
    for (char c : str) {
      result.push_back(static_cast<uint8_t>(c));
    }
    return result;
  }
  
  // Break pattern into individual characters
  // Example: "HEL" -> ["H", "E", "L"]
  std::vector<std::string> break_pattern_into_chars(const std::string& pattern) {
    std::vector<std::string> result;
    for (char c : pattern) {
      result.push_back(std::string(1, c));
    }
    return result;
  }
  
  // Create sliding window substrings from text
  // Example: text="HELLO", pattern_size=3 -> [["H","E","L"], ["E","L","L"], ["L","L","O"]]
  std::vector<std::vector<std::string>> create_sliding_windows(const std::string& text, size_t pattern_size) {
    std::vector<std::vector<std::string>> windows;
    
    if (text.length() < pattern_size) {
      return windows; // Return empty if text is shorter than pattern
    }
    
    for (size_t i = 0; i <= text.length() - pattern_size; ++i) {
      std::vector<std::string> window;
      for (size_t j = 0; j < pattern_size; ++j) {
        window.push_back(std::string(1, text[i + j]));
      }
      windows.push_back(window);
    }
    
    return windows;
  }
  
  // Convert pattern characters to uint8_t values for secret sharing
  std::vector<uint8_t> pattern_chars_to_integers(const std::vector<std::string>& pattern_chars) {
    std::vector<uint8_t> result;
    for (const auto& char_str : pattern_chars) {
      if (!char_str.empty()) {
        result.push_back(static_cast<uint8_t>(char_str[0]));
      }
    }
    return result;
  }
  
  // Convert sliding window substrings to uint8_t values for secret sharing
  std::vector<std::vector<uint8_t>> sliding_windows_to_integers(const std::vector<std::vector<std::string>>& windows) {
    std::vector<std::vector<uint8_t>> result;
    result.reserve(windows.size());

    for (const auto& window : windows) {
      result.push_back(pattern_chars_to_integers(window)); 
    }

    return result;
  }

  // Print pattern breakdown for debugging
  template <typename T>
  void print_vector(const std::vector<T>& input_vector) {
    std::cout << "[";
    for (size_t i = 0; i < input_vector.size(); ++i) {
      std::cout << "\"" << input_vector[i] << "\"";
      if (i < input_vector.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
  }
  
  // Print sliding windows for debugging
  template <typename T>
  void print_nested_vector(const std::vector<std::vector<T>>& windows) {
    std::cout << "[";
    for (size_t i = 0; i < windows.size(); ++i) {
        std::cout << "[";
        for (size_t j = 0; j < windows[i].size(); ++j) {
            std::cout << "\"" << windows[i][j] << "\"";
            if (j < windows[i].size() - 1) std::cout << ", ";
        }
        std::cout << "]";
        // if (i < windows.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
  }

  // Concat a vector of type T into string
  template <typename T>
  std::string concat_vector(const std::vector<T>& target_vector) {
    std::string final_result {};

    for (T el : target_vector) {
      final_result += std::to_string(el);
    }

    return final_result;
  }

  // String processing for pattern holder
  std::vector<uint8_t> pattern_holder(const std::string& pattern) {
    std::cout << "Pattern: \"" << pattern << "\"" << std::endl;

    std::vector<std::string> pattern_vector = break_pattern_into_chars(pattern);
    std:: cout << "Broken down to Char: \n";
    print_vector(pattern_vector);

    std::vector<uint8_t> pattern_ascii_vector = pattern_chars_to_integers(pattern_vector);
     std:: cout << "Broken down to ASCII: \n";
    print_vector(pattern_ascii_vector);

    return pattern_ascii_vector;
  }

  // String processing for text holder
  std::vector<std::vector<uint8_t>> text_holder(const std::string& text, size_t pattern_size) {
    std::cout << "Original text: \"" << text << "\", pattern_size: " << pattern_size << std::endl;

    std::vector<std::vector<std::string>> text_vector = create_sliding_windows(text, pattern_size);
    std:: cout << "Broken down to Char: \n";
    print_nested_vector(text_vector);

    std::vector<std::vector<uint8_t>> text_ascii_vector = sliding_windows_to_integers(text_vector);
    std:: cout << "Broken down to ASCII: \n";
    print_nested_vector(text_ascii_vector);

    return text_ascii_vector;
  }
}


namespace CommandLineArgumentProcessing {
    // Command-line argument structure
    struct ProgramArgs {
        int party_id = -1;
        int port = -1;
        string pattern = "";
        string text = "";
        int pattern_length = -1;
        bool help = false;
    };

    // Print usage information
    void print_usage(const char* program_name) {
        cout << "Usage: " << program_name << " [OPTIONS]\n\n"
             << "Options:\n"
             << "  --party-id <1|2>      Party identifier (1=ALICE/pattern holder, 2=BOB/text holder)\n"
             << "  --port <number>       Port number for network communication\n"
             << "  --pattern <string>    Secret pattern (for ALICE only)\n"
             << "  --text <string>       Secret text (for BOB only)\n"
             << "  --pattern-length <n>  Pattern length (for BOB to know Alice's pattern size)\n"
             << "  --help                Show this help message\n\n"
             << "Examples:\n"
             << "  # Alice (pattern holder):\n"
             << "  " << program_name << " --party-id 1 --port 12345 --pattern \"hello\"\n\n"
             << "  # Bob (text holder):\n"
             << "  " << program_name << " --party-id 2 --port 12345 --text \"hello world\" --pattern-length 5\n"
             << endl;
    }

    // Parse command-line arguments using getopt_long
    ProgramArgs parse_arguments(int argc, char** argv) {
        ProgramArgs args;
    
        static struct option long_options[] = {
            {"party-id", required_argument, 0, 'p'},
            {"port", required_argument, 0, 'P'},
            {"pattern", required_argument, 0, 'a'},
            {"text", required_argument, 0, 't'},
            {"pattern-length", required_argument, 0, 'l'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };
    
        int option_index = 0;
        int c;
    
        while ((c = getopt_long(argc, argv, "p:P:a:t:l:h", long_options, &option_index)) != -1) {
            switch (c) {
                case 'p':
                    args.party_id = atoi(optarg);
                    if (args.party_id != ALICE && args.party_id != BOB) {
                        cerr << "Error: party-id must be 1 (ALICE) or 2 (BOB)" << endl;
                        exit(1);
                    }
                    break;
                case 'P':
                    args.port = atoi(optarg);
                    if (args.port <= 0 || args.port > 65535) {
                        cerr << "Error: port must be between 1 and 65535" << endl;
                        exit(1);
                    }
                    break;
                case 'a':
                    args.pattern = string(optarg);
                    break;
                case 't':
                    args.text = string(optarg);
                    break;
                case 'l':
                    args.pattern_length = atoi(optarg);
                    if (args.pattern_length <= 0) {
                        cerr << "Error: pattern-length must be larger than 1" << endl;
                        exit(1);
                    }
                    break;
                case 'h':
                    args.help = true;
                    break;
                case '?':
                    // getopt_long already printed an error message
                    exit(1);
                    break;
                default:
                    abort();
            }
        }
        
        return args;
    }

    // Validate arguments based on party role
    void validate_arguments(const ProgramArgs& args) {
        if (args.help) {
            return; // Help will be printed in main
        }
        
        if (args.party_id == -1) {
            cerr << "Error: --party-id is required" << endl;
            exit(1);
        }
        
        if (args.port == -1) {
            cerr << "Error: --port is required" << endl;
            exit(1);
        }
        
        if (args.party_id == ALICE) {
            if (args.pattern.empty()) {
                cerr << "Error: ALICE (party 1) must provide --pattern" << endl;
                exit(1);
            }
            if (!args.text.empty()) {
                cout << "Warning: ALICE doesn't need --text argument (ignored)" << endl;
            }
        } else if (args.party_id == BOB) {
            if (args.text.empty()) {
                cerr << "Error: BOB (party 2) must provide --text" << endl;
                exit(1);
            }
            if (args.pattern_length == -1) {
                cerr << "Error: BOB (party 2) must provide --pattern-length" << endl;
                exit(1);
            }
            if (!args.pattern.empty()) {
                cout << "Warning: BOB doesn't need --pattern argument (ignored)" << endl;
            }
        }
    }
}


bool find_match(std::vector<Integer> pattern_vector, std::vector<std::vector<Integer>> text_vector) {
  size_t PATTERN_SIZE = 3;
  bool is_match { false };

  for(int i = 0; i < 3; i++) {
    
    for (int j = 0; j < PATTERN_SIZE; j++) {
      Integer a = pattern_vector[j];
      Integer b = text_vector[i][j];
      
      // Fix: a has no bool type or not convertible to bool
      if (a == b){
        return true;
      }
    }
  
  }
}

void test_matching(int party, string pattern_or_text) {

  size_t PATTERN_SIZE = 3;

  std::vector<uint8_t> pattern_holder_ascii {};
  std::vector<Integer> pattern_vector;

  std::vector<std::vector<uint8_t>> text_holder_ascii {};
  std::vector<std::vector<Integer>> text_vector;

  /*
    pattern: [1, 2, 3]
    text: [["H","E","L"], ["E","L","L"], ["L","L","O"]]
  */
  
  if (party == 0) {
    pattern_holder_ascii = StringProcessing::pattern_holder("HEL");
  } else{
    text_holder_ascii = StringProcessing::text_holder("HELLO", 3);
  }

  for(int i = 0; i < PATTERN_SIZE; i++) {
    pattern_vector.push_back(Integer(32, pattern_holder_ascii[i], ALICE));
  }

  for(int i = 0; i < 3; i++) {
    std::vector<Integer> temp_vec {};
    for (int j = 0; j < PATTERN_SIZE; j++) {
      temp_vec.push_back(Integer(32, text_holder_ascii[i][j], BOB));
    }
    text_vector.push_back(temp_vec);
  }


  int number = 20;
	Integer a(32, number, ALICE);
	Integer b(32, number, BOB);


	Bit res = a == b;

	cout << "Match found?\t"<< res.reveal<bool>()<<endl;
}



int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);
	int num = 20;
	if(argc > 3)
		num = atoi(argv[3]);

    cout << "Entered number: " << num << '\n';
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

	setup_semi_honest(io, party);
	test_matching(party, num);

	cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
