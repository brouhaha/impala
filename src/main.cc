// main.cc
//
// Copyright 2022-2025 Eric Smith
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdlib>
#include <format>
#include <initializer_list>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "assembler.hh"


namespace po = boost::program_options;

void conflicting_options(const boost::program_options::variables_map& vm,
			 std::initializer_list<const std::string> opts)
{
  if (opts.size() < 2)
  {
    throw std::invalid_argument("conflicting_options requires at least two options");
  }
  for (auto opt1 = opts.begin(); opt1 < opts.end(); opt1++)
  {
    if (vm.count(*opt1))
    {
      for (auto opt2 = opt1 + 1; opt2 != opts.end(); opt2++)
      {
	if (vm.count(*opt2))
	{
	  std::cerr << std::format("Options {} and {} are mutually exclusive\n", *opt1, *opt2);
	  std::exit(1);
	}
      }
    }
  }
}


constexpr std::string source_fn_suffix = ".p65";
constexpr std::string binary_fn_suffix = ".bin";
constexpr std::string listing_fn_suffix = ".lst";

int main(int argc, char *argv[])
{
  std::string source_fn;
  try
  {
    po::options_description gen_opts("Options");
    gen_opts.add_options()
      ("help", "output help message");

    po::options_description hidden_opts("Hidden options:");
    hidden_opts.add_options()
      ("source", po::value<std::string>(&source_fn), "source filename");

    po::positional_options_description positional_opts;
    positional_opts.add("source", -1);

    po::options_description cmdline_opts;
    cmdline_opts.add(gen_opts).add(hidden_opts);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
	      options(cmdline_opts).positional(positional_opts).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
      std::cout << "Usage: " << argv[0] << " [options]\n\n";
      std::cout << gen_opts << "\n";
      return 0;
    }

    if (vm.count("source") != 1)
    {
      std::cout << "source file must be specified\n";
      std::exit(1);
    }
  }
  catch (po::error& e)
  {
    std::cerr << "argument error: " << e.what() << "\n";
    std::exit(1);
  }

  std::string base_fn = source_fn;
  if (source_fn.ends_with(source_fn_suffix))
  {
    base_fn = source_fn.substr(0, source_fn.size() - source_fn_suffix.size());
  }

  std::string binary_fn  = base_fn + binary_fn_suffix;
  std::string listing_fn = base_fn + listing_fn_suffix;

  Assembler assembler(source_fn,
		      binary_fn,
		      listing_fn);

  assembler.assemble();
}
