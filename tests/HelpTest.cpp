#ifdef CLI_SINGLE_FILE
#include "CLI11.hpp"
#else
#include "CLI/CLI.hpp"
#endif

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <fstream>

using ::testing::HasSubstr;
using ::testing::Not;

TEST(THelp, Basic) {
    CLI::App app{"My prog"};

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, HasSubstr("-h,--help"));
    EXPECT_THAT(help, HasSubstr("Options:"));
    EXPECT_THAT(help, HasSubstr("Usage:"));
}

TEST(THelp, Footer) {
    CLI::App app{"My prog"};
    app.set_footer("Report bugs to bugs@example.com");

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, HasSubstr("-h,--help"));
    EXPECT_THAT(help, HasSubstr("Options:"));
    EXPECT_THAT(help, HasSubstr("Usage:"));
    EXPECT_THAT(help, HasSubstr("Report bugs to bugs@example.com"));
}

TEST(THelp, OptionalPositional) {
    CLI::App app{"My prog"};

    std::string x;
    app.add_option("something", x, "My option here");

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, HasSubstr("-h,--help"));
    EXPECT_THAT(help, HasSubstr("Options:"));
    EXPECT_THAT(help, HasSubstr("Positionals:"));
    EXPECT_THAT(help, HasSubstr("something TEXT"));
    EXPECT_THAT(help, HasSubstr("My option here"));
    EXPECT_THAT(help, HasSubstr("Usage: program [OPTIONS] [something]"));
}

TEST(THelp, Hidden) {
    CLI::App app{"My prog"};

    std::string x;
    app.add_option("something", x, "My option here")->group("");
    std::string y;
    app.add_option("--another", y)->group("");

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, HasSubstr("-h,--help"));
    EXPECT_THAT(help, HasSubstr("Options:"));
    EXPECT_THAT(help, HasSubstr("[something]"));
    EXPECT_THAT(help, Not(HasSubstr("something ")));
    EXPECT_THAT(help, Not(HasSubstr("another")));
}

TEST(THelp, OptionalPositionalAndOptions) {
    CLI::App app{"My prog"};
    app.add_flag("-q,--quick");

    std::string x;
    app.add_option("something", x, "My option here");

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, HasSubstr("-h,--help"));
    EXPECT_THAT(help, HasSubstr("Options:"));
    EXPECT_THAT(help, HasSubstr("Usage: program [OPTIONS] [something]"));
}

TEST(THelp, RequiredPositionalAndOptions) {
    CLI::App app{"My prog"};
    app.add_flag("-q,--quick");

    std::string x;
    app.add_option("something", x, "My option here")->required();

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, HasSubstr("-h,--help"));
    EXPECT_THAT(help, HasSubstr("Options:"));
    EXPECT_THAT(help, HasSubstr("Positionals:"));
    EXPECT_THAT(help, HasSubstr("Usage: program [OPTIONS] something"));
}

TEST(THelp, MultiOpts) {
    CLI::App app{"My prog"};
    std::vector<int> x, y;
    app.add_option("-q,--quick", x, "Disc")->expected(2);
    app.add_option("-v,--vals", y, "Other");

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, Not(HasSubstr("Positionals:")));
    EXPECT_THAT(help, HasSubstr("Usage: program [OPTIONS]"));
    EXPECT_THAT(help, HasSubstr("INT x 2"));
    EXPECT_THAT(help, HasSubstr("INT ..."));
}

TEST(THelp, VectorOpts) {
    CLI::App app{"My prog"};
    std::vector<int> x = {1, 2};
    app.add_option("-q,--quick", x, "", true);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("INT=[1,2] ..."));
}

TEST(THelp, MultiPosOpts) {
    CLI::App app{"My prog"};
    std::vector<int> x, y;
    app.add_option("quick", x, "Disc")->expected(2);
    app.add_option("vals", y, "Other");

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, HasSubstr("Positionals:"));
    EXPECT_THAT(help, HasSubstr("Usage: program [OPTIONS]"));
    EXPECT_THAT(help, HasSubstr("INT x 2"));
    EXPECT_THAT(help, HasSubstr("INT ..."));
    EXPECT_THAT(help, HasSubstr("[quick(2x)]"));
    EXPECT_THAT(help, HasSubstr("[vals...]"));
}

TEST(THelp, EnvName) {
    CLI::App app{"My prog"};
    std::string input;
    app.add_option("--something", input)->envname("SOME_ENV");

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("SOME_ENV"));
}

TEST(THelp, Requires) {
    CLI::App app{"My prog"};

    CLI::Option *op1 = app.add_flag("--op1");
    app.add_flag("--op2")->requires(op1);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("Requires: --op1"));
}

