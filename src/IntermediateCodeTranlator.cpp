//
// Created by maozunyao on 2021/12/6.
//

#include "IntermediateCodeTranlator.h"

std::string zero = "0";
std::string one = "1";

static std::string new_temp() {
    static int tempValueTemp = 1;
    return std::string("t").append(std::to_string(tempValueTemp++));
}

static std::string new_label() {
    static int tempValueLabel = 1;
    return std::string("label").append(std::to_string(tempValueLabel++));
}

void getExtDefs(SyntaxTreeNode *extdefList, std::vector<SyntaxTreeNode *> &extdefs) {
    if (extdefList->children.size() == 2) {
        extdefs.push_back(extdefList->children.at(0));
        getExtDefs(extdefList->children.at(1), extdefs);
    }
}

void translateFunctionAndPrint(SyntaxTreeNode *program, SymbolTable &symboltable) {
    // std::cout<<__func__<<":"<<__LINE__<<std::endl;
    SyntaxTreeNode *extDefList = program->children.at(0);
    std::vector<SyntaxTreeNode *> extdefs;
    getExtDefs(extDefList, extdefs);
    for (int i = 0; i < extdefs.size(); ++i) {
        SyntaxTreeNode *extDef = extdefs.at(i);
        if (extDef->productionEnum == ProductionEnum::EXTDEF_FROM_FUNC) {
            std::vector<IntermediateCode *> &code1 = translate_fundec(extDef->children.at(0)->children.at(1),
                                                                      symboltable);
            std::vector<IntermediateCode *> &code2 = translate_Compst(extDef->children.at(1), symboltable);
            mergeInterCode(extDef, code1);
            mergeInterCode(extDef, code2);
            extDef->printInterCode();
        }
    }
}

std::vector<IntermediateCode *> &translate_fundec(SyntaxTreeNode *fundec, SymbolTable &symbolTable) {
    //  std::cout<<__func__<<":"<<__LINE__<<std::endl;
    return translate_fundec_without_args(fundec, symbolTable);
}

std::vector<IntermediateCode *> &translate_fundec_without_args(SyntaxTreeNode *fundec, SymbolTable &symbolTable) {
    //   std::cout<<__func__<<":"<<__LINE__<<std::endl;
    std::string functionName = fundec->symbol->name;
    IntermediateCode *functionCode = createFuntionDecCode(functionName);
    fundec->selfAndChildrenCodes.push_back(functionCode);
    return fundec->selfAndChildrenCodes;

}

std::vector<IntermediateCode *> &translate_Compst(SyntaxTreeNode *compst, SymbolTable &symbolTable) {
    // std::cout<<__func__<<":"<<__LINE__<<std::endl;
    SyntaxTreeNode *stmtList = compst->children.at(2);
    std::vector<SyntaxTreeNode *> stmts;
    getStmtNodes(stmtList, stmts);
    for (int i = 0; i < stmts.size(); ++i) {
        std::vector<IntermediateCode *> &codes = translate_stmt(stmts.at(i), symbolTable);
        mergeInterCode(compst, codes);
    }
    return compst->selfAndChildrenCodes;
}

void getStmtNodes(SyntaxTreeNode *stmtList, std::vector<SyntaxTreeNode *> &stmts) {
    if (stmtList->children.size() == 2) {
        stmts.push_back(stmtList->children.at(0));
        getStmtNodes(stmtList->children.at(1), stmts);
    }
}

