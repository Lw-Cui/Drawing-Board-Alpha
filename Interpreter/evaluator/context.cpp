#include <memory>
#include <unordered_map>
#include <AST.h>
#include <builtinAST.h>
#include <parser.h>
#include <context.h>
#include <stack>
#include <exception.h>

using namespace std;
using namespace ast;

namespace context {

    bool Scope::count(const std::string &str) const {
        return findSymbol(str) != nullptr;
    }


    void Scope::setDynamicScope(const std::shared_ptr<Scope> &scope) {
        dynamicScope = scope;
    }

    void Scope::setLexicalScope(const std::shared_ptr<Scope> &scope) {
        lexicalScope = scope;
    }

    void Scope::clearCurScope() {
        symtab.clear();
    }

    void Scope::addBuiltinFunc(const std::string &name, const std::shared_ptr<ast::ExprAST> &expr) {
        symtab[name] = expr;
    }

    pExpr Scope::findSymbol(const std::string &id) const {
        if (builtinFunc.count(id)) return builtinFunc.find(id)->second;
        if (symtab.count(id)) return symtab.find(id)->second;

        pExpr ret = nullptr;
        if (lexicalScope && ((ret = lexicalScope->findSymbol(id)) != nullptr))
            return ret;
        if (dynamicScope && ((ret = dynamicScope->findSymbol(id)) != nullptr))
            return ret;
        return ret;
    }

    void Scope::addSymbol(const std::string &id, pExpr ptr) {
        symtab[id] = ptr;
    }

    std::stack<std::string> Scope::callTrace;

    int Scope::anonymousId;


    void Scope::stepIntoFunc(const std::string &name) {
        CLOG(DEBUG, "context") << string(callTrace.size(), '|') << " /`"
                               << "call [" << name << "] @ level " << callTrace.size() + 1;
        callTrace.push(name);
    }

    void Scope::stepOutFunc() {
        CLOG(DEBUG, "context") << string(callTrace.size() - 1, '|') << " \\_"
                               << "finish [" << callTrace.top() << "]" << " @ level " << callTrace.size();
        callTrace.pop();
    }

    void Scope::stepIntoAnonymousFunc() {
        stepIntoFunc("(anonymous #" + to_string(anonymousId++) + ")");
    }

    std::string Scope::currentFunc() const {
        if (callTrace.empty()) return "";
        else return callTrace.top();
    }

    Scope::Scope() {}

    // Attention: to add a builtin func, you have to:
    // 1. assure that any place where you make_shared<Builtin> invokes scope->stepInto()
    // 2. in its apply func, call scope->stepOut()
    const std::unordered_map<std::string, std::shared_ptr<ast::ExprAST>> Scope::builtinFunc = {
        {"cons",        make_shared<BuiltinConsAST>()},
        {"car",         make_shared<BuiltinCarAST>()},
        {"cdr",         make_shared<BuiltinCdrAST>()},
        {"+",           make_shared<BuiltinAddAST>()},
        {"*",           make_shared<BuiltinMultiplyAST>()},
        {"null?",       make_shared<BuiltinNullAST>()},
        {"<",           make_shared<BuiltinLessThanAST>()},
        {"#opposite",   make_shared<BuiltinOppositeAST>()},
        {"#reciprocal", make_shared<BuiltinReciprocalAST>()},
        {"list",        make_shared<BuiltinListAST>()},
        {"else",        make_shared<BooleansTrueAST>()},
    };

}

