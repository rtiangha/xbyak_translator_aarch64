/*******************************************************************************
 * Copyright 2020 FUJITSU LIMITED
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/
#include "test_generator2.h"

class TestPtnGenerator : public TestGenerator {
public:
  void setInitialRegValue() {
    /* Here modify arrays of inputGenReg, inputPredReg, inputZReg */
    setInputZregAllRandomHex();

    /* z31 - z29 are used as zTmpIdx - zTmp3Idx */
    inputPredReg[1] = (1 << 0);
    inputPredReg[2] = (1 << 0) | (1 << 6) | (uint64_t(1) << 15) | /* x86_64 */
                      (1 << 0) | (uint64_t(1) << 24) |
                      (uint64_t(1) << 60); /* aarch64 */
    inputPredReg[3] = inputPredReg[4] = inputPredReg[5] = inputPredReg[6] =
        inputPredReg[2];
    inputPredReg[7] = ~uint64_t(0);
  }

  void setCheckRegFlagAll() {
    /* Here modify arrays of checkGenRegMode, checkPredRegMode, checkZRegMode */
  }

  void genJitTestCode() {
    /* Here write JIT code with x86_64 mnemonic function to be tested. */
    /* z31 - z29 are used as zTmpIdx - zTmp3Idx */
    vpermd(Ymm(1) | k1, Ymm(30), Ymm(31));
    vpermd(Ymm(2) | k2, Ymm(30), Ymm(30));
    vpermd(Ymm(3) | k3, Ymm(3), Ymm(31));
    vpermd(Ymm(4) | k4, Ymm(30), Ymm(4));
    vpermd(Ymm(5) | k5, Ymm(5), Ymm(5));
    vpermd(Ymm(6) | k6, Ymm(30), Ymm(31));
    vpermd(Ymm(7) | k7, Ymm(30), Ymm(31));
  }
};

int main(int argc, char *argv[]) {
  /* Initializing arrays of inputData, inputGenReg, inputPredReg, inputZReg,
   * checkGenRegMode, checkPredRegMode,checkZRegMode */
  TestPtnGenerator gen;

  /* Set bool output_jit_on_, bool exec_jit_on_ = 0; */
  gen.parseArgs(argc, argv);

  /* Generate JIT code and get function pointer */
  void (*f)();
  if (gen.isOutputJitOn()) {
    f = (void (*)())gen.gen();
  }

  /* Dump generated JIT code to a binary file */
  gen.dumpJitCode();

  /* 1:Execute JIT code, 2:dump all register values, 3:dump register values to
   * be checked */
  if (gen.isExecJitOn()) {
    /* Before executing JIT code, dump inputData, inputGenReg, inputPredReg,
     * inputZReg. */
    gen.dumpInputReg();
    f();                 /* Execute JIT code */
    gen.dumpOutputReg(); /* Dump all register values */
    gen.dumpCheckReg();  /* Dump register values to be checked */
  }

  return 0;
}
