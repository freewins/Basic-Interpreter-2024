/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() = default;

void Program::clear() {
    // Replace this stub with your own code
    //todo
    source_program_map_.clear();
    //这里的内存要进行释放
    for(auto t = program_map_.begin();t!= program_map_.end(); ++t) {
        delete t->second;
        program_map_.erase(t);
    }
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    // Replace this stub with your own code
    //todo
    auto cur = source_program_map_.find(lineNumber) ;
    if(cur == source_program_map_.end()) {
        source_program_map_[lineNumber] = line;
    }
    else {
        source_program_map_[lineNumber] = line;
        //当我的程序需要重新修改一段指令时 如果这是在程序运行的阶段，那么我应该
        //需要把对应的表达式删除然后再进行求值
        //所以在RUN的时候应该先把程序都进行好求值和存储改变量，然后再进行运算
        //但是这样又想复杂了 只要我不断的进行替换就可以
    }
}

void Program::removeSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    auto cur = source_program_map_.find(lineNumber);
    if(cur == source_program_map_.end()) {
        //报错
        //有点想不到会在什么地方报错，应该不会有这种错误
    }
    else {
        source_program_map_.erase(cur);
        delete program_map_[lineNumber];
        program_map_.erase(lineNumber);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    auto cur = source_program_map_.find(lineNumber);
    if(cur == source_program_map_.end()) {
        return "";
    }
    else {
        return cur->second;
    }
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Replace this stub with your own code
    //todo
    program_map_[lineNumber] = stmt;
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
   // Replace this stub with your own code
   //todo
    auto cur = program_map_.find(lineNumber);
    if(cur == program_map_.end()) {
        return nullptr;
    }
    return program_map_[lineNumber];
}

int Program::getFirstLineNumber() {
    // Replace this stub with your own code
    //todo
    if(source_program_map_.empty()) {
        return -1;
    }
    else {
        return source_program_map_.begin()->first;
    }
}

int Program::getNextLineNumber(int lineNumber) {
    // Replace this stub with your own code
    //todo
    auto cur = source_program_map_.upper_bound(lineNumber);
    if(cur == source_program_map_.end()) {
        return -1;
    }
    else {
        return cur->first;
    }

}

//more func to add
//todo