std::vector<IntermediateCode *> &translate_Exp(SyntaxTreeNode *exp, SymbolTable &symbolTable, std::string &place) {
    // std::cout<<__func__<<":"<<__LINE__<<std::endl;
    switch (exp->productionEnum) {
        case ProductionEnum::EXP_FROM_ID_LP_ARGS_RP: {
            std::string functionName = exp->children.at(0)->attribute_value;
            Symbol *function = symbolTable.searchFunctionDefinitionSymbol(functionName);
            FunctionType *functionType = std::get<FunctionType *>(function->symbolData);
            std::vector<std::string> args;
            std::vector<IntermediateCode *> &code1 = translate_Args(exp->children.at(2), symbolTable, args);
            if (function->name == "write") {
                for (int i = 0; i < code1.size(); ++i) {
                    exp->selfAndChildrenCodes.push_back(code1.at(i));
                }
                IntermediateCode *intermediateCode = createWriteCode(args.at(0));
                exp->selfAndChildrenCodes.push_back(intermediateCode);
                return exp->selfAndChildrenCodes;
            } else {
                for (int i = 0; i < code1.size(); ++i) {
                    exp->selfAndChildrenCodes.push_back(code1.at(i));
                }
                for (int i = 0; i < args.size(); ++i) {
                    IntermediateCode *code2 = createArgsCode(args.at(i));
                    exp->selfAndChildrenCodes.push_back(code2);
                }
                IntermediateCode *code3 = createCallFuntionCode(place, function->name);
                exp->selfAndChildrenCodes.push_back(code3);
                return exp->selfAndChildrenCodes;
            }
        }
        case ProductionEnum::EXP_FROM_ID_LP_RP: {
            Symbol *function = exp->children.at(0)->symbol;
            if (function->name == "read") {
                IntermediateCode *code = createReadCode(place);
                exp->selfAndChildrenCodes.push_back(code);
                return exp->selfAndChildrenCodes;
            } else {
                IntermediateCode *code = createCallFuntionCode(place, function->name);
                exp->selfAndChildrenCodes.push_back(code);
                return exp->selfAndChildrenCodes;
            }
        }
        case ProductionEnum::EXP_FROM_INT: {
            IntermediateCode *intermediateCode = createConstantCode(place, exp->symbol->name);
            exp->selfAndChildrenCodes.push_back(intermediateCode);
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_ID: {
            std::string &variable = exp->children.at(0)->symbol->name;
            IntermediateCode *code = createAssignCode(place, variable);
            exp->selfAndChildrenCodes.push_back(code);
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_EXP_ASSIGNOP_EXP: {
            SyntaxTreeNode *operatorNode = exp->children.at(1);
            SyntaxTreeNode *leftExp = exp->children.at(0);
            SyntaxTreeNode *rightExp = exp->children.at(2);
            std::string variableName = leftExp->symbol->name; //the symbol is reserved in semantic analyzer, so we just get it, not using symbol table
            std::string temp = new_temp();
            std::vector<IntermediateCode *> &code1 = translate_Exp(rightExp, symbolTable, temp);
            IntermediateCode *code2 = createAssignCode(variableName, temp);
            for (int i = 0; i < code1.size(); ++i) {
                exp->selfAndChildrenCodes.push_back(code1.at(i));
            }
            exp->selfAndChildrenCodes.push_back(code2);
            if (place != "null") {
                IntermediateCode *code3 = createAssignCode(place, variableName);
                exp->selfAndChildrenCodes.push_back(code3);
            }
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_EXP_BINARY_EXP: {
            std::string temp1 = new_temp();
            std::string temp2 = new_temp();
            std::vector<IntermediateCode *> &code1 = translate_Exp(exp->children.at(0), symbolTable, temp1);
            std::vector<IntermediateCode *> &code2 = translate_Exp(exp->children.at(2), symbolTable, temp2);
            IntermediateCode *code3 = createBinaryCode(place, temp1, temp2, exp->children.at(1)->attribute_value);
            mergeInterCode(exp, code1);
            mergeInterCode(exp, code2);
            exp->selfAndChildrenCodes.push_back(code3);
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_MINUS_EXP: {
            std::string temp1 = new_temp();
            std::vector<IntermediateCode *> &code1 = translate_Exp(exp->children.at(1), symbolTable, temp1);
            IntermediateCode *code2 = createMinusCode(place, temp1);
            mergeInterCode(exp, code1);
            exp->selfAndChildrenCodes.push_back(code2);
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_EXP_RELOP_EXP:
        case ProductionEnum::EXP_FROM_EXP_AND_EXP:
        case ProductionEnum::EXP_FROM_EXP_OR_EXP:
        case ProductionEnum::EXP_FROM_NOT_EXP: {
            std::string label1 = new_label();
            std::string label2 = new_label();
            IntermediateCode *code0 = createConstantCode(place, zero);
            exp->selfAndChildrenCodes.push_back(code0);
            std::vector<IntermediateCode *> &code1 = translate_cond_Exp(exp, symbolTable, label1, label2);
            IntermediateCode *code2 = createLabelCode(label1);
            IntermediateCode *code3 = createConstantCode(place, one);
            IntermediateCode *code4 = createLabelCode(label2);
            exp->selfAndChildrenCodes.push_back(code2);
            exp->selfAndChildrenCodes.push_back(code3);
            exp->selfAndChildrenCodes.push_back(code4);
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_LP_EXP_RP: {
            std::vector<IntermediateCode *> &code1 = translate_Exp(exp->children.at(1), symbolTable, place);
            mergeInterCode(exp, code1);
            return exp->selfAndChildrenCodes;
        }
        default:
            std::cout << "warning: this case is not handled!" << std::endl;
            return exp->selfAndChildrenCodes;
    }
}

std::vector<IntermediateCode *> &
translate_Args(SyntaxTreeNode *argsNode, SymbolTable &symbolTable, std::vector<std::string> &argList) {
    // std::cout<<__func__<<":"<<__LINE__<<std::endl;;
    if (argsNode->productionEnum == ProductionEnum::ARGS_FROM_EXP_COMMA_ARGS) {
        SyntaxTreeNode *exp = argsNode->children.at(0);
        std::string temp = new_temp();
        std::vector<IntermediateCode *> &code1 = translate_Exp(exp, symbolTable, temp);
        argList.push_back(temp);
        std::vector<IntermediateCode *> &code2 = translate_Args(exp, symbolTable, argList);
        for (int i = 0; i < code1.size(); ++i) {
            argsNode->selfAndChildrenCodes.push_back(code1.at(i));
        }
        for (int i = 0; i < code2.size(); ++i) {
            argsNode->selfAndChildrenCodes.push_back(code2.at(i));
        }
        return argsNode->selfAndChildrenCodes;
    } else if (argsNode->productionEnum == ProductionEnum::ARGS_FROM_EXP) {
        SyntaxTreeNode *exp = argsNode->children.at(0);
        std::string temp = new_temp();
        std::vector<IntermediateCode *> &code1 = translate_Exp(exp, symbolTable, temp);
        argList.push_back(temp);
        for (int i = 0; i < code1.size(); ++i) {
            argsNode->selfAndChildrenCodes.push_back(code1.at(i));
        }
        return argsNode->selfAndChildrenCodes;
    } else {
        return argsNode->selfAndChildrenCodes;
    }
}

std::vector<IntermediateCode *> &translate_stmt(SyntaxTreeNode *stmt, SymbolTable &symbolTable) {
    // std::cout<<__func__<<":"<<__LINE__<<std::endl;
    switch (stmt->productionEnum) {
        case ProductionEnum::STMT_FROM_EXP_SEMI: {
            std::string str = "null";
            mergeInterCode(stmt, translate_Exp(stmt->children.at(0), symbolTable, str));
            return stmt->selfAndChildrenCodes;
        }
        case ProductionEnum::STMT_FROM_RETURN_EXP_SEMI: {
            std::string temp = new_temp();
            std::vector<IntermediateCode *> &code1 = translate_Exp(stmt->children.at(1), symbolTable, temp);
            IntermediateCode *code2 = createReturnCode(temp);
            code1.push_back(code2);
            mergeInterCode(stmt, code1);
            return stmt->selfAndChildrenCodes;
        }
        case ProductionEnum::STMT_IF_LP_EXP_RP_STMT: {
            std::string label1 = new_label();
            std::string label2 = new_label();
            std::vector<IntermediateCode *> &code1 = translate_cond_Exp(stmt->children[2], symbolTable, label1, label2);
            IntermediateCode *codelabel1 = createLabelCode(label1);
            std::vector<IntermediateCode *> &code2 = translate_stmt(stmt->children[4], symbolTable);
            IntermediateCode *codelabel2 = createLabelCode(label2);
            mergeInterCode(stmt, code1);
            stmt->selfAndChildrenCodes.push_back(codelabel1);
            mergeInterCode(stmt, code2);
            stmt->selfAndChildrenCodes.push_back(codelabel2);
            return stmt->selfAndChildrenCodes;
        }
        case ProductionEnum::STMT_IF_LP_EXP_RP_STMT_ELSE_STMT: {
            std::string label1 = new_label();
            std::string label2 = new_label();
            std::string label3 = new_label();
            std::vector<IntermediateCode *> &code1 = translate_cond_Exp(stmt->children[2], symbolTable, label1, label2);
            IntermediateCode *codelabel1 = createLabelCode(label1);
            std::vector<IntermediateCode *> &code2 = translate_stmt(stmt->children[4], symbolTable);
            IntermediateCode *GOTOlabel3 = createGOTOCode(label3);
            IntermediateCode *codelabel2 = createLabelCode(label2);
            std::vector<IntermediateCode *> &code3 = translate_stmt(stmt->children[6], symbolTable);
            IntermediateCode *codelabel3 = createLabelCode(label3);
            mergeInterCode(stmt, code1);
            stmt->selfAndChildrenCodes.push_back(codelabel1);
            mergeInterCode(stmt, code2);
            stmt->selfAndChildrenCodes.push_back(GOTOlabel3);
            stmt->selfAndChildrenCodes.push_back(codelabel2);
            mergeInterCode(stmt, code3);
            stmt->selfAndChildrenCodes.push_back(codelabel3);
            return stmt->selfAndChildrenCodes;
        }
        case ProductionEnum::STMT_WHILE_LP_EXP_RP_STMT: {
            std::string label1 = new_label();
            std::string label2 = new_label();
            std::string label3 = new_label();
            IntermediateCode *codelabel1 = createLabelCode(label1);
            std::vector<IntermediateCode *> &code1 = translate_cond_Exp(stmt->children[2], symbolTable, label2, label3);
            IntermediateCode *codelabel2 = createLabelCode(label2);
            std::vector<IntermediateCode *> &code2 = translate_stmt(stmt->children.at(4), symbolTable);
            IntermediateCode *gotolabel1 = createGOTOCode(label1);
            stmt->selfAndChildrenCodes.push_back(codelabel1);
            mergeInterCode(stmt, code1);
            stmt->selfAndChildrenCodes.push_back(codelabel2);
            mergeInterCode(stmt, code2);
            stmt->selfAndChildrenCodes.push_back(gotolabel1);
            IntermediateCode *gotolabel3 = createGOTOCode(label3);
            stmt->selfAndChildrenCodes.push_back(gotolabel3);
            return stmt->selfAndChildrenCodes;
        }
        case ProductionEnum::STMT_FROM_COMPST: {
            std::vector<IntermediateCode *> &code1 = translate_Compst(stmt->children[0], symbolTable);
            mergeInterCode(stmt, code1);
            return stmt->selfAndChildrenCodes;
        }
        default:
            std::cout << "warning: this case is not handled!" << std::endl;
            return stmt->selfAndChildrenCodes;
    }
}

void mergeInterCode(SyntaxTreeNode *merge, std::vector<IntermediateCode *> &codes) {
    //   std::cout<<__func__<<":"<<__LINE__<<std::endl;
    for (int i = 0; i < codes.size(); ++i) {
        merge->selfAndChildrenCodes.push_back(codes.at(i));
    }
}

std::vector<IntermediateCode *> &
translate_cond_Exp(SyntaxTreeNode *exp, SymbolTable &symbolTable, std::string &lb_t, std::string lb_f) {
    // std::cout<<__func__<<":"<<__LINE__<<std::endl;
    switch (exp->productionEnum) {
        case ProductionEnum::EXP_FROM_EXP_RELOP_EXP: {
            std::string temp1 = new_temp();
            std::string temp2 = new_temp();
            std::vector<IntermediateCode *> &code1 = translate_Exp(exp->children.at(0), symbolTable, temp1);
            std::vector<IntermediateCode *> &code2 = translate_Exp(exp->children.at(2), symbolTable, temp2);
            IntermediateCode *ifGOTOCode = createIfGOTOCode(temp1, temp2, exp->children.at(1)->attribute_value, lb_t);
            IntermediateCode *GOTOcode = createGOTOCode(lb_f);
            mergeInterCode(exp, code1);
            mergeInterCode(exp, code2);
            exp->selfAndChildrenCodes.push_back(ifGOTOCode);
            exp->selfAndChildrenCodes.push_back(GOTOcode);
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_EXP_AND_EXP: {
            std::string label1 = new_label();
            std::vector<IntermediateCode *> &code1 = translate_cond_Exp(exp->children.at(0), symbolTable, label1, lb_f);
            IntermediateCode *labelCode = createLabelCode(label1);
            std::vector<IntermediateCode *> &code2 = translate_cond_Exp(exp->children.at(2), symbolTable, lb_t, lb_f);
            mergeInterCode(exp, code1);
            exp->selfAndChildrenCodes.push_back(labelCode);
            mergeInterCode(exp, code2);
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_EXP_OR_EXP: {
            std::string label1 = new_label();
            std::vector<IntermediateCode *> &code1 = translate_cond_Exp(exp->children.at(0), symbolTable, lb_t, lb_f);
            IntermediateCode *labelCode = createLabelCode(label1);
            std::vector<IntermediateCode *> &code2 = translate_cond_Exp(exp->children.at(2), symbolTable, lb_t, lb_f);
            mergeInterCode(exp, code1);
            exp->selfAndChildrenCodes.push_back(labelCode);
            mergeInterCode(exp, code2);
            return exp->selfAndChildrenCodes;
        }
        case ProductionEnum::EXP_FROM_NOT_EXP: {
            std::vector<IntermediateCode *> &code1 = translate_cond_Exp(exp->children.at(1), symbolTable, lb_f, lb_t);
            mergeInterCode(exp, code1);
            return exp->selfAndChildrenCodes;
        }
        default:
            std::cout << "warning: this case is not handled!" << std::endl;
            return exp->selfAndChildrenCodes;
    }
}