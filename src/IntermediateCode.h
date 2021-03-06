//
// Created by maozunyao on 2021/12/6.
//

#ifndef CS323_PROJECT1_INTERMEDIATECODE_H
#define CS323_PROJECT1_INTERMEDIATECODE_H

#include <string>

enum class OperandType { VARIABLE, CONSTANT, ADDRESS };

enum class IntermediateCodeType {
  ASSIGN,
  CALL,
  CONSTANT,
  BINARY,
  MINUS,
  LABEL,
  GOTO,
  FUNCTION,
  RETURN,
  PARAM,
  ARG,
  READ,
  WRITE,
  DEC,
  IF_GOTO,
  ARRAY_OFFSET,
  GET_VALUE_IN_ADDRESS,
  ASSIGN_VALUE_IN_ADDRESS,
  ADDRESS_ASSIGN_ADDRESS
};

class Operand {
 public:
  Operand(OperandType type, std::string &variableName);

  OperandType operandEnum;

  std::string var_name_;

  Operand(const Operand &operand) {
    operandEnum = operand.operandEnum;
    var_name_ = operand.var_name_;
  }
};

class IntermediateCode {
 public:
  IntermediateCodeType intermediateCodeEnum;
  Operand *result;
  Operand *op1;
  Operand *op2;
  std::string relation;
  explicit IntermediateCode(IntermediateCodeType type);
  void print() const;
};

IntermediateCode *createCallFuntionCode(std::string &rtName,
                                        std::string &funtionName);

IntermediateCode *createReadCode(std::string &variable);

IntermediateCode *createAssignCode(std::string &left, std::string &right);

IntermediateCode *createWriteCode(std::string &variable);

IntermediateCode *createArgsCode(std::string &variable);

IntermediateCode *createReturnCode(std::string &returnValue);

IntermediateCode *createIfGOTOCode(std::string &temp_left,
                                   std::string &temp_right,
                                   std::string &relation, std::string label);

IntermediateCode *createGOTOCode(std::string &label);

IntermediateCode *createLabelCode(std::string &label);

IntermediateCode *createConstantCode(std::string &place, std::string &value);

IntermediateCode *createBinaryCode(std::string &result, std::string &oprand1,
                                   std::string &oprand2,
                                   std::string &binaryOperator);

IntermediateCode *createMinusCode(std::string &result, std::string &oprand1);

IntermediateCode *createFunctionDecCode(std::string &funtionName);

IntermediateCode *createParamCode(std::string &varName);

IntermediateCode *createArrayOffsetCode(std::string &result,
                                        std::string &oprand1,
                                        std::string &oprand2,
                                        std::string &binaryOperator);

IntermediateCode *createGetValueInAddressCode(std::string &result,
                                              std::string &oprand1);

IntermediateCode *createAssignValueInAddressCode(std::string &result,
                                                 std::string &oprand1);

IntermediateCode *createIntMultiF4Code(std::string &result,
                                       std::string &oprand1);

IntermediateCode *createADDRESS_ASSIGN_ADDRESSCode(std::string &result,
                                                   std::string &oprand1);

IntermediateCode *createArrayDecCode(
    std::string &arrayName, int size);  // can just handle one dim array

#endif  // CS323_PROJECT1_INTERMEDIATECODE_H
