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
