/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <complex>
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
std::string getOrder(const std::string &line) {
  int i = 0;
  std::string ans;
  while(line[i] >='0'&&line[i] <='9') {
    i++;
  }
  i++;
  while(line[i]!=' ') {
    ans+=line[i];
    i++;
  }
  return ans;
}

void processLine(std::string line, Program &program, EvalState &state) {
  TokenScanner scanner;
  scanner.ignoreWhitespace();
  scanner.scanNumbers();
  scanner.setInput(line);
  //todo
  //scanner.scanStrings();
  std::string input = scanner.nextToken();

  if (scanner.getTokenType(input) == NUMBER) {
    //TODO
    //当这里数字为1e10的时候，要取消读数，输出sysnax
    //获取数字所占的长度，用来在读取表达式的时候去掉数字
    int length_number = input.length();
    int pos = stringToInteger(input);
    input = scanner.nextToken();

    //还有语句说明这个语句非空
    if (input != "") {
      if (scanner.getTokenType(input) == WORD) {
        if (input == "LET") {
          input = scanner.nextToken();
          if (input != "") {
            //只要后面有语句就进行存储
            LET *let = new LET;
            std::string new_line = line.substr(length_number + 5);
            try {
              TokenScanner *scaner_exp = new TokenScanner(new_line);
              scaner_exp->ignoreWhitespace();
              let->ex = parseExp(*scaner_exp);
              program.setParsedStatement(pos,let);
              program.addSourceLine(pos, line);
              delete scaner_exp;
            }catch (ErrorException &ex) {
              std::cout << ex.getMessage() << std::endl;
              delete let;
              return;
            }

          } else {
            error("SYNTAX ERROR");
          }
        }
        else if (input == "INPUT") {
          input = scanner.nextToken();
          if(input!= "") {
            std::string name = input;
            if(!scanner.hasMoreTokens()) {
              INPUT *in = new INPUT;
              in->name = name;
              program.setParsedStatement(pos,in);
              program.addSourceLine(pos,line);
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
          input = scanner.nextToken();
          if (input != " ") {
            PRINT *pr = new PRINT;
            //----Read a expression
            std::string  new_Line = line.substr(length_number + 7);
            TokenScanner *scanner_new = new TokenScanner(new_Line);
            try {
              scanner_new->ignoreWhitespace();
              pr->exp = parseExp(*scanner_new);
              program.setParsedStatement(pos,pr);
              program.addSourceLine(pos, line);
              delete scanner_new;
            }catch (ErrorException &ex) {
              std::cout << ex.getMessage() << std::endl;
              delete scanner_new;
              return;
            }
          } else {
            //抛出错误
            error("SYNTAX ERROR");
          }
        }
        else if (input == "REM") {
          program.addSourceLine(pos, line);
        }
        else if (input == "END") {
          if (scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
          } else {
            program.addSourceLine(pos, line);
          }
        }
        else if (input == "GOTO") {
          input = scanner.nextToken();
          //TODO 这个地方number要防止1e10 的可能直接修改scanner
          if (input != "" && scanner.getTokenType(input) == NUMBER)
            if(!scanner.hasMoreTokens()) {
              program.addSourceLine(pos, line);
            }else {
              error("SYNTAX ERROR");
            }
          }
        else if (input == "IF") {
          input = scanner.nextToken();
          IF *if_ = new IF;
          if(input != "") {
            std::string exp;
            //读左表达式
            while(input != "=" && input != ">" && input != "<" && input != "") {
              exp += input;
              exp += " ";
              input = scanner.nextToken();
            }
            //先检测左表达式
            //TODO 左表达式 = 号不判错 出现错误会不会有leak
            TokenScanner *scanner_new = new TokenScanner(exp);
            try {
              scanner_new->ignoreWhitespace();
              if_->lep = parseExp(*scanner_new);
              //
              if(input != "=" && input != ">" && input != "<") {//符号错误
                delete if_;
                delete scanner_new;
                error("SYNTAX ERROR");
              }
              //
              if_->cmp = input[0];
              input = scanner.nextToken();
              //读右表达式
              exp.clear();
              while(input != "THEN" && input != "") {
                exp += input;
                exp += " ";
                input = scanner.nextToken();
              }
              //
              if(input == "THEN") {
                scanner_new->setInput(exp);
                if_ ->rep = parseExp(*scanner_new);
              }else {
                delete if_;
                delete scanner_new;
                error("SYNTAX ERROR");
              }
              input = scanner.nextToken();
              if(input != ""&& scanner.getTokenType(input) == NUMBER) {
                if_->goto_pos = stringToInteger(input);
                program.addSourceLine(pos,line);
                program.setParsedStatement(pos,if_);
                delete scanner_new;
                return;
              }
              else {
                delete if_;
                delete scanner_new;
                error("SYNTAX ERROR");
              }
            }catch (ErrorException &ex) {
              std::cout << ex.getMessage() << std::endl;
              delete if_;
              delete scanner_new;
              return;
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
      else {
        error("SYNTAX ERROR");
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
        return;
      }
    }
  }
  else
    {
    //外部指令 OR 无序号直接执行
    if (scanner.getTokenType(input) == WORD) {
      if (input == "RUN") {
        int cur = program.getFirstLineNumber();

        std::string order;
        while(cur != -1) {
          scanner.setInput(program.getSourceLine(cur));
          scanner.nextToken();
          order = scanner.nextToken();
          if(order == "PRINT"||
            order == "LET"
            || order == "INPUT")
            {
            auto t = program.getParsedStatement(cur);
            t->execute(state,program);
            cur = program.getNextLineNumber(cur);
          }
          else if(order == "REM") {
            cur = program.getNextLineNumber(cur);
          }
          else if(order == "GOTO") {
            cur = stringToInteger(scanner.nextToken());
            if(program.getSourceLine(cur) == "") {
              error("LINE NUMBER ERROR");
            }
          }
          else if(order == "IF") {
            state.setValue("______if_next_position_________",program.getNextLineNumber(cur));
            auto t = program.getParsedStatement(cur);
            t->execute(state,program);
            cur = state.getValue("______if_next_position_________");
            if(program.getSourceLine(cur) == "") {
              error("LINE NUMBER ERROR");
            }
          }
          else if(order == "END") {
            return;
          }
          else {
            error("SYNTAX ERROR");
          }
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
        try {
          pr->exp = parseExp(scanner);
          pr->execute(state,program);
          delete pr;
        }catch (ErrorException &ex) {
          std::cout << ex.getMessage() << std::endl;
          delete pr;
          return;
        }

      }
      else if (input == "LET") {
        LET *le = new LET;
        try {
          le ->ex = parseExp(scanner);
          le ->execute(state,program);
          delete le;
        }
        catch (ErrorException &ex) {
          std::cout << ex.getMessage() << std::endl;
          delete le;
          return;
        }

      }
      else if (input == "INPUT") {
        input =scanner.nextToken();
        if(input != "") {
          if(!scanner.hasMoreTokens()){
            INPUT *in = new INPUT;
            in->name = input;
            in->execute(state,program);
            delete in;
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
        state.Clear();
        return;
      }
      else if (input == "QUIT") {
        //重新调用一遍删除指令的命令
        if (scanner.hasMoreTokens()) {
          error("SYNTAX ERROR");
          return;
        }
        program.clear();
        state.Clear();
        exit(0);
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
