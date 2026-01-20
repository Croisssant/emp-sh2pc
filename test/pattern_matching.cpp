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
    // Too many variables to handle if I create a specialized function
    std::cout << "[";
    for (size_t i = 0; i < pattern_ascii_vector.size(); ++i) {
      std::cout << "\"" << static_cast<int>(pattern_ascii_vector[i]) << "\"";
      if (i < pattern_ascii_vector.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

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
    std::cout << "[";
    for (size_t i = 0; i < text_ascii_vector.size(); ++i) {
        std::cout << "[";
        for (size_t j = 0; j < text_ascii_vector[i].size(); ++j) {
            std::cout << "\"" << static_cast<int>(text_ascii_vector[i][j]) << "\"";
            if (j < text_ascii_vector[i].size() - 1) std::cout << ", ";
        }
        std::cout << "]";
        // if (i < windows.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    return text_ascii_vector;
  }
}


Bit find_match(std::vector<Integer> pattern_vector, std::vector<std::vector<Integer>> text_vector) {

  size_t pattern_size = pattern_vector.size();
  size_t num_windows = text_vector.size();

  Bit any_window_matches(false, PUBLIC);

  for (size_t window = 0; window < num_windows; window++) {
    Bit all_chars_match(true, PUBLIC);

    for (size_t char_pos = 0; char_pos < pattern_size; char_pos++) {
      Integer pattern_char = pattern_vector[char_pos];
      Integer window_char = text_vector[window][char_pos];

      Bit char_matches = pattern_char == window_char;

      all_chars_match = all_chars_match & char_matches;
    }

    any_window_matches = any_window_matches | all_chars_match;
  }

  return any_window_matches;
}




void test_matching(int party) {
    size_t PATTERN_SIZE = 3;
    size_t TEXT_LENGTH = 5;  // "HELLO" has 5 characters
    size_t num_windows = TEXT_LENGTH - PATTERN_SIZE + 1; // 3 windows

    std::vector<uint8_t> pattern_holder_ascii(PATTERN_SIZE, 0);  // Initialize with zeros
    std::vector<Integer> pattern_vector;

    std::vector<std::vector<uint8_t>> text_holder_ascii(num_windows, std::vector<uint8_t>(PATTERN_SIZE, 0));
    std::vector<std::vector<Integer>> text_vector;

    // Fill in the actual data based on party
    if (party == ALICE) {
        pattern_holder_ascii = StringProcessing::pattern_holder("HEM");
    } else {
        text_holder_ascii = StringProcessing::text_holder("HELLO", PATTERN_SIZE);
    }

    // Create Integer vectors (both parties do this)
    for(size_t i = 0; i < PATTERN_SIZE; i++) {
        pattern_vector.push_back(Integer(32, pattern_holder_ascii[i], ALICE));
    }

    for(size_t i = 0; i < num_windows; i++) {
        std::vector<Integer> temp_vec;
        for (size_t j = 0; j < PATTERN_SIZE; j++) {
            temp_vec.push_back(Integer(32, text_holder_ascii[i][j], BOB));
        }
        text_vector.push_back(temp_vec);
    }

    Bit res = find_match(pattern_vector, text_vector);
    cout << "Match found?\t" << res.reveal<bool>() << endl;
}

// void test_matching(int party) {

//   size_t PATTERN_SIZE = 3;

//   std::vector<uint8_t> pattern_holder_ascii {};
//   std::vector<Integer> pattern_vector;

//   std::vector<std::vector<uint8_t>> text_holder_ascii {};
//   std::vector<std::vector<Integer>> text_vector;

//   /*
//     pattern: [1, 2, 3]
//     text: [["H","E","L"], ["E","L","L"], ["L","L","O"]]
//   */
  
//   if (party == 0) {
//     pattern_holder_ascii = StringProcessing::pattern_holder("HEL");
//   } else{
//     text_holder_ascii = StringProcessing::text_holder("HELLO", 3);
//   }

//   size_t num_windows = text_holder_ascii.size();

//   for(int i = 0; i < PATTERN_SIZE; i++) {
//     pattern_vector.push_back(Integer(32, pattern_holder_ascii[i], ALICE));
//   }

//   for(int i = 0; i < num_windows; i++) {
//     std::vector<Integer> temp_vec {};
//     for (int j = 0; j < PATTERN_SIZE; j++) {
//       temp_vec.push_back(Integer(32, text_holder_ascii[i][j], BOB));
//     }
//     text_vector.push_back(temp_vec);
//   }


// 	Bit res = find_match(pattern_vector, text_vector);

// 	cout << "Match found?\t"<< res.reveal<bool>()<<endl;
// }



int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);
	int num = 20;
	if(argc > 3)
		num = atoi(argv[3]);

    cout << "Entered number: " << num << '\n';
	NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

	setup_semi_honest(io, party);
	test_matching(party);

	cout << CircuitExecution::circ_exec->num_and()<<endl;
	finalize_semi_honest();
	delete io;
}
