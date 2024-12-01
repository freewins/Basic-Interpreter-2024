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


class REM:public Statement {
 //感觉这里不需要写什么类似 注释内容的存储，在运行的时候会直接把这里跳过
public:
 void execute(EvalState &state, Program &program) override {

 }
};
class LET:public Statement {
public:
 Expression *ex ;
 void execute(EvalState &state, Program &program) override {
  //
  ex->eval(state);

 }
};

class PRINT:public Statement {
public:
 void execute(EvalState &state, Program &program) override {
  std::cout<<"\n"<<exp->eval(state);
 }
 Expression *exp;
};
class INPUT:public Statement {
 //后检测操作 只在执行的时候检测语法错误

public:
 std::string name;
 void execute(EvalState &state, Program &program) override {

  int temp;
  if(state.isDefined(name)) {
    std::cout<<"\n ?";
    if(std::cin>>temp) {
     state.setValue(name ,temp);

    }
   else {
    error("INVALID NUMBER");
   }
  }
  else {
   error("VARIABLE NOT DEFINED");
  }
 }

};
class END:public Statement {
public:
void execute(EvalState &state, Program &program) override {
 return;
}
};
class GOTO:public Statement {
public:
 //下一个跳转的行数
 //除了最开始的检查，刚进入程序的时候也要进行检查
 int next_pos;
 void execute(EvalState &state, Program &program) override {

 }
};
class IF:public Statement {
public:
 void execute(EvalState &state, Program &program) override {

 }
 Expression *lep,*rep;
 int goto_pos;
};
class THEN:public Statement {

};
#endif
