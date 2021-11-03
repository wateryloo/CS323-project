//
// Created by maozunyao on 2021/10/24.
//

#ifndef CS323_PROJECT1_SEMANTICANALYZER_H
#define CS323_PROJECT1_SEMANTICANALYZER_H
#include <iostream>

#include "SymbolTable.h"
#include "SyntaxTreeNode.h"
void usePrimarySymbol(SyntaxTreeNode *idNode, SymbolTable &symbolTable);
void useFunctionSymbol(SyntaxTreeNode *expNode, SymbolTable &symbolTable);
void insertPrimarySymbol(SyntaxTreeNode *defNode, SymbolTable &symbolTable);
void insertFunctionSymbol(SyntaxTreeNode *extDefNode, SymbolTable &symbolTable);
void checkAssignDataType(SyntaxTreeNode * left,SyntaxTreeNode * right);
void assignVarDecIDType(SyntaxTreeNode* varDec,SyntaxTreeNode *exp);
void assignSpecifierType(SyntaxTreeNode *specifier);
void checkrValue(SyntaxTreeNode * exp);
void checkReturnType(SyntaxTreeNode * rtExp);
#endif  // CS323_PROJECT1_SEMANTICANALYZER_H
