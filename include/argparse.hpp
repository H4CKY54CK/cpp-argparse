#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Declaration

bool is_number(const std::string&);

struct Argument {
  std::string name;
  std::string dest;
  std::string alt = "";
  std::string nargs = "1";
  std::string argtype = "optional";
  std::string action = "store";
  Argument(const std::string&);
};


typedef std::unordered_map<std::string, std::vector<std::string>> ParsedResults;
typedef unsigned long long ulonglong;
typedef unsigned long ulong;



class ArgumentParser {
public:
  std::string prog;
  ArgumentParser(): ArgumentParser("parser"){};
  ArgumentParser(const std::string&);
  void error(const std::string&);
  std::unordered_map<std::string, Argument> optionals;
  std::vector<Argument> positionals;
  void add_argument(const Argument&);
  ParsedResults parse_args(int, char* const*);
  std::vector<std::string> read_values(const std::vector<std::string>&);
};





// Implementation

bool is_number(const std::string& s) {
  for (ulong i = 0; i < s.size(); i++) {
    if (!std::isdigit(s.at(i))) {
      return false;
    }
  }
  return true;
}



// Argument

Argument::Argument(const std::string& s) {
  name = s;
  if (s.substr(0, 2) == "--") {
    dest = s.substr(2, s.size());
  }
  else if (s.substr(0, 1) == "-") {
    dest = s.substr(1, s.size());
  }
  else {
    dest = s;
    argtype = "positional";
  }
}


// ArgumentParser
ArgumentParser::ArgumentParser(const std::string& s) {
  prog = s;
}

void ArgumentParser::error(const std::string& s) {
  std::cerr << "\033[31m[" << prog << " error]\033[m: " << s << std::endl;
}

void ArgumentParser::add_argument(const Argument& argument) {
  if (argument.argtype == "optional") {
    if (optionals.count(argument.name)) {
      error("Can't use the same flag twice: " + argument.name);
      exit(1);
    }
    optionals.emplace(argument.name, argument);
    if (argument.alt != "") {
      if (optionals.count(argument.alt)) {
        error("Can't use the same flag twice: " + argument.name);
        exit(1);
      }
      optionals.emplace(argument.alt, argument);
    }
  }
  else if (argument.argtype == "positional") {
    for (auto p: positionals) {
      if (p.name == argument.name and p.dest == argument.dest) {
        error("Can't have two positional arguments with the same name and dest");
        exit(1);
      }
    }
    positionals.emplace_back(argument);
  }
}


std::vector<std::string> ArgumentParser::read_values(const std::vector<std::string>& vec) {
  std::vector<std::string> result;
  std::string arg = vec.at(0);
  Argument* option = &(optionals.at(arg));
  for (ulong i = 1; i < vec.size(); i++) {
    if (is_number(option->nargs)) {
      if (result.size() == std::stoul(option->nargs)) {
        break;
      }
    }
    else if (option->nargs == "?" and result.size() == 1) {
      break;
    }
    arg = vec.at(i);
    if (arg.substr(0, 1) == "-") {
      break;
    }
    result.push_back(arg);
  }
  // Make sure we have enough args
  if (is_number(option->nargs)) {
    if (result.size() < std::stoul(option->nargs)) {
      error(vec.at(0) + " expects " + option->nargs + " argument(s)");
      exit(1);
    }
  }
  else if (option->nargs == "+" and result.size() == 0) {
    error(vec.at(0) + " expects at least 1 argument");
    exit(1);
  }
  return result;
}

