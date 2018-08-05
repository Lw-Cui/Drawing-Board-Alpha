#include <memory>
#include <fstream>
#include <lexers.h>
#include <parser.h>
#include <exception.h>

using namespace lexers;
using namespace parser;
using namespace exception;
using namespace std;


shared_ptr<ExprAST> parser::parseDefinitionExpr(lexers::Lexer &lex) {
    switch (lex.stepForward()) {
        case Lexer::TokIdentifier:
            return parseIdDefinitionExpr(lex);
        case Lexer::TokOpeningBracket:
            return parseFunctionDefinitionExpr(lex);
        default:
            CLOG(DEBUG, "exception");
            throw UnsupportedSyntax("Cannot parse definition: lexer return invalid type");
    }
}

std::shared_ptr<ExprAST> parser::parseIfStatementExpr(lexers::Lexer &lex) {
    CLOG(DEBUG, "parser") << "Parse If statement";
    lex.stepForward();
    try {
        auto condition = parseExpr(lex);
        auto trueClause = parseExpr(lex);
        auto falseClause = parseExpr(lex);
        return make_shared<IfStatementAST>(condition, trueClause, falseClause);
    } catch (RuntimeError &e) {
        e.appendInfo("@ if statement");
        throw;
    }
}

std::shared_ptr<ExprAST> parser::parseTrueExpr(lexers::Lexer &lex) {
    lex.stepForward();
    CLOG(DEBUG, "parser") << "Parse #t";
    return make_shared<BooleansTrueAST>();
}

std::shared_ptr<ExprAST> parser::parseCondStatementExpr(lexers::Lexer &lex) {
    lex.stepForward();
    CLOG(DEBUG, "parser") << "Parse cond";
    std::vector<std::shared_ptr<ExprAST>> condition, result;
    while (lex.getTokType() != Lexer::TokClosingBracket) {
        lex.stepForward();
        condition.push_back(parseExpr(lex));
        result.push_back(parseExpr(lex));
        lex.stepForward();
    }
    return make_shared<CondStatementAST>(condition, result);
}

std::shared_ptr<ExprAST> parser::parseFalseExpr(lexers::Lexer &lex) {
    lex.stepForward();
    CLOG(DEBUG, "parser") << "Parse #f";
    return make_shared<BooleansFalseAST>();
}

std::shared_ptr<ExprAST> parser::parseLoadingFileExpr(lexers::Lexer &lex) {
    lex.stepForward();
    auto filename = lex.getIdentifier().substr(1);
    filename.pop_back();
    CLOG(DEBUG, "parser") << "Loading file: " << filename;
    return make_shared<LoadingFileAST>(filename);
}

std::shared_ptr<ExprAST> parser::parseNilExpr(lexers::Lexer &lex) {
    lex.stepForward();
    CLOG(DEBUG, "parser") << "Parse nil";
    return std::make_shared<NilAST>();
}

std::shared_ptr<ExprAST> parser::parseLetStatementExpr(lexers::Lexer &lex) {
    CLOG(DEBUG, "parser") << "Parse let";
    lex.stepForward();
    lex.stepForward();
    std::vector<std::shared_ptr<ExprAST>> id, v;
    while (lex.getTokType() != Lexer::TokClosingBracket) {
        lex.stepForward();
        id.push_back(parseExpr(lex));
        v.push_back(parseExpr(lex));
        lex.stepForward();
    }
    lex.stepForward();
    std::shared_ptr<ExprAST> expr = parseExpr(lex);
    return make_shared<LetStatementAST>(id, v, expr);
}

