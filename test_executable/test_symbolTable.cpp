//
// Created by maozunyao on 2021/10/24.
//
#include <iostream>
#include "SymbolTable.h"
int main(){
    SymbolTable symbolTable;
    IntType * intTypePointer=new IntType();
    Symbol *symbol=new Symbol("a",SymbolType::FLOAT,intTypePointer);
    symbolTable.insertVariableSymbol("a", symbol);
    Symbol *another= symbolTable.searchVariableSymbol("a");
    std::cout<<(SymbolType::FLOAT==another->symbolType)<<std::endl;
    return 0;
};