ParsedResults ArgumentParser::parse_args(int argc, char* const* argv) {

  ParsedResults results;
  std::vector<std::string> params(argv + 1, argv + argc);
  std::vector<std::string> remaining;
  std::vector<std::string> r;
  std::string arg;
  std::string flag;
  std::string value;
  int found;
  bool keep_parsing = true;

  // while there are still arguments to be parsed
  while (params.size()) {
    arg = params.at(0);
    // when we get -- then we need to stop parsing options
    if (keep_parsing and arg == "--") {
      keep_parsing = false;
      params.erase(params.begin());
    }
    // if it looks like an option
    else if (keep_parsing and arg.substr(0, 1) == "-") {
      found = arg.find("=");
      // if it looks like --option=value
      if (found > 0) {
        flag = arg.substr(0, arg.find("="));
        if (optionals.count(flag)) {
          value = arg.substr(arg.find("=") + 1, arg.size());
          r = {value};
          results[optionals.at(flag).dest] = r;
          params.erase(params.begin());
        }
        else {
          error("Unrecognized optional argument: " + flag);
          exit(1);
        }
      }
      // if its a known option
      else if (optionals.count(arg)) {
        // Yes, it is necessary for this to be two lines. We need to know how many we collected from this function call,
        // not how many we collected ever. aka action="append"
        r = read_values(params);
        results[optionals.at(arg).dest] = r;
        for (ulong a = 0; a < r.size() + 1; a++) {
          params.erase(params.begin());
        }
      }
      // otherwise its unrecognized
      else {
        error("Unrecognized optional arguments: " + arg);
        exit(1);
      }
    }
    // otherwise its positional (check for unrecognized later)
    else {
      remaining.push_back(arg);
      params.erase(params.begin());
    }
  }


  // Get the minimum required arguments and whether it needs to be exact or not
  ulong total = 0;
  bool exact = true;
  for (auto p: positionals) {
    if (is_number(p.nargs)) {
      total += std::stoul(p.nargs);
    }
    else if (p.nargs == "+") {
      total += 1;
      exact = false;
    }
    else if (p.nargs == "*" || p.nargs == "?") {
      exact = false;
    }
  }

  // Check that we have enough positional arguments
  std::string joined = "";
  if (exact) {
    if (remaining.size() > total) {
      for (ulong s = total; s < remaining.size(); s++) {
        joined += remaining.at(s);
        joined += ", ";
      }
      error("Unrecognized positional arg(s): " + joined.substr(0, joined.size() - 2));
      error("First");
      exit(1);
    }
  }

  // if not enough positional arguments
  int subtotal = remaining.size();
  for (auto p: positionals) {
    if (is_number(p.nargs)) {
      subtotal -= std::stoul(p.nargs);
    }
    else if (p.nargs == "+") {
      subtotal -= 1;
    }
    if (subtotal < 0) {
      error("Expected positional argument: " + p.dest);
      exit(1);
    }
  }

  // Figure out how to arrange the arguments
  ulong one = 1;
  int known = 0;
  std::vector<std::string> layout;
  for (auto p: positionals) {
    if (is_number(p.nargs)) {
      known += std::stoul(p.nargs);
    }
  }

  // Arrange the arguments
  ulong ix = 0;
  for (auto p: positionals) {
    if (is_number(p.nargs)) {
      for (ulong i = 0; i < std::stoul(p.nargs); i++) {
        results[p.dest].push_back(remaining.at(ix));
        ix += 1;
      }
    }
    else if (p.nargs == "+" || p.nargs == "*") {
      for (ulong i = 0; i < remaining.size() - known; i++) {
        results[p.dest].push_back(remaining.at(ix));
        ix += 1;
      }
    }
    else if (p.nargs == "?") {
      for (ulong i = 0; i < std::min(remaining.size() - known, one); i++) {
        results[p.dest].push_back(remaining.at(ix));
        ix += 1;
      }
    }
  }

  // If not all arguments were consumed, then error (unsure if this can happen?)
  joined = "";
  if (ix < remaining.size()) {
    for (ulong s = ix; s < remaining.size(); s++) {
      joined += remaining.at(s);
      joined += ", ";
    }
    error("Unrecognized positional arg(s): " + joined.substr(0, joined.size() - 2));
    error("Second");
    exit(1);
  }

  return results;
}



#endif /* FUNCTIONS_H */