TEST(THelp, RequiresPositional) {
    CLI::App app{"My prog"};

    int x, y;

    CLI::Option *op1 = app.add_option("op1", x, "one");
    app.add_option("op2", y, "two")->requires(op1);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("Positionals:"));
    EXPECT_THAT(help, HasSubstr("Requires: op1"));
}

TEST(THelp, Excludes) {
    CLI::App app{"My prog"};

    CLI::Option *op1 = app.add_flag("--op1");
    app.add_flag("--op2")->excludes(op1);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("Excludes: --op1"));
}

TEST(THelp, ExcludesPositional) {
    CLI::App app{"My prog"};

    int x, y;

    CLI::Option *op1 = app.add_option("op1", x);
    app.add_option("op2", y)->excludes(op1);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("Positionals:"));
    EXPECT_THAT(help, HasSubstr("Excludes: op1"));
}

TEST(THelp, ManualSetters) {

    CLI::App app{"My prog"};

    int x = 1;

    CLI::Option *op1 = app.add_option("--op", x);
    op1->set_default_str("12");
    op1->set_type_name("BIGGLES");
    EXPECT_EQ(x, 1);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("=12"));
    EXPECT_THAT(help, HasSubstr("BIGGLES"));

    op1->set_default_val("14");
    EXPECT_EQ(x, 14);
    help = app.help();
    EXPECT_THAT(help, HasSubstr("=14"));
}

TEST(THelp, Subcom) {
    CLI::App app{"My prog"};

    auto sub1 = app.add_subcommand("sub1");
    app.add_subcommand("sub2");

    std::string help = app.help();
    EXPECT_THAT(help, HasSubstr("Usage: program [OPTIONS] [SUBCOMMAND]"));

    app.require_subcommand();

    help = app.help();
    EXPECT_THAT(help, HasSubstr("Usage: program [OPTIONS] SUBCOMMAND"));

    help = sub1->help();
    EXPECT_THAT(help, HasSubstr("Usage: sub1"));

    char x[] = "./myprogram";
    char y[] = "sub2";

    std::vector<char *> args = {x, y};
    app.parse((int)args.size(), args.data());

    help = app.help();
    EXPECT_THAT(help, HasSubstr("Usage: ./myprogram sub2"));
}

TEST(THelp, IntDefaults) {
    CLI::App app{"My prog"};

    int one{1}, two{2};
    app.add_option("--one", one, "Help for one", true);
    app.add_set("--set", two, {2, 3, 4}, "Help for set", true);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("--one"));
    EXPECT_THAT(help, HasSubstr("--set"));
    EXPECT_THAT(help, HasSubstr("1"));
    EXPECT_THAT(help, HasSubstr("=2"));
    EXPECT_THAT(help, HasSubstr("2,3,4"));
}

TEST(THelp, SetLower) {
    CLI::App app{"My prog"};

    std::string def{"One"};
    app.add_set_ignore_case("--set", def, {"oNe", "twO", "THREE"}, "Help for set", true);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("--set"));
    EXPECT_THAT(help, HasSubstr("=One"));
    EXPECT_THAT(help, HasSubstr("oNe"));
    EXPECT_THAT(help, HasSubstr("twO"));
    EXPECT_THAT(help, HasSubstr("THREE"));
}

TEST(THelp, OnlyOneHelp) {
    CLI::App app{"My prog"};

    // It is not supported to have more than one help flag, last one wins
    app.set_help_flag("--help", "No short name allowed");
    app.set_help_flag("--yelp", "Alias for help");

    std::vector<std::string> input{"--help"};
    EXPECT_THROW(app.parse(input), CLI::ExtrasError);
}

TEST(THelp, RemoveHelp) {
    CLI::App app{"My prog"};
    app.set_help_flag();

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, Not(HasSubstr("-h,--help")));
    EXPECT_THAT(help, Not(HasSubstr("Options:")));
    EXPECT_THAT(help, HasSubstr("Usage:"));

    std::vector<std::string> input{"--help"};
    try {
        app.parse(input);
    } catch(const CLI::ParseError &e) {
        EXPECT_EQ(static_cast<int>(CLI::ExitCodes::Extras), e.get_exit_code());
    }
}

TEST(THelp, NoHelp) {
    CLI::App app{"My prog"};
    app.set_help_flag();

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, Not(HasSubstr("-h,--help")));
    EXPECT_THAT(help, Not(HasSubstr("Options:")));
    EXPECT_THAT(help, HasSubstr("Usage:"));

    std::vector<std::string> input{"--help"};
    try {
        app.parse(input);
    } catch(const CLI::ParseError &e) {
        EXPECT_EQ(static_cast<int>(CLI::ExitCodes::Extras), e.get_exit_code());
    }
}

