# cpp-argparse
A semi-faithful port of Python's argparse to C++

## Documentation
(This builds as low as C++11, I think)

First, you create a parser object with `ArgumentParser parser("program name")`.

Then you create an `Argument` object `arg` (for example) with either the optional argument string flag or positional argument name, then change one of these class/instance members:
- `.alt` - (string) Allows for adding a short flag option string
- `.nargs` - (string) Defaults to "1", but can be set to an integer or one of:
  - \? - Accepts 0 or 1 arguments
  - \* - Accepts 0 or more arguments
  - \+ - Accepts 1 or more arguments

Then you add the Argument object to the parser with `parser.add_argument(arg)`

Once your arguments are added to the parser, you can then use `parser.parse_args(argc, argv)` to parse your CLI's arguments. The first argument will be skipped, of course.

A setup for a multi-argument SOURCE, single-argument DEST program (`examples/examples1.cpp`) might look like:

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

    }

Producing this output:

    source
      one
      two
    dest
      three
