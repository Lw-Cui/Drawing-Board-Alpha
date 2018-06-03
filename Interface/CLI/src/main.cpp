#include <exception>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <sys/resource.h>
#include <context.h>
#include <parser.h>
#include <image.h>
#include <exception.h>
#include <cxxopts.hpp>
#include <visitor.h>
#include <CLIbuiltinDrawAST.h>

using namespace std;
using namespace ast;
using namespace visitor;
using namespace parser;
using namespace lexers;
using namespace cxxopts;
using namespace context;
using namespace exception;

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[]) {
    START_EASYLOGGINGPP(argc, argv);
    el::Logger *parserLogger = el::Loggers::getLogger("parser");
    el::Logger *evaluatorLogger = el::Loggers::getLogger("evaluator");
    el::Logger *conextLogger = el::Loggers::getLogger("context");
    el::Logger *exceptionLogger = el::Loggers::getLogger("exception");
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format,
                                       "[%logger] %msg [%fbase:%line]");
    try {
        // since we may access (1000, 0), which will crashe the whole world if `Image image(1000, 1000);`
        Image image(1001, 1001);
        // set stack up to 48MB: We don't need it anymore!
        //setStack(48 * 1024 * 1024);
        Options options(argv[0], " - Scheme Interpreter/painter command line options");
        options.add_options()("o,output", "output image", value<std::string>())
            ("src", "src filename", cxxopts::value<std::vector<std::string>>())
            ("p,path", "stdlib path", cxxopts::value<std::string>())
            ("nostdlib", "Do not use stdlib")
            ("nopainter", "Do not use painter-related lib")
            ("h,help", "Print help");
        options.parse_positional("src");
        options.parse(argc, argv);

        if (options.count("h")) {
            cout << options.help({""}) << endl;
            return 0;
        }
        if (!options.count("nostdlib") && !options.count("path")) {
            cout << "You must specify -nostdlib or -path" << endl;
            return 0;
        }
        Lexer lex;
        auto scope = std::make_shared<Scope>();
        auto path = options["path"].as<std::string>();
        if (!options.count("nostdlib")) {
            lex.appendExp("(load \"" + path + "/Base.scm\")");
        }
        if (!options.count("nopainter") && !options.count("nostdlib")) {
            scope->addBuiltinFunc("#painter", std::make_shared<ast::CLIBuiltinDrawAST>(image));
            lex.appendExp("(load \"" + path + "/Shape.scm\")");
            lex.appendExp("(load \"" + path + "/Frame.scm\")");
        }
        auto ast = parseAllExpr(lex);
        ast->eval(scope);
        auto &v = options["src"].as<std::vector<std::string>>();
        for (const auto &s : v) {
            lex.appendExp(string("(load \"") + s + "\")");
            auto ast = std::dynamic_pointer_cast<AllExprAST>(parseAllExpr(lex));
            auto ret = ast->evalAll(scope);
            for (auto ptr: ret) {
                if (ptr) {
                    visitor::DisplayVisitor disp;
                    ptr->accept(disp);
                    auto str = disp.to_string();
                    if (!str.empty()) cout << str << endl;
                }
            }
        }
        if (!options.count("nopainter") && !options.count("nostdlib")) {
            std::string::size_type const p(v.back().find_last_of('.'));
            auto basename = v.back().substr(0, p) + ".bmp";
            image.save(basename.c_str());
        }
        //} catch (RuntimeError &e) {
        //cout << e.what() << endl;
        //throw;
    } catch (const OptionException &e) {
        cout << "error parsing options: " << e.what()
             << "Try \' --help\' for more information." << endl;
        throw;
    } catch (std::exception &e) {
        cout << e.what() << endl;
        throw;
    }
    return 0;
}
