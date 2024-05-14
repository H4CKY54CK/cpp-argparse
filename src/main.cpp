#include <argparse.hpp>


int main(int argc, char* const argv[]) {
  std::string prog = argv[0];
  prog = prog.substr(2, prog.size());
  ArgumentParser parser(prog);
  Argument arg("--animals");
  arg.alt = "-a";
  arg.nargs = "+";
  parser.add_argument(arg);
  arg = {"people"};
  arg.nargs = "+";
  parser.add_argument(arg);
  ParsedResults args = parser.parse_args(argc, argv);

  for (auto i: args) {
    std::cout << i.first << std::endl;
    for (auto v: i.second) {
      std::cout << "  " << v << std::endl;
    }
  }


  return 0;
}
