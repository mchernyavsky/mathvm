#include "mathvm.h"
#include "parser.h"

#include <iostream>
#include <sstream>

namespace mathvm {

struct AstToStringVisitor : public AstVisitor {
  virtual void visitUnaryOpNode(UnaryOpNode* node) override {
    ss << tokenOp(node->kind()) << '(';
    node->operand()->visit(this);
    ss << ')';
  }

  virtual void visitBinaryOpNode(BinaryOpNode* node) override {
    ss << '(';
    node->left()->visit(this);
    ss << ") " << tokenOp(node->kind()) << " (";
    node->right()->visit(this);
    ss << ')';
  }

  virtual void visitStringLiteralNode(StringLiteralNode* node) override {
    ss << '\'' << node->literal() << '\'';
  }

  virtual void visitDoubleLiteralNode(DoubleLiteralNode* node) override {
    ss << node->literal();
  }

  virtual void visitIntLiteralNode(IntLiteralNode* node) override {
    ss << node->literal();
  }

  virtual void visitLoadNode(LoadNode* node) override {
    ss << node->var()->name();
  }

  virtual void visitStoreNode(StoreNode* node) override {
    ss << node->var()->name() << ' ' << tokenOp(node->op()) << ' ';
    node->value()->visit(this);
    ss << ';' << endl;
  }

  virtual void visitForNode(ForNode* node) override {
    ss << "for (" << node->var()->name() << " in ";
    node->inExpr()->visit(this);
    ss << ") ";
    node->body()->visit(this);
  }

  virtual void visitWhileNode(WhileNode* node) override {
    ss << "while (";
    node->whileExpr()->visit(this);
    ss << ") ";
    node->loopBlock()->visit(this);
  }

  virtual void visitIfNode(IfNode* node) override {
    ss << "if (";
    node->ifExpr()->visit(this);
    ss << ") ";
    node->thenBlock()->visit(this);
    ss << " else ";
    node->elseBlock()->visit(this);
  }

  virtual void visitBlockNode(BlockNode* node) override {
    ss << "{" << endl;
    Scope::VarIterator vi = Scope::VarIterator(node->scope());
    while (vi.hasNext()) {
      AstVar *v = vi.next();
      ss << typeToName(v->type()) << ' ' << v->name() << ';' << endl;
    }
    node->visitChildren(this);
    ss << "}" << endl;
  }

  virtual void visitFunctionNode(FunctionNode* node) override {
    ss << typeToName(node->returnType()) << " (";
    for (uint32_t i = 0; i < node->parametersNumber(); i++) {
      ss << typeToName(node->parameterType(i)) << ' ' << node->parameterName(i);
      if (i < node->parametersNumber() - 1) ss << ", ";
    }
    ss << ") ";
    node->body()->visit(this);
  }

  virtual void visitReturnNode(ReturnNode* node) override {
    ss << "return ";
    node->returnExpr()->visit(this);
    ss << ";" << endl;
  }

  virtual void visitCallNode(CallNode* node) override {
    ss << node->name() << " (";
    for (uint32_t i = 0; i < node->parametersNumber(); i++) {
      node->parameterAt(i)->visit(this);
      if (i < node->parametersNumber() - 1) ss << ", ";
    }
    ss << ");" << endl;
  }

  virtual void visitNativeCallNode(NativeCallNode* node) override {
    // TODO
  }

  virtual void visitPrintNode(PrintNode* node) override {
    ss << "print(";
    for (uint32_t i = 0; i < node->operands(); i++) {
      node->operandAt(i)->visit(this);
      if (i < node->operands() - 1) ss << ", ";
    }
    ss << ");" << endl;
  }

  stringstream ss;
};

struct AstPrinter : public Translator {
  virtual Status *translate(const string &program, Code **code) {
    Parser parser;
    Status *status = parser.parseProgram(program);
    if (status != 0 && status->isError()) return status;
    AstToStringVisitor stringVisitor;
    parser.top()->node()->body()->visit(&stringVisitor);
    cout << stringVisitor.ss.str() << endl;

    return new Status("No executable code produced");
  }
};

Translator *Translator::create(const string &impl) {
  if (impl == "printer") {
    return new AstPrinter();
  } else {
    return NULL;
  }
}

}