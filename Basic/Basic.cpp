/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
  EvalState state;
  Program program;
  //cout << "Stub implementation of BASIC" << endl;
  while (true) {
    try {
      std::string input;
      getline(std::cin, input);
      if (input.empty())
        continue;
      processLine(input, program, state);
    } catch (ErrorException &ex) {
      std::cout << ex.getMessage() << std::endl;
    }
  }
  return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins

 with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */
//读指令和运行指令要分开进行
void processLine(std::string line, Program &program, EvalState &state) {
  TokenScanner scanner;
  scanner.ignoreWhitespace();
  scanner.scanNumbers();
  scanner.setInput(line);
  //todo
  std::string input = scanner.nextToken();

  if (scanner.getTokenType(input) == NUMBER) {
    //将指令位置转换器下标

    int pos = stringToInteger(input);
    input = scanner.nextToken();
    //还有语句说明这个语句非空
    if (input != "") {
      if (scanner.getTokenType(scanner.nextToken()) == WORD) {
        if (scanner.nextToken() == "LET") {
          if (scanner.hasMoreTokens()) {
            //只要后面有语句就进行存储
            LET *let = new LET;
            let->ex = parseExp(scanner);
            program.setParsedStatement(pos,let);
            program.addSourceLine(pos, line);
          } else {
            error("SYNTAX ERROR");
          }
        }
        else if (input == "INPUT") {
          if (scanner.hasMoreTokens() &&
            (scanner.getTokenType(scanner.nextToken()) == NUMBER
              || scanner.getTokenType(scanner.nextToken()) == WORD)) {
              std::string name = scanner.nextToken();
            if(!scanner.hasMoreTokens()) {
              INPUT *in = new INPUT;
              in->name = name;
              program.setParsedStatement(pos,in);
              program.addSourceLine(pos, line);
            }
            else {
              error("SYNTAX ERROR");
            }
          }
          else {
            error("SYNTAX ERROR");
          }
        }
        else if (input == "PRINT") {
          if (scanner.hasMoreTokens()) {
            PRINT *pr = new PRINT;
            pr->exp;
            program.setParsedStatement(pos,pr);
            program.addSourceLine(pos, line);
          } else {
            //抛出错误
            error("SYNTAX ERROR");
          }
        }
        else if (input == "REM") {
          //应该直接忽略REM中的所有内容
          REM *re = new REM;
          program.addSourceLine(pos, line);
          program.setParsedStatement(pos,re);
        }
        else if (input == "END") {
          if (scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
          } else {
            END *end = new END;
            program.setParsedStatement(pos,end);
            program.addSourceLine(pos, line);
          }
        }
        else if (input == "GOTO") {
          input = scanner.nextToken();
          if (scanner.hasMoreTokens() && scanner.getTokenType(input) == NUMBER) {
            GOTO *go = new GOTO;
            program.addSourceLine(pos, line);
            program.setParsedStatement(pos,go);
          } else {
            error("SYNTAX ERROR");
          }
        }
        //感觉if 这里还能再优化 能不能考虑用parser来进行使用呢
        else if (input == "IF") {
          input = scanner.nextToken();
          while (scanner.hasMoreTokens() && scanner.getTokenType(scanner.nextToken()) != OPERATOR) {
            continue;
          }
          if (scanner.nextToken() == "=" || scanner.nextToken() == ">"
              || scanner.nextToken() == "<") {
            while (scanner.hasMoreTokens() && scanner.nextToken() != "THEN") {
              continue;
            }
            if (scanner.nextToken() == "THEN") {
              if(scanner.hasMoreTokens() && scanner.getTokenType(scanner.nextToken()) == NUMBER) {
                if(!scanner.hasMoreTokens()) {
                  IF *if_ = new IF;
                  program.addSourceLine(pos,line);
                  program.setParsedStatement(pos,if_);
                  return;
                }
                else {
                  error("SYNTAX ERROR");
                }
              }
              else {
                error("SYNTAX ERROR");
              }
            }
            else {
              error("SYNTAX ERROR");
            }
              }
          else {
            error("SYNTAX ERROR");
          }
        }
        else
          {
          error("SYNTAX ERROR");
        }
      }
    }
    else
      {
      //若输入行号+ “”说明语句为空 删除这个语句
      if (program.getSourceLine(pos) != "")
        {
        //找到目标语句
        program.removeSourceLine(pos);
      }
      else {
        //看看空语句会不会报错
        return ;
        //不会做任何操作
      }
    }
  }
  else {
    //外部指令 OR 无序号直接执行
    if (scanner.getTokenType(input) == WORD) {
      if (input == "RUN") {
        int cur = program.getFirstLineNumber();
        while(cur != -1) {
          auto t = program.getParsedStatement(cur);
          t->execute(state,program);
          cur = program.getNextLineNumber(cur);
        }
      }
      else if (input == "LIST") {
        int i = program.getFirstLineNumber();
        while (i != -1) {
          std::cout << (program.getSourceLine(i)) << "\n";
          i = program.getNextLineNumber(i);
        }
      }
      else if (input == "PRINT") {
        PRINT *pr = new PRINT;
        pr->exp = parseExp(scanner);
        pr->execute(state,program);
      }
      else if (input == "LET") {
        LET *le = new LET;
        le ->ex = parseExp(scanner);
        le ->execute(state,program);
      }
      else if (input == "INPUT") {
        if (scanner.hasMoreTokens() &&
            (scanner.getTokenType(scanner.nextToken()) == NUMBER
              || scanner.getTokenType(scanner.nextToken()) == WORD)) {
          std::string name = scanner.nextToken();
          if(!scanner.hasMoreTokens()) {
            INPUT *in = new INPUT;
            in->name = name;
            in->execute(state,program);
          }
          else {
            error("SYNTAX ERROR");
          }
              }
        else {
          error("SYNTAX ERROR");
        }
      }
      else if (input == "CLEAR") {
        if (scanner.hasMoreTokens()) {
          error("SYNTAX ERROR");
          return;
        }
        program.clear();
      }
      else if (input == "QUIT") {
        //重新调用一遍删除指令的命令
        if (scanner.hasMoreTokens()) {
          error("SYNTAX ERROR");
          return;
        }
        program.clear();
        return;
      }
      else if (input == "HELP") {
      }
      else
        {
        //只有上面这些指令能在没有序号的情况下执行
        error("SYNTAX ERROR");
      }
    }
    else {
      error("SYNTAX ERROR");
    }
  }
}