TEST(THelp, CustomHelp) {
    CLI::App app{"My prog"};

    CLI::Option *help_option = app.set_help_flag("--yelp", "display help and exit");
    EXPECT_EQ(app.get_help_ptr(), help_option);

    std::string help = app.help();

    EXPECT_THAT(help, HasSubstr("My prog"));
    EXPECT_THAT(help, Not(HasSubstr("-h,--help")));
    EXPECT_THAT(help, HasSubstr("--yelp"));
    EXPECT_THAT(help, HasSubstr("Options:"));
    EXPECT_THAT(help, HasSubstr("Usage:"));

    std::vector<std::string> input{"--yelp"};
    try {
        app.parse(input);
    } catch(const CLI::CallForHelp &e) {
        EXPECT_EQ(static_cast<int>(CLI::ExitCodes::Success), e.get_exit_code());
    }
}

TEST(THelp, NiceName) {
    CLI::App app;

    int x;
    auto long_name = app.add_option("-s,--long,-q,--other,that", x);
    auto short_name = app.add_option("more,-x,-y", x);
    auto positional = app.add_option("posit", x);

    EXPECT_EQ(long_name->single_name(), "--long");
    EXPECT_EQ(short_name->single_name(), "-x");
    EXPECT_EQ(positional->single_name(), "posit");
}

TEST(Exit, ErrorWithHelp) {
    CLI::App app{"My prog"};

    std::vector<std::string> input{"-h"};
    try {
        app.parse(input);
    } catch(const CLI::CallForHelp &e) {
        EXPECT_EQ(static_cast<int>(CLI::ExitCodes::Success), e.get_exit_code());
    }
}

TEST(Exit, ErrorWithoutHelp) {
    CLI::App app{"My prog"};

    std::vector<std::string> input{"--none"};
    try {
        app.parse(input);
    } catch(const CLI::ParseError &e) {
        EXPECT_EQ(static_cast<int>(CLI::ExitCodes::Extras), e.get_exit_code());
    }
}

TEST(Exit, ExitCodes) {
    CLI::App app;

    auto i = static_cast<int>(CLI::ExitCodes::Extras);
    EXPECT_EQ(0, app.exit(CLI::Success()));
    EXPECT_EQ(0, app.exit(CLI::CallForHelp()));
    EXPECT_EQ(i, app.exit(CLI::ExtrasError("Thing")));
    EXPECT_EQ(42, app.exit(CLI::RuntimeError(42)));
    EXPECT_EQ(1, app.exit(CLI::RuntimeError())); // Not sure if a default here is a good thing
}

struct CapturedHelp : public ::testing::Test {
    CLI::App app{"My Test Program"};
    std::stringstream out;
    std::stringstream err;

    int run(const CLI::Error &e) { return app.exit(e, out, err); }

    void reset() {
        out.clear();
        err.clear();
    }
};

TEST_F(CapturedHelp, Sucessful) {
    EXPECT_EQ(run(CLI::Success()), 0);
    EXPECT_EQ(out.str(), "");
    EXPECT_EQ(err.str(), "");
}

TEST_F(CapturedHelp, JustAnError) {
    EXPECT_EQ(run(CLI::RuntimeError(42)), 42);
    EXPECT_EQ(out.str(), "");
    EXPECT_EQ(err.str(), "");
}

TEST_F(CapturedHelp, CallForHelp) {
    EXPECT_EQ(run(CLI::CallForHelp()), 0);
    EXPECT_EQ(out.str(), app.help());
    EXPECT_EQ(err.str(), "");
}

TEST_F(CapturedHelp, NormalError) {
    EXPECT_EQ(run(CLI::ExtrasError("Thing")), static_cast<int>(CLI::ExitCodes::Extras));
    EXPECT_EQ(out.str(), "");
    EXPECT_THAT(err.str(), HasSubstr("for more information"));
    EXPECT_THAT(err.str(), HasSubstr("ERROR: ExtrasError"));
    EXPECT_THAT(err.str(), HasSubstr("Thing"));
    EXPECT_THAT(err.str(), Not(HasSubstr("Usage")));
}

TEST_F(CapturedHelp, RepacedError) {
    app.set_failure_message(CLI::FailureMessage::help);

    EXPECT_EQ(run(CLI::ExtrasError("Thing")), static_cast<int>(CLI::ExitCodes::Extras));
    EXPECT_EQ(out.str(), "");
    EXPECT_THAT(err.str(), Not(HasSubstr("for more information")));
    EXPECT_THAT(err.str(), HasSubstr("ERROR: ExtrasError"));
    EXPECT_THAT(err.str(), HasSubstr("Thing"));
    EXPECT_THAT(err.str(), HasSubstr("Usage"));
}
