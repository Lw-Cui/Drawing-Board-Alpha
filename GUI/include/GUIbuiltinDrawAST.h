#ifndef GI_BUILTINDRAW_H
#define GI_BUILTINDRAW_H

#include <AST.h>
#include <SFML/Graphics.hpp>
#include <Controller.h>

namespace ast {
    class GUIBuiltinDrawAST : public BuiltinDrawAST {
    private:
        sf::Vector2f toVec2f(const std::shared_ptr<ExprAST> &) const;

        std::shared_ptr<ExprAST> toPairAST(const sf::Vector2f &) const;

        con::Controller &controller;
    public:
        GUIBuiltinDrawAST(con::Controller &);

        std::string display() const override;

        std::shared_ptr<ExprAST> apply(const std::vector<std::shared_ptr<ExprAST>> &actualArgs, Scope &) override;
    };
}

#endif //GI_BUILTINDRAW_H
