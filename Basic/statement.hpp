/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include <pstl/glue_execution_defs.h>

#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "program.hpp"
#include "parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"

class Program;

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {
public:
  /*
   * Constructor: Statement
   * ----------------------
   * The base class constructor is empty.  Each subclass must provide
   * its own constructor.
   */

  Statement();

  /*
   * Destructor: ~Statement
   * Usage: delete stmt;
   * -------------------
   * The destructor deallocates the storage for this expression.
   * It must be declared virtual to ensure that the correct subclass
   * destructor is called when deleting a statement.
   */

  virtual ~Statement();

  /*
   * Method: execute
   * Usage: stmt->execute(state);
   * ----------------------------
   * This method executes a BASIC statement.  Each of the subclasses
   * defines its own execute method that implements the necessary
   * operations.  As was true for the expression evaluator, this
   * method takes an EvalState object for looking up variables or
   * controlling the operation of the interpreter.
   */

  virtual void execute(EvalState &state, Program &program) = 0;
};


/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */

class LET : public Statement {
public:
  Expression *ex = nullptr;

  void execute(EvalState &state, Program &program) override {
    //
    ex->eval(state);
  }
  ~LET() {
    if(ex!=nullptr){delete ex;
    }
  }
};

class PRINT : public Statement {
public:
  Expression *exp;
  void execute(EvalState &state, Program &program) override {
    std::cout << exp->eval(state) << "\n";
  }
  ~PRINT() {
    if(exp != nullptr){
      delete exp;
    }
  }
};

class INPUT : public Statement {
public:
  std::string name;
  ~INPUT() {
  }
  void execute(EvalState &state, Program &program) override {
    int temp = 0;
    int cur = 1;
    std::string rubbish;
    //TODO
    //添加一个变量名检索的工具
    while(true) {
      try {
        std::cout << " ? ";
        std::string line_;
        getline(std::cin,line_);
        int i = 0;
        if(line_[i] == '-') {
          cur = -1;
          i++;
        }
        for(;i<line_.length();i++) {

          if(line_[i]>='0'&&line_[i]<='9') {
            temp = temp *10 + (line_[i] - '0');
          }
          else {
            error("INVALID NUMBER");
          }
        }
        temp *= cur;
        state.setValue(name, temp);
        return;
      }catch (ErrorException &ex) {
        temp = 0;
        std::cout << ex.getMessage() << std::endl;
      }
    }
  }
};
//这个可以删掉了

class IF : public Statement {
public:
  void execute(EvalState &state, Program &program) override {
    int l = lep->eval(state);
    int r = rep->eval(state);
    if(cmp == '=') {
      if(l == r) {
        state.setValue("______if_next_position_________",goto_pos);

      }
      else {
        return;
      }
    }
    if(cmp == '>') {
      if(l > r) {
        state.setValue("______if_next_position_________",goto_pos);

      }
      else {
        return;
      }
    }
    if(cmp == '<') {
      if(l < r) {
        state.setValue("______if_next_position_________",goto_pos);
      }
      else {
        return;
      }
    }
  }

  Expression *lep = nullptr, *rep = nullptr;
  char cmp;
  int goto_pos;
  ~IF() {
    if(lep != nullptr) {
      delete lep;
    }
    if(rep != nullptr) {
      delete rep;
    }
  }
};

class THEN : public Statement {

};
#endif
