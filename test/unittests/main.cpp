// zvmone: Fast Zond Virtual Machine implementation
// Copyright 2019 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "vm_loader.hpp"
#include <gtest/gtest.h>
#include <zvmc/loader.h>
#include <iostream>
#include <string>
#include <vector>

/// The loaded ZVMC module.
static zvmc::VM zvmc_module;

zvmc::VM& get_vm() noexcept
{
    return zvmc_module;
}

/// Simple and copy&paste distributable CLI parser.
///
/// TODO: Originally taken from ZVMC and modified here. Copy it back.
class cli_parser
{
public:
    using preprocessor_fn = void (*)(int*, char**);

    const char* const application_name = nullptr;
    const char* const application_version = nullptr;
    const char* const application_description = nullptr;

    std::vector<std::string> arguments_names;
    std::vector<std::string> arguments;

    preprocessor_fn preprocessor = [](int*, char**) {};

    cli_parser(const char* app_name, const char* app_version, const char* app_description,
        std::vector<std::string> args_names) noexcept
      : application_name{app_name},
        application_version{app_version},
        application_description{app_description},
        arguments_names{std::move(args_names)}
    {
        arguments.reserve(this->arguments_names.size());
    }

    /// Sets the preprocessor and enables preprocessing.
    ///
    /// The preprocessor runs on provided arguments before the parsing is done.
    /// It is allowed to modify the arguments and/or generate other output.
    ///
    /// @param fn The preprocessor function.
    void set_preprocessor(preprocessor_fn fn) noexcept { preprocessor = fn; }

    /// Parses the command line arguments.
    ///
    /// It recognize --help and --version built-in options and output for these is sent
    /// to the @p out output stream.
    /// Errors are sent to the @p err output stream.
    ///
    /// @return Negative value in case of error,
    ///         0 in case --help or --version was provided and the program should terminate,
    ///         positive value in case the program should continue.
    int parse(int argc, char* argv[], std::ostream& out, std::ostream& err)
    {
        out << application_name << " " << application_version << "\n\n";

        const auto should_exit = handle_builtin_options(argc, argv, out);

        // Run preprocessor after the output from built-in options.
        preprocessor(&argc, argv);

        if (should_exit)
            return 0;

        size_t num_args = 0;
        for (int i = 1; i < argc; ++i)
        {
            auto arg = std::string{argv[i]};

            const auto num_dashes = arg.find_first_not_of('-');
            if (num_dashes == 0)  // Argument.
            {
                ++num_args;
                if (num_args > arguments_names.size())
                {
                    err << "Unexpected argument \"" << arg << "\"\n";
                    return -1;
                }
                arguments.emplace_back(std::move(arg));
                continue;
            }

            err << "Unknown option \"" << argv[i] << "\"\n";
            return -1;
        }

        if (num_args < arguments_names.size())
        {
            for (auto i = num_args; i < arguments_names.size(); ++i)
                err << "The " << arguments_names[i] << " argument is required.\n";
            err << "Run with --help for more information.\n";
            return -1;
        }

        return 1;
    }

private:
    bool handle_builtin_options(int argc, char* argv[], std::ostream& out)
    {
        using namespace std::string_literals;

        auto help = false;
        auto version = false;

        for (int i = 1; i < argc; ++i)
        {
            help |= argv[i] == "--help"s || argv[i] == "-h"s;
            version |= argv[i] == "--version"s;
        }

        if (help)
        {
            out << "Usage: " << argv[0];
            for (const auto& name : arguments_names)
                out << " " << name;
            out << "\n\n";
            return true;
        }

        if (version)
        {
            if (application_description)
                out << application_description << "\n";
            return true;
        }

        return false;
    }
};

int main(int argc, char* argv[])
{
    try
    {
        auto cli = cli_parser{"ZVM Test", PROJECT_VERSION,
            "Testing tool for ZVMC-compatible Zond Virtual Machine implementations.\n"
            "Powered by the zvmone project.\n\n"
            "ZVMC:   https://github.com/theqrl/zvmc\n"
            "zvmone: https://github.com/theqrl/zvmone",
            {"MODULE"}};
        cli.set_preprocessor(testing::InitGoogleTest);

        if (const auto error_code = cli.parse(argc, argv, std::cout, std::cerr); error_code <= 0)
            return error_code;

        const auto& zvmc_config = cli.arguments[0];
        zvmc_loader_error_code ec;
        zvmc_module = zvmc::VM{zvmc_load_and_configure(zvmc_config.c_str(), &ec)};

        if (ec != ZVMC_LOADER_SUCCESS)
        {
            if (const auto error = zvmc_last_error_msg())
                std::cerr << "ZVMC loading error: " << error << "\n";
            else
                std::cerr << "ZVMC loading error " << ec << "\n";
            return static_cast<int>(ec);
        }

        std::cout << "Testing " << zvmc_config << "\n\n";
        return RUN_ALL_TESTS();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << "\n";
        return -2;
    }
}