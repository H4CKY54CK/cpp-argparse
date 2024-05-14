#include <argparse.hpp>

int main(int argc, char* argv[]) {
  ArgumentParser parser("mycli");

  Argument arg("source");
  arg.nargs = "+";
  parser.add_argument(arg);

  arg = {"dest"};
  parser.add_argument(arg);

  ParsedResults results = parser.parse_args(argc, argv);

  std::cout << "source" << std::endl;
  for (int i = 0; i < results["source"].size(); ++i) {
    std::cout << "  " << results["source"].at(i) << std::endl;
  }

  std::cout << "dest" << std::endl;
  for (int i = 0; i < results["dest"].size(); ++i) {
    std::cout << "  " << results["dest"].at(i) << std::endl;
  }
}
