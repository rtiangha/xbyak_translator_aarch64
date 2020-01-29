#pragma once
#pragma GCC diagnostic warning "-Wunused-but-set-variable"
#pragma GCC diagnostic warning "-Wunused-variable"

#define W_TMP_0 w25
#define X_TMP_0 x25
#define X_TMP_1 x26
#define X_TMP_2 x27
#define X_TMP_3 x28
#define X_TMP_ADDR x29
#define P_TMP_0 p9
#define P_LSB_128 p10
#define P_LSB_256 p11
#define P_MSB_256 p12
#define P_MSB_384 p13
#define P_ALL_ZERO p14
#define P_ALL_ONE p15

typedef unsigned int xt_reg_idx_t;
typedef xed_int64_t xt_disp_t;
typedef xed_uint_t xt_scale_t;

#define XT_REG_INVALID std::numeric_limits<xt_reg_idx_t>::max()
#define XT_DISP_INVALID std::numeric_limits<xt_disp_t>::max()
#define XT_SCALE_INVALID std::numeric_limits<xt_scale_t>::max()

#define XT_UNIMPLEMENTED                                                       \
  std::cerr << __FILE__ << ":" << __LINE__ << ":Unimplemented" << std::endl;   \
  assert(NULL);

enum x64_inst_t {
  X64_NO_ASSIGN = 0,
  X64_VBROADCASTSS = 0x18,
};

enum ModRM_t {
  DST_ADDR = 0,
  DST_ADDR_DISP8 = 1,
  DST_ADDR_DISP32 = 2,
  DST_SRC = 3,
};

enum VecLen_t {
  VL128 = 0,
  VL256 = 1,
  VL512 = 2,
};

enum xt_predicate_type_t {
  A64_PRED_INIT = 0,
  A64_PRED_NO,
  A64_PRED_MERG,
  A64_PRED_ZERO,
};

enum xt_operand_type_t {
  A64_OP_INIT = 0,
  A64_OP_REG,
  A64_OP_MEM,
  A64_OP_MBCST,
};


void db_clear() { CodeArray::size_ = 0; }

xt_reg_idx_t xt_get_register_index(const xed_decoded_inst_t *p,
                                    unsigned int i) {
  const xed_inst_t *xi = xed_decoded_inst_inst(p);
  const xed_operand_t *op = xed_inst_operand(xi, i);
  xed_operand_enum_t op_name = xed_operand_name(op);
  xed_reg_enum_t r = xed_decoded_inst_get_reg(p, op_name);

  if (XED_REG_RAX <= r && r <= XED_REG_R15) {
    return r - XED_REG_RAX;
  } else if (XED_REG_K0 <= r && r <= XED_REG_K7) {
    return r - XED_REG_K0;
  } else if (XED_REG_MMX0 <= r && r <= XED_REG_MMX7) {
    return r - XED_REG_MMX0;
  } else if (XED_REG_XMM0 <= r && r <= XED_REG_XMM31) {
    return r - XED_REG_XMM0;
  } else if (XED_REG_YMM0 <= r && r <= XED_REG_YMM31) {
    return r - XED_REG_YMM0;
  } else if (XED_REG_ZMM0 <= r && r <= XED_REG_ZMM31) {
    return r - XED_REG_ZMM0;
  } else if (XED_REG_EAX <= r && r <= XED_REG_R15D) {
    return r - XED_REG_EAX;
  } else {
    std::cerr << __FILE__ << ":" << __LINE__ << ":Under construction!"
              << std::endl;
    exit(1);
  }
}

unsigned int xt_get_register_index(const xed_reg_enum_t r) {
  if (XED_REG_RAX <= r && r <= XED_REG_R15) {
    return r - XED_REG_RAX;
  } else if (XED_REG_K0 <= r && r <= XED_REG_K7) {
    return r - XED_REG_K0;
  } else if (XED_REG_MMX0 <= r && r <= XED_REG_MMX7) {
    return r - XED_REG_MMX0;
  } else if (XED_REG_XMM0 <= r && r <= XED_REG_XMM31) {
    return r - XED_REG_XMM0;
  } else if (XED_REG_YMM0 <= r && r <= XED_REG_YMM31) {
    return r - XED_REG_YMM0;
  } else if (XED_REG_ZMM0 <= r && r <= XED_REG_ZMM31) {
    return r - XED_REG_ZMM0;
  } else {
    std::cerr << __FILE__ << ":" << __LINE__ << ":Under construction!"
              << std::endl;
    exit(1);
  }
}

void xt_decode_memory_operand(const xed_decoded_inst_t *p, unsigned int i,
                               unsigned int *base, xed_int64_t *disp,
                               unsigned int *index, xed_uint_t *scale,
                               unsigned int *seg) {
  unsigned int width = xed_decoded_inst_get_memop_address_width(p, i);
  unsigned int memops = xed_decoded_inst_number_of_memory_operands(p);

  *base = XT_REG_INVALID;
  *index = XT_REG_INVALID;
  *seg = XT_REG_INVALID;

  if (width != 64) {
    std::cerr << __FILE__ << ":" << __LINE__
              << ":Unsupported address width=" << width
              << ". Please contact to system administrator!" << std::endl;
    exit(1);
  }

  for (unsigned int j = 0; j < memops; j++) {
    xed_reg_enum_t tmpReg;
    xed_int64_t tmpDisp;
    xed_uint32_t tmpScale;

    tmpReg = xed_decoded_inst_get_seg_reg(p, j);
    if (tmpReg != XED_REG_INVALID) {
      *seg = xt_get_register_index(tmpReg);
    }

    tmpReg = xed_decoded_inst_get_base_reg(p, j);
    if (tmpReg != XED_REG_INVALID) {
      *base = xt_get_register_index(tmpReg);
    }

    tmpReg = xed_decoded_inst_get_index_reg(p, j);
    if (tmpReg != XED_REG_INVALID) {
      *index = xt_get_register_index(tmpReg);
      tmpScale = xed_decoded_inst_get_scale(p, j);
      if (tmpScale) {
        *scale = tmpScale;
      }
    }

    tmpDisp = xed_decoded_inst_get_memory_displacement(p, j);
    if (tmpDisp) {
      *disp = tmpDisp;
    }
  }
}

void xt_decode_memory_operand_designated(const xed_decoded_inst_t *p,
                                          unsigned int i, unsigned int *base,
                                          xed_int64_t *disp,
                                          unsigned int *index,
                                          xed_uint_t *scale,
                                          unsigned int *seg) {
  unsigned int width = xed_decoded_inst_get_memop_address_width(p, i);
  unsigned int memops = xed_decoded_inst_number_of_memory_operands(p);

  *base = XT_REG_INVALID;
  *index = XT_REG_INVALID;
  *seg = XT_REG_INVALID;

  *scale = XT_SCALE_INVALID;
  *disp = XT_DISP_INVALID;

  if (width != 64) {
    std::cerr << __FILE__ << ":" << __LINE__
              << ":Unsupported address width=" << width
              << ". Please contact to system administrator!" << std::endl;
    exit(1);
  }

  xed_reg_enum_t tmpReg;
  xed_int64_t tmpDisp;
  xed_uint32_t tmpScale;

  tmpReg = xed_decoded_inst_get_seg_reg(p, i);
  if (tmpReg != XED_REG_INVALID) {
    *seg = xt_get_register_index(tmpReg);
  }

  tmpReg = xed_decoded_inst_get_base_reg(p, i);
  if (tmpReg != XED_REG_INVALID) {
    *base = xt_get_register_index(tmpReg);
  }

  tmpReg = xed_decoded_inst_get_index_reg(p, i);
  if (tmpReg != XED_REG_INVALID) {
    *index = xt_get_register_index(tmpReg);
    tmpScale = xed_decoded_inst_get_scale(p, i);
    if (tmpScale) {
      *scale = tmpScale;
    }
  }

  tmpDisp = xed_decoded_inst_get_memory_displacement(p, i);
  if (tmpDisp) {
    *disp = tmpDisp;
  }
}

Xbyak_aarch64::XReg xt_get_addr_reg(unsigned int base, xed_int64_t disp,
                                     unsigned int index, xed_uint_t scale,
                                     const Xbyak_aarch64::XReg tmp0,
                                     const Xbyak_aarch64::XReg tmp1) {

  unsigned int shift = 0;
  if (scale == 0) {
    /* Nothing to do */
  } else if (scale == 2) {
    shift = 1;
  } else if (scale == 4) {
    shift = 2;
  } else if (scale == 8) {
    shift = 3;
  } else {
    std::cerr << __FILE__ << ":" << __LINE__ << ":scale=" << scale
              << " is assumed to 2, 4, or 8!" << std::endl;
    exit(1);
  }

  Xbyak_aarch64::XReg retReg(tmp0);

  if (base != XT_REG_INVALID /* Base only */
      && disp == 0 && index == XT_REG_INVALID) {
    return Xbyak_aarch64::XReg(base);

  } else if (base != XT_REG_INVALID && disp != 0 /* Base + disp */
             && index == XT_REG_INVALID) {
    add_imm(retReg, Xbyak_aarch64::XReg(base), disp, tmp1);
    return retReg;
  } else if (base != XT_REG_INVALID && disp != 0 &&
             index != XT_REG_INVALID) { /* Base + disp + index (*scale) */
    add_imm(retReg, Xbyak_aarch64::XReg(base), disp, tmp1);

    if (shift == 0) {
        CodeGeneratorAArch64::add(retReg, retReg, Xbyak_aarch64::XReg(index));
      return retReg; /* Base + disp + index */
    } else {
      lsl(tmp1, Xbyak_aarch64::XReg(index), shift);
      CodeGeneratorAArch64::add(retReg, Xbyak_aarch64::XReg(base), tmp1);
      return retReg; /* Base + disp + index*scale */
    }
  } else if (base == XT_REG_INVALID /* disp + index (*scale) */
             && index != XT_REG_INVALID && disp != 0) {
    if (shift == 0) {
      add_imm(retReg, Xbyak_aarch64::XReg(index), disp, tmp1);
      return retReg; /* disp + index */
    } else {
      lsl(tmp1, Xbyak_aarch64::XReg(index), shift);
      CodeGeneratorAArch64::add(retReg, Xbyak_aarch64::XReg(index), tmp1);
      return retReg; /* disp + index*scale */
    }
  } else {
    std::cerr << __FILE__ << ":" << __LINE__ << ":Something wrong"
              << ". Please contact to system administrator!" << std::endl;
    exit(1);
  }

  return retReg;
}

unsigned int xt_push_vreg() {
  return 31;
}

unsigned int xt_push_zreg() {
  return 31;
}

void xt_pop_vreg() {
}
    
void xt_pop_zreg() {
}

#include "xbyak_translate_inc.h"

bool decodeOpcode() {
  xed_state_t dstate;
  xed_decoded_inst_t xedd;
  xed_error_enum_t xed_error;
  xed_chip_enum_t chip = XED_CHIP_INVALID;

  xed_tables_init();
  xed_state_zero(&dstate);

  dstate.mmode = XED_MACHINE_MODE_LONG_64;
  dstate.stack_addr_width = XED_ADDRESS_WIDTH_32b;

  xed_decoded_inst_zero_set_mode(&xedd, &dstate);
  xed_decoded_inst_set_input_chip(&xedd, chip);

  xed3_operand_set_mpxmode(&xedd, 0);
  xed3_operand_set_cet(&xedd, 0);

#if 0
  CodeArray::top_[0] = 0x62;
  CodeArray::top_[1] = 0xf2;
  CodeArray::top_[2] = 0x7d;
  CodeArray::top_[3] = 0x09;
  CodeArray::top_[4] = 0x18;
  CodeArray::top_[5] = 0xc2;
  CodeArray::size_ = 6;
#endif
#if 0
  CodeArray::top_[0] = 0x62;
  CodeArray::top_[1] = 0xf2;
  CodeArray::top_[2] = 0x7d;
  CodeArray::top_[3] = 0x09;
  CodeArray::top_[4] = 0x18;
  CodeArray::top_[5] = 0x44;
  CodeArray::top_[6] = 0x98;
  CodeArray::top_[7] = 0x0f;
  CodeArray::size_ = 8;
#endif

  printf("Attempting to decode: ");
  for (unsigned int i = 0; i < CodeArray::size_; i++)
    printf("%02x ", XED_STATIC_CAST(xed_uint_t, CodeArray::top_[i]));
  printf("\n");

  xed_error = xed_decode(
      &xedd, XED_REINTERPRET_CAST(const xed_uint8_t *, CodeArray::top_),
      CodeArray::size_);

  switch (xed_error) {
  case XED_ERROR_NONE:
    break;
  case XED_ERROR_BUFFER_TOO_SHORT:
    printf("Not enough bytes provided\n");
    exit(1);
  case XED_ERROR_INVALID_FOR_CHIP:
    printf("The instruction was not valid for the specified chip.\n");
    exit(1);
  case XED_ERROR_GENERAL_ERROR:
    printf("Could not decode given input.\n");
    exit(1);
  default:
    printf("Unhandled error code %s\n", xed_error_enum_t2str(xed_error));
    exit(1);
  }

  printf("iclass %s\t",
         xed_iclass_enum_t2str(xed_decoded_inst_get_iclass(&xedd)));
  printf("category %s\t",
         xed_category_enum_t2str(xed_decoded_inst_get_category(&xedd)));
  printf("ISA-extension %s\t",
         xed_extension_enum_t2str(xed_decoded_inst_get_extension(&xedd)));
  printf("ISA-set %s\n",
         xed_isa_set_enum_t2str(xed_decoded_inst_get_isa_set(&xedd)));
  printf("instruction-length %u\n", xed_decoded_inst_get_length(&xedd));
  printf("operand-width %u\n", xed_decoded_inst_get_operand_width(&xedd));
  printf("effective-operand-width %u\n",
         xed_operand_values_get_effective_operand_width(
             xed_decoded_inst_operands_const(&xedd)));
  printf("effective-address-width %u\n",
         xed_operand_values_get_effective_address_width(
             xed_decoded_inst_operands_const(&xedd)));
  printf("stack-address-width %u\n",
         xed_operand_values_get_stack_address_width(
             xed_decoded_inst_operands_const(&xedd)));
  printf("iform-enum-name %s\n",
         xed_iform_enum_t2str(xed_decoded_inst_get_iform_enum(&xedd)));
  printf("iform-enum-name-dispatch (zero based) %u\n",
         xed_decoded_inst_get_iform_enum_dispatch(&xedd));
  printf("iclass-max-iform-dispatch %u\n",
         xed_iform_max_per_iclass(xed_decoded_inst_get_iclass(&xedd)));

  // operands
  print_operands(&xedd);

  // memops
  print_memops(&xedd);

#if 0
  // flags
  print_flags(&xedd);
  print_reads_zf_flag(&xedd);

  // attributes
  print_attributes(&xedd);

  // misc
  print_misc(&xedd);
#endif

  xed_iclass_enum_t p = xed_decoded_inst_get_iclass(&xedd);

  switch (p) {
  case XED_ICLASS_INVALID:
  case XED_ICLASS_AAA:
  case XED_ICLASS_AAD:
  case XED_ICLASS_AAM:
  case XED_ICLASS_AAS:
  case XED_ICLASS_ADC:
  case XED_ICLASS_ADCX:
  case XED_ICLASS_ADC_LOCK:
  case XED_ICLASS_ADD:
    //    translateADD(&xedd);
    break;

  case XED_ICLASS_ADDPD:
  case XED_ICLASS_ADDPS:
  case XED_ICLASS_ADDSD:
  case XED_ICLASS_ADDSS:
  case XED_ICLASS_ADDSUBPD:
  case XED_ICLASS_ADDSUBPS:
  case XED_ICLASS_ADD_LOCK:
  case XED_ICLASS_ADOX:
  case XED_ICLASS_AESDEC:
  case XED_ICLASS_AESDECLAST:
  case XED_ICLASS_AESENC:
  case XED_ICLASS_AESENCLAST:
  case XED_ICLASS_AESIMC:
  case XED_ICLASS_AESKEYGENASSIST:
  case XED_ICLASS_AND:
  case XED_ICLASS_ANDN:
  case XED_ICLASS_ANDNPD:
  case XED_ICLASS_ANDNPS:
  case XED_ICLASS_ANDPD:
  case XED_ICLASS_ANDPS:
  case XED_ICLASS_AND_LOCK:
  case XED_ICLASS_ARPL:
  case XED_ICLASS_BEXTR:
  case XED_ICLASS_BEXTR_XOP:
  case XED_ICLASS_BLCFILL:
  case XED_ICLASS_BLCI:
  case XED_ICLASS_BLCIC:
  case XED_ICLASS_BLCMSK:
  case XED_ICLASS_BLCS:
  case XED_ICLASS_BLENDPD:
  case XED_ICLASS_BLENDPS:
  case XED_ICLASS_BLENDVPD:
  case XED_ICLASS_BLENDVPS:
  case XED_ICLASS_BLSFILL:
  case XED_ICLASS_BLSI:
  case XED_ICLASS_BLSIC:
  case XED_ICLASS_BLSMSK:
  case XED_ICLASS_BLSR:
  case XED_ICLASS_BNDCL:
  case XED_ICLASS_BNDCN:
  case XED_ICLASS_BNDCU:
  case XED_ICLASS_BNDLDX:
  case XED_ICLASS_BNDMK:
  case XED_ICLASS_BNDMOV:
  case XED_ICLASS_BNDSTX:
  case XED_ICLASS_BOUND:
  case XED_ICLASS_BSF:
  case XED_ICLASS_BSR:
  case XED_ICLASS_BSWAP:
  case XED_ICLASS_BT:
  case XED_ICLASS_BTC:
  case XED_ICLASS_BTC_LOCK:
  case XED_ICLASS_BTR:
  case XED_ICLASS_BTR_LOCK:
  case XED_ICLASS_BTS:
  case XED_ICLASS_BTS_LOCK:
  case XED_ICLASS_BZHI:
  case XED_ICLASS_CALL_FAR:
  case XED_ICLASS_CALL_NEAR:
  case XED_ICLASS_CBW:
  case XED_ICLASS_CDQ:
  case XED_ICLASS_CDQE:
  case XED_ICLASS_CLAC:
  case XED_ICLASS_CLC:
  case XED_ICLASS_CLD:
  case XED_ICLASS_CLDEMOTE:
  case XED_ICLASS_CLFLUSH:
  case XED_ICLASS_CLFLUSHOPT:
  case XED_ICLASS_CLGI:
  case XED_ICLASS_CLI:
  case XED_ICLASS_CLRSSBSY:
  case XED_ICLASS_CLTS:
  case XED_ICLASS_CLWB:
  case XED_ICLASS_CLZERO:
  case XED_ICLASS_CMC:
  case XED_ICLASS_CMOVB:
  case XED_ICLASS_CMOVBE:
  case XED_ICLASS_CMOVL:
  case XED_ICLASS_CMOVLE:
  case XED_ICLASS_CMOVNB:
  case XED_ICLASS_CMOVNBE:
  case XED_ICLASS_CMOVNL:
  case XED_ICLASS_CMOVNLE:
  case XED_ICLASS_CMOVNO:
  case XED_ICLASS_CMOVNP:
  case XED_ICLASS_CMOVNS:
  case XED_ICLASS_CMOVNZ:
  case XED_ICLASS_CMOVO:
  case XED_ICLASS_CMOVP:
  case XED_ICLASS_CMOVS:
  case XED_ICLASS_CMOVZ:
  case XED_ICLASS_CMP:
    //    translateCMP(&xedd);
    break;

  case XED_ICLASS_CMPPD:
  case XED_ICLASS_CMPPS:
  case XED_ICLASS_CMPSB:
  case XED_ICLASS_CMPSD:
  case XED_ICLASS_CMPSD_XMM:
  case XED_ICLASS_CMPSQ:
  case XED_ICLASS_CMPSS:
  case XED_ICLASS_CMPSW:
  case XED_ICLASS_CMPXCHG:
  case XED_ICLASS_CMPXCHG16B:
  case XED_ICLASS_CMPXCHG16B_LOCK:
  case XED_ICLASS_CMPXCHG8B:
  case XED_ICLASS_CMPXCHG8B_LOCK:
  case XED_ICLASS_CMPXCHG_LOCK:
  case XED_ICLASS_COMISD:
  case XED_ICLASS_COMISS:
  case XED_ICLASS_CPUID:
  case XED_ICLASS_CQO:
  case XED_ICLASS_CRC32:
  case XED_ICLASS_CVTDQ2PD:
  case XED_ICLASS_CVTDQ2PS:
  case XED_ICLASS_CVTPD2DQ:
  case XED_ICLASS_CVTPD2PI:
  case XED_ICLASS_CVTPD2PS:
  case XED_ICLASS_CVTPI2PD:
  case XED_ICLASS_CVTPI2PS:
  case XED_ICLASS_CVTPS2DQ:
  case XED_ICLASS_CVTPS2PD:
  case XED_ICLASS_CVTPS2PI:
  case XED_ICLASS_CVTSD2SI:
  case XED_ICLASS_CVTSD2SS:
  case XED_ICLASS_CVTSI2SD:
  case XED_ICLASS_CVTSI2SS:
  case XED_ICLASS_CVTSS2SD:
  case XED_ICLASS_CVTSS2SI:
  case XED_ICLASS_CVTTPD2DQ:
  case XED_ICLASS_CVTTPD2PI:
  case XED_ICLASS_CVTTPS2DQ:
  case XED_ICLASS_CVTTPS2PI:
  case XED_ICLASS_CVTTSD2SI:
  case XED_ICLASS_CVTTSS2SI:
  case XED_ICLASS_CWD:
  case XED_ICLASS_CWDE:
  case XED_ICLASS_DAA:
  case XED_ICLASS_DAS:
  case XED_ICLASS_DEC:
  case XED_ICLASS_DEC_LOCK:
  case XED_ICLASS_DIV:
  case XED_ICLASS_DIVPD:
  case XED_ICLASS_DIVPS:
  case XED_ICLASS_DIVSD:
  case XED_ICLASS_DIVSS:
  case XED_ICLASS_DPPD:
  case XED_ICLASS_DPPS:
  case XED_ICLASS_EMMS:
  case XED_ICLASS_ENCLS:
  case XED_ICLASS_ENCLU:
  case XED_ICLASS_ENCLV:
  case XED_ICLASS_ENDBR32:
  case XED_ICLASS_ENDBR64:
  case XED_ICLASS_ENQCMD:
  case XED_ICLASS_ENQCMDS:
  case XED_ICLASS_ENTER:
  case XED_ICLASS_EXTRACTPS:
  case XED_ICLASS_EXTRQ:
  case XED_ICLASS_F2XM1:
  case XED_ICLASS_FABS:
  case XED_ICLASS_FADD:
  case XED_ICLASS_FADDP:
  case XED_ICLASS_FBLD:
  case XED_ICLASS_FBSTP:
  case XED_ICLASS_FCHS:
  case XED_ICLASS_FCMOVB:
  case XED_ICLASS_FCMOVBE:
  case XED_ICLASS_FCMOVE:
  case XED_ICLASS_FCMOVNB:
  case XED_ICLASS_FCMOVNBE:
  case XED_ICLASS_FCMOVNE:
  case XED_ICLASS_FCMOVNU:
  case XED_ICLASS_FCMOVU:
  case XED_ICLASS_FCOM:
  case XED_ICLASS_FCOMI:
  case XED_ICLASS_FCOMIP:
  case XED_ICLASS_FCOMP:
  case XED_ICLASS_FCOMPP:
  case XED_ICLASS_FCOS:
  case XED_ICLASS_FDECSTP:
  case XED_ICLASS_FDISI8087_NOP:
  case XED_ICLASS_FDIV:
  case XED_ICLASS_FDIVP:
  case XED_ICLASS_FDIVR:
  case XED_ICLASS_FDIVRP:
  case XED_ICLASS_FEMMS:
  case XED_ICLASS_FENI8087_NOP:
  case XED_ICLASS_FFREE:
  case XED_ICLASS_FFREEP:
  case XED_ICLASS_FIADD:
  case XED_ICLASS_FICOM:
  case XED_ICLASS_FICOMP:
  case XED_ICLASS_FIDIV:
  case XED_ICLASS_FIDIVR:
  case XED_ICLASS_FILD:
  case XED_ICLASS_FIMUL:
  case XED_ICLASS_FINCSTP:
  case XED_ICLASS_FIST:
  case XED_ICLASS_FISTP:
  case XED_ICLASS_FISTTP:
  case XED_ICLASS_FISUB:
  case XED_ICLASS_FISUBR:
  case XED_ICLASS_FLD:
  case XED_ICLASS_FLD1:
  case XED_ICLASS_FLDCW:
  case XED_ICLASS_FLDENV:
  case XED_ICLASS_FLDL2E:
  case XED_ICLASS_FLDL2T:
  case XED_ICLASS_FLDLG2:
  case XED_ICLASS_FLDLN2:
  case XED_ICLASS_FLDPI:
  case XED_ICLASS_FLDZ:
  case XED_ICLASS_FMUL:
  case XED_ICLASS_FMULP:
  case XED_ICLASS_FNCLEX:
  case XED_ICLASS_FNINIT:
  case XED_ICLASS_FNOP:
  case XED_ICLASS_FNSAVE:
  case XED_ICLASS_FNSTCW:
  case XED_ICLASS_FNSTENV:
  case XED_ICLASS_FNSTSW:
  case XED_ICLASS_FPATAN:
  case XED_ICLASS_FPREM:
  case XED_ICLASS_FPREM1:
  case XED_ICLASS_FPTAN:
  case XED_ICLASS_FRNDINT:
  case XED_ICLASS_FRSTOR:
  case XED_ICLASS_FSCALE:
  case XED_ICLASS_FSETPM287_NOP:
  case XED_ICLASS_FSIN:
  case XED_ICLASS_FSINCOS:
  case XED_ICLASS_FSQRT:
  case XED_ICLASS_FST:
  case XED_ICLASS_FSTP:
  case XED_ICLASS_FSTPNCE:
  case XED_ICLASS_FSUB:
  case XED_ICLASS_FSUBP:
  case XED_ICLASS_FSUBR:
  case XED_ICLASS_FSUBRP:
  case XED_ICLASS_FTST:
  case XED_ICLASS_FUCOM:
  case XED_ICLASS_FUCOMI:
  case XED_ICLASS_FUCOMIP:
  case XED_ICLASS_FUCOMP:
  case XED_ICLASS_FUCOMPP:
  case XED_ICLASS_FWAIT:
  case XED_ICLASS_FXAM:
  case XED_ICLASS_FXCH:
  case XED_ICLASS_FXRSTOR:
  case XED_ICLASS_FXRSTOR64:
  case XED_ICLASS_FXSAVE:
  case XED_ICLASS_FXSAVE64:
  case XED_ICLASS_FXTRACT:
  case XED_ICLASS_FYL2X:
  case XED_ICLASS_FYL2XP1:
  case XED_ICLASS_GETSEC:
  case XED_ICLASS_GF2P8AFFINEINVQB:
  case XED_ICLASS_GF2P8AFFINEQB:
  case XED_ICLASS_GF2P8MULB:
  case XED_ICLASS_HADDPD:
  case XED_ICLASS_HADDPS:
  case XED_ICLASS_HLT:
  case XED_ICLASS_HSUBPD:
  case XED_ICLASS_HSUBPS:
  case XED_ICLASS_IDIV:
  case XED_ICLASS_IMUL:
  case XED_ICLASS_IN:
  case XED_ICLASS_INC:
  case XED_ICLASS_INCSSPD:
  case XED_ICLASS_INCSSPQ:
  case XED_ICLASS_INC_LOCK:
  case XED_ICLASS_INSB:
  case XED_ICLASS_INSD:
  case XED_ICLASS_INSERTPS:
  case XED_ICLASS_INSERTQ:
  case XED_ICLASS_INSW:
  case XED_ICLASS_INT:
  case XED_ICLASS_INT1:
  case XED_ICLASS_INT3:
  case XED_ICLASS_INTO:
  case XED_ICLASS_INVD:
  case XED_ICLASS_INVEPT:
  case XED_ICLASS_INVLPG:
  case XED_ICLASS_INVLPGA:
  case XED_ICLASS_INVPCID:
  case XED_ICLASS_INVVPID:
  case XED_ICLASS_IRET:
  case XED_ICLASS_IRETD:
  case XED_ICLASS_IRETQ:
  case XED_ICLASS_JB:
  case XED_ICLASS_JBE:
  case XED_ICLASS_JCXZ:
  case XED_ICLASS_JECXZ:
  case XED_ICLASS_JL:
  case XED_ICLASS_JLE:
  case XED_ICLASS_JMP:
  case XED_ICLASS_JMP_FAR:
  case XED_ICLASS_JNB:
  case XED_ICLASS_JNBE:
  case XED_ICLASS_JNL:
  case XED_ICLASS_JNLE:
  case XED_ICLASS_JNO:
  case XED_ICLASS_JNP:
  case XED_ICLASS_JNS:
  case XED_ICLASS_JNZ:
  case XED_ICLASS_JO:
  case XED_ICLASS_JP:
  case XED_ICLASS_JRCXZ:
  case XED_ICLASS_JS:
  case XED_ICLASS_JZ:
  case XED_ICLASS_KADDB:
  case XED_ICLASS_KADDD:
  case XED_ICLASS_KADDQ:
  case XED_ICLASS_KADDW:
  case XED_ICLASS_KANDB:
  case XED_ICLASS_KANDD:
  case XED_ICLASS_KANDNB:
  case XED_ICLASS_KANDND:
  case XED_ICLASS_KANDNQ:
  case XED_ICLASS_KANDNW:
  case XED_ICLASS_KANDQ:
  case XED_ICLASS_KANDW:
  case XED_ICLASS_KMOVB:
  case XED_ICLASS_KMOVD:
  case XED_ICLASS_KMOVQ:
  case XED_ICLASS_KMOVW:
  case XED_ICLASS_KNOTB:
  case XED_ICLASS_KNOTD:
  case XED_ICLASS_KNOTQ:
  case XED_ICLASS_KNOTW:
  case XED_ICLASS_KORB:
  case XED_ICLASS_KORD:
  case XED_ICLASS_KORQ:
  case XED_ICLASS_KORTESTB:
  case XED_ICLASS_KORTESTD:
  case XED_ICLASS_KORTESTQ:
  case XED_ICLASS_KORTESTW:
  case XED_ICLASS_KORW:
  case XED_ICLASS_KSHIFTLB:
  case XED_ICLASS_KSHIFTLD:
  case XED_ICLASS_KSHIFTLQ:
  case XED_ICLASS_KSHIFTLW:
  case XED_ICLASS_KSHIFTRB:
  case XED_ICLASS_KSHIFTRD:
  case XED_ICLASS_KSHIFTRQ:
  case XED_ICLASS_KSHIFTRW:
  case XED_ICLASS_KTESTB:
  case XED_ICLASS_KTESTD:
  case XED_ICLASS_KTESTQ:
  case XED_ICLASS_KTESTW:
  case XED_ICLASS_KUNPCKBW:
  case XED_ICLASS_KUNPCKDQ:
  case XED_ICLASS_KUNPCKWD:
  case XED_ICLASS_KXNORB:
  case XED_ICLASS_KXNORD:
  case XED_ICLASS_KXNORQ:
  case XED_ICLASS_KXNORW:
  case XED_ICLASS_KXORB:
  case XED_ICLASS_KXORD:
  case XED_ICLASS_KXORQ:
  case XED_ICLASS_KXORW:
  case XED_ICLASS_LAHF:
  case XED_ICLASS_LAR:
  case XED_ICLASS_LDDQU:
  case XED_ICLASS_LDMXCSR:
  case XED_ICLASS_LDS:
  case XED_ICLASS_LEA:
  case XED_ICLASS_LEAVE:
  case XED_ICLASS_LES:
  case XED_ICLASS_LFENCE:
  case XED_ICLASS_LFS:
  case XED_ICLASS_LGDT:
  case XED_ICLASS_LGS:
  case XED_ICLASS_LIDT:
  case XED_ICLASS_LLDT:
  case XED_ICLASS_LLWPCB:
  case XED_ICLASS_LMSW:
  case XED_ICLASS_LODSB:
  case XED_ICLASS_LODSD:
  case XED_ICLASS_LODSQ:
  case XED_ICLASS_LODSW:
  case XED_ICLASS_LOOP:
  case XED_ICLASS_LOOPE:
  case XED_ICLASS_LOOPNE:
  case XED_ICLASS_LSL:
  case XED_ICLASS_LSS:
  case XED_ICLASS_LTR:
  case XED_ICLASS_LWPINS:
  case XED_ICLASS_LWPVAL:
  case XED_ICLASS_LZCNT:
  case XED_ICLASS_MASKMOVDQU:
  case XED_ICLASS_MASKMOVQ:
  case XED_ICLASS_MAXPD:
  case XED_ICLASS_MAXPS:
  case XED_ICLASS_MAXSD:
  case XED_ICLASS_MAXSS:
  case XED_ICLASS_MFENCE:
  case XED_ICLASS_MINPD:
  case XED_ICLASS_MINPS:
  case XED_ICLASS_MINSD:
  case XED_ICLASS_MINSS:
  case XED_ICLASS_MONITOR:
  case XED_ICLASS_MONITORX:
  case XED_ICLASS_MOV:
    //    translateMOV(&xedd);
    break;


  case XED_ICLASS_MOVAPD:
  case XED_ICLASS_MOVAPS:
  case XED_ICLASS_MOVBE:
  case XED_ICLASS_MOVD:
  case XED_ICLASS_MOVDDUP:
  case XED_ICLASS_MOVDIR64B:
  case XED_ICLASS_MOVDIRI:
  case XED_ICLASS_MOVDQ2Q:
  case XED_ICLASS_MOVDQA:
  case XED_ICLASS_MOVDQU:
  case XED_ICLASS_MOVHLPS:
  case XED_ICLASS_MOVHPD:
  case XED_ICLASS_MOVHPS:
  case XED_ICLASS_MOVLHPS:
  case XED_ICLASS_MOVLPD:
  case XED_ICLASS_MOVLPS:
  case XED_ICLASS_MOVMSKPD:
  case XED_ICLASS_MOVMSKPS:
  case XED_ICLASS_MOVNTDQ:
  case XED_ICLASS_MOVNTDQA:
  case XED_ICLASS_MOVNTI:
  case XED_ICLASS_MOVNTPD:
  case XED_ICLASS_MOVNTPS:
  case XED_ICLASS_MOVNTQ:
  case XED_ICLASS_MOVNTSD:
  case XED_ICLASS_MOVNTSS:
  case XED_ICLASS_MOVQ:
  case XED_ICLASS_MOVQ2DQ:
  case XED_ICLASS_MOVSB:
  case XED_ICLASS_MOVSD:
  case XED_ICLASS_MOVSD_XMM:
  case XED_ICLASS_MOVSHDUP:
  case XED_ICLASS_MOVSLDUP:
  case XED_ICLASS_MOVSQ:
  case XED_ICLASS_MOVSS:
  case XED_ICLASS_MOVSW:
  case XED_ICLASS_MOVSX:
  case XED_ICLASS_MOVSXD:
  case XED_ICLASS_MOVUPD:
  case XED_ICLASS_MOVUPS:
  case XED_ICLASS_MOVZX:
  case XED_ICLASS_MOV_CR:
  case XED_ICLASS_MOV_DR:
  case XED_ICLASS_MPSADBW:
  case XED_ICLASS_MUL:
  case XED_ICLASS_MULPD:
  case XED_ICLASS_MULPS:
  case XED_ICLASS_MULSD:
  case XED_ICLASS_MULSS:
  case XED_ICLASS_MULX:
  case XED_ICLASS_MWAIT:
  case XED_ICLASS_MWAITX:
  case XED_ICLASS_NEG:
  case XED_ICLASS_NEG_LOCK:
  case XED_ICLASS_NOP:
  case XED_ICLASS_NOP2:
  case XED_ICLASS_NOP3:
  case XED_ICLASS_NOP4:
  case XED_ICLASS_NOP5:
  case XED_ICLASS_NOP6:
  case XED_ICLASS_NOP7:
  case XED_ICLASS_NOP8:
  case XED_ICLASS_NOP9:
  case XED_ICLASS_NOT:
  case XED_ICLASS_NOT_LOCK:
  case XED_ICLASS_OR:
  case XED_ICLASS_ORPD:
  case XED_ICLASS_ORPS:
  case XED_ICLASS_OR_LOCK:
  case XED_ICLASS_OUT:
  case XED_ICLASS_OUTSB:
  case XED_ICLASS_OUTSD:
  case XED_ICLASS_OUTSW:
  case XED_ICLASS_PABSB:
  case XED_ICLASS_PABSD:
  case XED_ICLASS_PABSW:
  case XED_ICLASS_PACKSSDW:
  case XED_ICLASS_PACKSSWB:
  case XED_ICLASS_PACKUSDW:
  case XED_ICLASS_PACKUSWB:
  case XED_ICLASS_PADDB:
  case XED_ICLASS_PADDD:
  case XED_ICLASS_PADDQ:
  case XED_ICLASS_PADDSB:
  case XED_ICLASS_PADDSW:
  case XED_ICLASS_PADDUSB:
  case XED_ICLASS_PADDUSW:
  case XED_ICLASS_PADDW:
  case XED_ICLASS_PALIGNR:
  case XED_ICLASS_PAND:
  case XED_ICLASS_PANDN:
  case XED_ICLASS_PAUSE:
  case XED_ICLASS_PAVGB:
  case XED_ICLASS_PAVGUSB:
  case XED_ICLASS_PAVGW:
  case XED_ICLASS_PBLENDVB:
  case XED_ICLASS_PBLENDW:
  case XED_ICLASS_PCLMULQDQ:
  case XED_ICLASS_PCMPEQB:
  case XED_ICLASS_PCMPEQD:
  case XED_ICLASS_PCMPEQQ:
  case XED_ICLASS_PCMPEQW:
  case XED_ICLASS_PCMPESTRI:
  case XED_ICLASS_PCMPESTRI64:
  case XED_ICLASS_PCMPESTRM:
  case XED_ICLASS_PCMPESTRM64:
  case XED_ICLASS_PCMPGTB:
  case XED_ICLASS_PCMPGTD:
  case XED_ICLASS_PCMPGTQ:
  case XED_ICLASS_PCMPGTW:
  case XED_ICLASS_PCMPISTRI:
  case XED_ICLASS_PCMPISTRI64:
  case XED_ICLASS_PCMPISTRM:
  case XED_ICLASS_PCONFIG:
  case XED_ICLASS_PDEP:
  case XED_ICLASS_PEXT:
  case XED_ICLASS_PEXTRB:
  case XED_ICLASS_PEXTRD:
  case XED_ICLASS_PEXTRQ:
  case XED_ICLASS_PEXTRW:
  case XED_ICLASS_PEXTRW_SSE4:
  case XED_ICLASS_PF2ID:
  case XED_ICLASS_PF2IW:
  case XED_ICLASS_PFACC:
  case XED_ICLASS_PFADD:
  case XED_ICLASS_PFCMPEQ:
  case XED_ICLASS_PFCMPGE:
  case XED_ICLASS_PFCMPGT:
  case XED_ICLASS_PFMAX:
  case XED_ICLASS_PFMIN:
  case XED_ICLASS_PFMUL:
  case XED_ICLASS_PFNACC:
  case XED_ICLASS_PFPNACC:
  case XED_ICLASS_PFRCP:
  case XED_ICLASS_PFRCPIT1:
  case XED_ICLASS_PFRCPIT2:
  case XED_ICLASS_PFRSQIT1:
  case XED_ICLASS_PFRSQRT:
  case XED_ICLASS_PFSUB:
  case XED_ICLASS_PFSUBR:
  case XED_ICLASS_PHADDD:
  case XED_ICLASS_PHADDSW:
  case XED_ICLASS_PHADDW:
  case XED_ICLASS_PHMINPOSUW:
  case XED_ICLASS_PHSUBD:
  case XED_ICLASS_PHSUBSW:
  case XED_ICLASS_PHSUBW:
  case XED_ICLASS_PI2FD:
  case XED_ICLASS_PI2FW:
  case XED_ICLASS_PINSRB:
  case XED_ICLASS_PINSRD:
  case XED_ICLASS_PINSRQ:
  case XED_ICLASS_PINSRW:
  case XED_ICLASS_PMADDUBSW:
  case XED_ICLASS_PMADDWD:
  case XED_ICLASS_PMAXSB:
  case XED_ICLASS_PMAXSD:
  case XED_ICLASS_PMAXSW:
  case XED_ICLASS_PMAXUB:
  case XED_ICLASS_PMAXUD:
  case XED_ICLASS_PMAXUW:
  case XED_ICLASS_PMINSB:
  case XED_ICLASS_PMINSD:
  case XED_ICLASS_PMINSW:
  case XED_ICLASS_PMINUB:
  case XED_ICLASS_PMINUD:
  case XED_ICLASS_PMINUW:
  case XED_ICLASS_PMOVMSKB:
  case XED_ICLASS_PMOVSXBD:
  case XED_ICLASS_PMOVSXBQ:
  case XED_ICLASS_PMOVSXBW:
  case XED_ICLASS_PMOVSXDQ:
  case XED_ICLASS_PMOVSXWD:
  case XED_ICLASS_PMOVSXWQ:
  case XED_ICLASS_PMOVZXBD:
  case XED_ICLASS_PMOVZXBQ:
  case XED_ICLASS_PMOVZXBW:
  case XED_ICLASS_PMOVZXDQ:
  case XED_ICLASS_PMOVZXWD:
  case XED_ICLASS_PMOVZXWQ:
  case XED_ICLASS_PMULDQ:
  case XED_ICLASS_PMULHRSW:
  case XED_ICLASS_PMULHRW:
  case XED_ICLASS_PMULHUW:
  case XED_ICLASS_PMULHW:
  case XED_ICLASS_PMULLD:
  case XED_ICLASS_PMULLW:
  case XED_ICLASS_PMULUDQ:
  case XED_ICLASS_POP:
  case XED_ICLASS_POPA:
  case XED_ICLASS_POPAD:
  case XED_ICLASS_POPCNT:
  case XED_ICLASS_POPF:
  case XED_ICLASS_POPFD:
  case XED_ICLASS_POPFQ:
  case XED_ICLASS_POR:
  case XED_ICLASS_PREFETCHNTA:
  case XED_ICLASS_PREFETCHT0:
  case XED_ICLASS_PREFETCHT1:
  case XED_ICLASS_PREFETCHT2:
  case XED_ICLASS_PREFETCHW:
  case XED_ICLASS_PREFETCHWT1:
  case XED_ICLASS_PREFETCH_EXCLUSIVE:
  case XED_ICLASS_PREFETCH_RESERVED:
  case XED_ICLASS_PSADBW:
  case XED_ICLASS_PSHUFB:
  case XED_ICLASS_PSHUFD:
  case XED_ICLASS_PSHUFHW:
  case XED_ICLASS_PSHUFLW:
  case XED_ICLASS_PSHUFW:
  case XED_ICLASS_PSIGNB:
  case XED_ICLASS_PSIGND:
  case XED_ICLASS_PSIGNW:
  case XED_ICLASS_PSLLD:
  case XED_ICLASS_PSLLDQ:
  case XED_ICLASS_PSLLQ:
  case XED_ICLASS_PSLLW:
  case XED_ICLASS_PSRAD:
  case XED_ICLASS_PSRAW:
  case XED_ICLASS_PSRLD:
  case XED_ICLASS_PSRLDQ:
  case XED_ICLASS_PSRLQ:
  case XED_ICLASS_PSRLW:
  case XED_ICLASS_PSUBB:
  case XED_ICLASS_PSUBD:
  case XED_ICLASS_PSUBQ:
  case XED_ICLASS_PSUBSB:
  case XED_ICLASS_PSUBSW:
  case XED_ICLASS_PSUBUSB:
  case XED_ICLASS_PSUBUSW:
  case XED_ICLASS_PSUBW:
  case XED_ICLASS_PSWAPD:
  case XED_ICLASS_PTEST:
  case XED_ICLASS_PTWRITE:
  case XED_ICLASS_PUNPCKHBW:
  case XED_ICLASS_PUNPCKHDQ:
  case XED_ICLASS_PUNPCKHQDQ:
  case XED_ICLASS_PUNPCKHWD:
  case XED_ICLASS_PUNPCKLBW:
  case XED_ICLASS_PUNPCKLDQ:
  case XED_ICLASS_PUNPCKLQDQ:
  case XED_ICLASS_PUNPCKLWD:
  case XED_ICLASS_PUSH:
  case XED_ICLASS_PUSHA:
  case XED_ICLASS_PUSHAD:
  case XED_ICLASS_PUSHF:
  case XED_ICLASS_PUSHFD:
  case XED_ICLASS_PUSHFQ:
  case XED_ICLASS_PXOR:
  case XED_ICLASS_RCL:
  case XED_ICLASS_RCPPS:
  case XED_ICLASS_RCPSS:
  case XED_ICLASS_RCR:
  case XED_ICLASS_RDFSBASE:
  case XED_ICLASS_RDGSBASE:
  case XED_ICLASS_RDMSR:
  case XED_ICLASS_RDPID:
  case XED_ICLASS_RDPKRU:
  case XED_ICLASS_RDPMC:
  case XED_ICLASS_RDPRU:
  case XED_ICLASS_RDRAND:
  case XED_ICLASS_RDSEED:
  case XED_ICLASS_RDSSPD:
  case XED_ICLASS_RDSSPQ:
  case XED_ICLASS_RDTSC:
  case XED_ICLASS_RDTSCP:
  case XED_ICLASS_REPE_CMPSB:
  case XED_ICLASS_REPE_CMPSD:
  case XED_ICLASS_REPE_CMPSQ:
  case XED_ICLASS_REPE_CMPSW:
  case XED_ICLASS_REPE_SCASB:
  case XED_ICLASS_REPE_SCASD:
  case XED_ICLASS_REPE_SCASQ:
  case XED_ICLASS_REPE_SCASW:
  case XED_ICLASS_REPNE_CMPSB:
  case XED_ICLASS_REPNE_CMPSD:
  case XED_ICLASS_REPNE_CMPSQ:
  case XED_ICLASS_REPNE_CMPSW:
  case XED_ICLASS_REPNE_SCASB:
  case XED_ICLASS_REPNE_SCASD:
  case XED_ICLASS_REPNE_SCASQ:
  case XED_ICLASS_REPNE_SCASW:
  case XED_ICLASS_REP_INSB:
  case XED_ICLASS_REP_INSD:
  case XED_ICLASS_REP_INSW:
  case XED_ICLASS_REP_LODSB:
  case XED_ICLASS_REP_LODSD:
  case XED_ICLASS_REP_LODSQ:
  case XED_ICLASS_REP_LODSW:
  case XED_ICLASS_REP_MONTMUL:
  case XED_ICLASS_REP_MOVSB:
  case XED_ICLASS_REP_MOVSD:
  case XED_ICLASS_REP_MOVSQ:
  case XED_ICLASS_REP_MOVSW:
  case XED_ICLASS_REP_OUTSB:
  case XED_ICLASS_REP_OUTSD:
  case XED_ICLASS_REP_OUTSW:
  case XED_ICLASS_REP_STOSB:
  case XED_ICLASS_REP_STOSD:
  case XED_ICLASS_REP_STOSQ:
  case XED_ICLASS_REP_STOSW:
  case XED_ICLASS_REP_XCRYPTCBC:
  case XED_ICLASS_REP_XCRYPTCFB:
  case XED_ICLASS_REP_XCRYPTCTR:
  case XED_ICLASS_REP_XCRYPTECB:
  case XED_ICLASS_REP_XCRYPTOFB:
  case XED_ICLASS_REP_XSHA1:
  case XED_ICLASS_REP_XSHA256:
  case XED_ICLASS_REP_XSTORE:
  case XED_ICLASS_RET_FAR:
  case XED_ICLASS_RET_NEAR:
  case XED_ICLASS_ROL:
  case XED_ICLASS_ROR:
  case XED_ICLASS_RORX:
  case XED_ICLASS_ROUNDPD:
  case XED_ICLASS_ROUNDPS:
  case XED_ICLASS_ROUNDSD:
  case XED_ICLASS_ROUNDSS:
  case XED_ICLASS_RSM:
  case XED_ICLASS_RSQRTPS:
  case XED_ICLASS_RSQRTSS:
  case XED_ICLASS_RSTORSSP:
  case XED_ICLASS_SAHF:
  case XED_ICLASS_SALC:
  case XED_ICLASS_SAR:
  case XED_ICLASS_SARX:
  case XED_ICLASS_SAVEPREVSSP:
  case XED_ICLASS_SBB:
  case XED_ICLASS_SBB_LOCK:
  case XED_ICLASS_SCASB:
  case XED_ICLASS_SCASD:
  case XED_ICLASS_SCASQ:
  case XED_ICLASS_SCASW:
  case XED_ICLASS_SETB:
  case XED_ICLASS_SETBE:
  case XED_ICLASS_SETL:
  case XED_ICLASS_SETLE:
  case XED_ICLASS_SETNB:
  case XED_ICLASS_SETNBE:
  case XED_ICLASS_SETNL:
  case XED_ICLASS_SETNLE:
  case XED_ICLASS_SETNO:
  case XED_ICLASS_SETNP:
  case XED_ICLASS_SETNS:
  case XED_ICLASS_SETNZ:
  case XED_ICLASS_SETO:
  case XED_ICLASS_SETP:
  case XED_ICLASS_SETS:
  case XED_ICLASS_SETSSBSY:
  case XED_ICLASS_SETZ:
  case XED_ICLASS_SFENCE:
  case XED_ICLASS_SGDT:
  case XED_ICLASS_SHA1MSG1:
  case XED_ICLASS_SHA1MSG2:
  case XED_ICLASS_SHA1NEXTE:
  case XED_ICLASS_SHA1RNDS4:
  case XED_ICLASS_SHA256MSG1:
  case XED_ICLASS_SHA256MSG2:
  case XED_ICLASS_SHA256RNDS2:
  case XED_ICLASS_SHL:
  case XED_ICLASS_SHLD:
  case XED_ICLASS_SHLX:
  case XED_ICLASS_SHR:
  case XED_ICLASS_SHRD:
  case XED_ICLASS_SHRX:
  case XED_ICLASS_SHUFPD:
  case XED_ICLASS_SHUFPS:
  case XED_ICLASS_SIDT:
  case XED_ICLASS_SKINIT:
  case XED_ICLASS_SLDT:
  case XED_ICLASS_SLWPCB:
  case XED_ICLASS_SMSW:
  case XED_ICLASS_SQRTPD:
  case XED_ICLASS_SQRTPS:
  case XED_ICLASS_SQRTSD:
  case XED_ICLASS_SQRTSS:
  case XED_ICLASS_STAC:
  case XED_ICLASS_STC:
  case XED_ICLASS_STD:
  case XED_ICLASS_STGI:
  case XED_ICLASS_STI:
  case XED_ICLASS_STMXCSR:
  case XED_ICLASS_STOSB:
  case XED_ICLASS_STOSD:
  case XED_ICLASS_STOSQ:
  case XED_ICLASS_STOSW:
  case XED_ICLASS_STR:
  case XED_ICLASS_SUB:
    //    translateSUB(&xedd);
    break;

  case XED_ICLASS_SUBPD:
  case XED_ICLASS_SUBPS:
  case XED_ICLASS_SUBSD:
  case XED_ICLASS_SUBSS:
  case XED_ICLASS_SUB_LOCK:
  case XED_ICLASS_SWAPGS:
  case XED_ICLASS_SYSCALL:
  case XED_ICLASS_SYSCALL_AMD:
  case XED_ICLASS_SYSENTER:
  case XED_ICLASS_SYSEXIT:
  case XED_ICLASS_SYSRET:
  case XED_ICLASS_SYSRET64:
  case XED_ICLASS_SYSRET_AMD:
  case XED_ICLASS_T1MSKC:
  case XED_ICLASS_TEST:
  case XED_ICLASS_TPAUSE:
  case XED_ICLASS_TZCNT:
  case XED_ICLASS_TZMSK:
  case XED_ICLASS_UCOMISD:
  case XED_ICLASS_UCOMISS:
  case XED_ICLASS_UD0:
  case XED_ICLASS_UD1:
  case XED_ICLASS_UD2:
  case XED_ICLASS_UMONITOR:
  case XED_ICLASS_UMWAIT:
  case XED_ICLASS_UNPCKHPD:
  case XED_ICLASS_UNPCKHPS:
  case XED_ICLASS_UNPCKLPD:
  case XED_ICLASS_UNPCKLPS:
  case XED_ICLASS_V4FMADDPS:
  case XED_ICLASS_V4FMADDSS:
  case XED_ICLASS_V4FNMADDPS:
  case XED_ICLASS_V4FNMADDSS:
  case XED_ICLASS_VADDPD:
  case XED_ICLASS_VADDPS:
  case XED_ICLASS_VADDSD:
  case XED_ICLASS_VADDSS:
  case XED_ICLASS_VADDSUBPD:
  case XED_ICLASS_VADDSUBPS:
  case XED_ICLASS_VAESDEC:
  case XED_ICLASS_VAESDECLAST:
  case XED_ICLASS_VAESENC:
  case XED_ICLASS_VAESENCLAST:
  case XED_ICLASS_VAESIMC:
  case XED_ICLASS_VAESKEYGENASSIST:
  case XED_ICLASS_VALIGND:
  case XED_ICLASS_VALIGNQ:
  case XED_ICLASS_VANDNPD:
  case XED_ICLASS_VANDNPS:
  case XED_ICLASS_VANDPD:
  case XED_ICLASS_VANDPS:
  case XED_ICLASS_VBLENDMPD:
  case XED_ICLASS_VBLENDMPS:
  case XED_ICLASS_VBLENDPD:
  case XED_ICLASS_VBLENDPS:
  case XED_ICLASS_VBLENDVPD:
  case XED_ICLASS_VBLENDVPS:
  case XED_ICLASS_VBROADCASTF128:
  case XED_ICLASS_VBROADCASTF32X2:
  case XED_ICLASS_VBROADCASTF32X4:
  case XED_ICLASS_VBROADCASTF32X8:
  case XED_ICLASS_VBROADCASTF64X2:
  case XED_ICLASS_VBROADCASTF64X4:
  case XED_ICLASS_VBROADCASTI128:
  case XED_ICLASS_VBROADCASTI32X2:
  case XED_ICLASS_VBROADCASTI32X4:
  case XED_ICLASS_VBROADCASTI32X8:
  case XED_ICLASS_VBROADCASTI64X2:
  case XED_ICLASS_VBROADCASTI64X4:
  case XED_ICLASS_VBROADCASTSD:
    xed_assert(0);
    break;

  case XED_ICLASS_VBROADCASTSS:
    //    translateVBROADCASTSS(&xedd);
    break;

  case XED_ICLASS_VCMPPD:
  case XED_ICLASS_VCMPPS:
  case XED_ICLASS_VCMPSD:
  case XED_ICLASS_VCMPSS:
  case XED_ICLASS_VCOMISD:
  case XED_ICLASS_VCOMISS:
  case XED_ICLASS_VCOMPRESSPD:
  case XED_ICLASS_VCOMPRESSPS:
  case XED_ICLASS_VCVTDQ2PD:
  case XED_ICLASS_VCVTDQ2PS:
  case XED_ICLASS_VCVTNE2PS2BF16:
  case XED_ICLASS_VCVTNEPS2BF16:
  case XED_ICLASS_VCVTPD2DQ:
  case XED_ICLASS_VCVTPD2PS:
  case XED_ICLASS_VCVTPD2QQ:
  case XED_ICLASS_VCVTPD2UDQ:
  case XED_ICLASS_VCVTPD2UQQ:
  case XED_ICLASS_VCVTPH2PS:
  case XED_ICLASS_VCVTPS2DQ:
  case XED_ICLASS_VCVTPS2PD:
  case XED_ICLASS_VCVTPS2PH:
  case XED_ICLASS_VCVTPS2QQ:
  case XED_ICLASS_VCVTPS2UDQ:
  case XED_ICLASS_VCVTPS2UQQ:
  case XED_ICLASS_VCVTQQ2PD:
  case XED_ICLASS_VCVTQQ2PS:
  case XED_ICLASS_VCVTSD2SI:
  case XED_ICLASS_VCVTSD2SS:
  case XED_ICLASS_VCVTSD2USI:
  case XED_ICLASS_VCVTSI2SD:
  case XED_ICLASS_VCVTSI2SS:
  case XED_ICLASS_VCVTSS2SD:
  case XED_ICLASS_VCVTSS2SI:
  case XED_ICLASS_VCVTSS2USI:
  case XED_ICLASS_VCVTTPD2DQ:
  case XED_ICLASS_VCVTTPD2QQ:
  case XED_ICLASS_VCVTTPD2UDQ:
  case XED_ICLASS_VCVTTPD2UQQ:
  case XED_ICLASS_VCVTTPS2DQ:
  case XED_ICLASS_VCVTTPS2QQ:
  case XED_ICLASS_VCVTTPS2UDQ:
  case XED_ICLASS_VCVTTPS2UQQ:
  case XED_ICLASS_VCVTTSD2SI:
  case XED_ICLASS_VCVTTSD2USI:
  case XED_ICLASS_VCVTTSS2SI:
  case XED_ICLASS_VCVTTSS2USI:
  case XED_ICLASS_VCVTUDQ2PD:
  case XED_ICLASS_VCVTUDQ2PS:
  case XED_ICLASS_VCVTUQQ2PD:
  case XED_ICLASS_VCVTUQQ2PS:
  case XED_ICLASS_VCVTUSI2SD:
  case XED_ICLASS_VCVTUSI2SS:
  case XED_ICLASS_VDBPSADBW:
  case XED_ICLASS_VDIVPD:
  case XED_ICLASS_VDIVPS:
  case XED_ICLASS_VDIVSD:
  case XED_ICLASS_VDIVSS:
  case XED_ICLASS_VDPBF16PS:
  case XED_ICLASS_VDPPD:
  case XED_ICLASS_VDPPS:
  case XED_ICLASS_VERR:
  case XED_ICLASS_VERW:
  case XED_ICLASS_VEXP2PD:
  case XED_ICLASS_VEXP2PS:
  case XED_ICLASS_VEXPANDPD:
  case XED_ICLASS_VEXPANDPS:
  case XED_ICLASS_VEXTRACTF128:
  case XED_ICLASS_VEXTRACTF32X4:
  case XED_ICLASS_VEXTRACTF32X8:
  case XED_ICLASS_VEXTRACTF64X2:
  case XED_ICLASS_VEXTRACTF64X4:
  case XED_ICLASS_VEXTRACTI128:
  case XED_ICLASS_VEXTRACTI32X4:
  case XED_ICLASS_VEXTRACTI32X8:
  case XED_ICLASS_VEXTRACTI64X2:
  case XED_ICLASS_VEXTRACTI64X4:
  case XED_ICLASS_VEXTRACTPS:
  case XED_ICLASS_VFIXUPIMMPD:
  case XED_ICLASS_VFIXUPIMMPS:
  case XED_ICLASS_VFIXUPIMMSD:
  case XED_ICLASS_VFIXUPIMMSS:
  case XED_ICLASS_VFMADD132PD:
  case XED_ICLASS_VFMADD132PS:
  case XED_ICLASS_VFMADD132SD:
  case XED_ICLASS_VFMADD132SS:
  case XED_ICLASS_VFMADD213PD:
  case XED_ICLASS_VFMADD213PS:
  case XED_ICLASS_VFMADD213SD:
  case XED_ICLASS_VFMADD213SS:
  case XED_ICLASS_VFMADD231PD:
  case XED_ICLASS_VFMADD231PS:
  case XED_ICLASS_VFMADD231SD:
  case XED_ICLASS_VFMADD231SS:



  case XED_ICLASS_VFMADDPD:
  case XED_ICLASS_VFMADDPS:
  case XED_ICLASS_VFMADDSD:
  case XED_ICLASS_VFMADDSS:
  case XED_ICLASS_VFMADDSUB132PD:
  case XED_ICLASS_VFMADDSUB132PS:
  case XED_ICLASS_VFMADDSUB213PD:
  case XED_ICLASS_VFMADDSUB213PS:
  case XED_ICLASS_VFMADDSUB231PD:
  case XED_ICLASS_VFMADDSUB231PS:
  case XED_ICLASS_VFMADDSUBPD:
  case XED_ICLASS_VFMADDSUBPS:
  case XED_ICLASS_VFMSUB132PD:
  case XED_ICLASS_VFMSUB132PS:
  case XED_ICLASS_VFMSUB132SD:
  case XED_ICLASS_VFMSUB132SS:
  case XED_ICLASS_VFMSUB213PD:
  case XED_ICLASS_VFMSUB213PS:
  case XED_ICLASS_VFMSUB213SD:
  case XED_ICLASS_VFMSUB213SS:
  case XED_ICLASS_VFMSUB231PD:
  case XED_ICLASS_VFMSUB231PS:
  case XED_ICLASS_VFMSUB231SD:
  case XED_ICLASS_VFMSUB231SS:
  case XED_ICLASS_VFMSUBADD132PD:
  case XED_ICLASS_VFMSUBADD132PS:
  case XED_ICLASS_VFMSUBADD213PD:
  case XED_ICLASS_VFMSUBADD213PS:
  case XED_ICLASS_VFMSUBADD231PD:
  case XED_ICLASS_VFMSUBADD231PS:
  case XED_ICLASS_VFMSUBADDPD:
  case XED_ICLASS_VFMSUBADDPS:
  case XED_ICLASS_VFMSUBPD:
  case XED_ICLASS_VFMSUBPS:
  case XED_ICLASS_VFMSUBSD:
  case XED_ICLASS_VFMSUBSS:
  case XED_ICLASS_VFNMADD132PD:
  case XED_ICLASS_VFNMADD132PS:
  case XED_ICLASS_VFNMADD132SD:
  case XED_ICLASS_VFNMADD132SS:
  case XED_ICLASS_VFNMADD213PD:
  case XED_ICLASS_VFNMADD213PS:
  case XED_ICLASS_VFNMADD213SD:
  case XED_ICLASS_VFNMADD213SS:
  case XED_ICLASS_VFNMADD231PD:
  case XED_ICLASS_VFNMADD231PS:
  case XED_ICLASS_VFNMADD231SD:
  case XED_ICLASS_VFNMADD231SS:
  case XED_ICLASS_VFNMADDPD:
  case XED_ICLASS_VFNMADDPS:
  case XED_ICLASS_VFNMADDSD:
  case XED_ICLASS_VFNMADDSS:
  case XED_ICLASS_VFNMSUB132PD:
  case XED_ICLASS_VFNMSUB132PS:
  case XED_ICLASS_VFNMSUB132SD:
  case XED_ICLASS_VFNMSUB132SS:
  case XED_ICLASS_VFNMSUB213PD:
  case XED_ICLASS_VFNMSUB213PS:
  case XED_ICLASS_VFNMSUB213SD:
  case XED_ICLASS_VFNMSUB213SS:
  case XED_ICLASS_VFNMSUB231PD:
  case XED_ICLASS_VFNMSUB231PS:
  case XED_ICLASS_VFNMSUB231SD:
  case XED_ICLASS_VFNMSUB231SS:
  case XED_ICLASS_VFNMSUBPD:
  case XED_ICLASS_VFNMSUBPS:
  case XED_ICLASS_VFNMSUBSD:
  case XED_ICLASS_VFNMSUBSS:
  case XED_ICLASS_VFPCLASSPD:
  case XED_ICLASS_VFPCLASSPS:
  case XED_ICLASS_VFPCLASSSD:
  case XED_ICLASS_VFPCLASSSS:
  case XED_ICLASS_VFRCZPD:
  case XED_ICLASS_VFRCZPS:
  case XED_ICLASS_VFRCZSD:
  case XED_ICLASS_VFRCZSS:
  case XED_ICLASS_VGATHERDPD:
  case XED_ICLASS_VGATHERDPS:
  case XED_ICLASS_VGATHERPF0DPD:
  case XED_ICLASS_VGATHERPF0DPS:
  case XED_ICLASS_VGATHERPF0QPD:
  case XED_ICLASS_VGATHERPF0QPS:
  case XED_ICLASS_VGATHERPF1DPD:
  case XED_ICLASS_VGATHERPF1DPS:
  case XED_ICLASS_VGATHERPF1QPD:
  case XED_ICLASS_VGATHERPF1QPS:
  case XED_ICLASS_VGATHERQPD:
  case XED_ICLASS_VGATHERQPS:
  case XED_ICLASS_VGETEXPPD:
  case XED_ICLASS_VGETEXPPS:
  case XED_ICLASS_VGETEXPSD:
  case XED_ICLASS_VGETEXPSS:
  case XED_ICLASS_VGETMANTPD:
  case XED_ICLASS_VGETMANTPS:
  case XED_ICLASS_VGETMANTSD:
  case XED_ICLASS_VGETMANTSS:
  case XED_ICLASS_VGF2P8AFFINEINVQB:
  case XED_ICLASS_VGF2P8AFFINEQB:
  case XED_ICLASS_VGF2P8MULB:
  case XED_ICLASS_VHADDPD:
  case XED_ICLASS_VHADDPS:
  case XED_ICLASS_VHSUBPD:
  case XED_ICLASS_VHSUBPS:
  case XED_ICLASS_VINSERTF128:
  case XED_ICLASS_VINSERTF32X4:
  case XED_ICLASS_VINSERTF32X8:
  case XED_ICLASS_VINSERTF64X2:
  case XED_ICLASS_VINSERTF64X4:
  case XED_ICLASS_VINSERTI128:
  case XED_ICLASS_VINSERTI32X4:
  case XED_ICLASS_VINSERTI32X8:
  case XED_ICLASS_VINSERTI64X2:
  case XED_ICLASS_VINSERTI64X4:
  case XED_ICLASS_VINSERTPS:
  case XED_ICLASS_VLDDQU:
  case XED_ICLASS_VLDMXCSR:
  case XED_ICLASS_VMASKMOVDQU:
  case XED_ICLASS_VMASKMOVPD:
  case XED_ICLASS_VMASKMOVPS:
  case XED_ICLASS_VMAXPD:
  case XED_ICLASS_VMAXPS:
  case XED_ICLASS_VMAXSD:
  case XED_ICLASS_VMAXSS:
  case XED_ICLASS_VMCALL:
  case XED_ICLASS_VMCLEAR:
  case XED_ICLASS_VMFUNC:
  case XED_ICLASS_VMINPD:
  case XED_ICLASS_VMINPS:
  case XED_ICLASS_VMINSD:
  case XED_ICLASS_VMINSS:
  case XED_ICLASS_VMLAUNCH:
  case XED_ICLASS_VMLOAD:
  case XED_ICLASS_VMMCALL:
  case XED_ICLASS_VMOVAPD:
  case XED_ICLASS_VMOVAPS:
  case XED_ICLASS_VMOVD:
  case XED_ICLASS_VMOVDDUP:
  case XED_ICLASS_VMOVDQA:
  case XED_ICLASS_VMOVDQA32:
  case XED_ICLASS_VMOVDQA64:
  case XED_ICLASS_VMOVDQU:
  case XED_ICLASS_VMOVDQU16:
  case XED_ICLASS_VMOVDQU32:
  case XED_ICLASS_VMOVDQU64:
  case XED_ICLASS_VMOVDQU8:
  case XED_ICLASS_VMOVHLPS:
  case XED_ICLASS_VMOVHPD:
  case XED_ICLASS_VMOVHPS:
  case XED_ICLASS_VMOVLHPS:
  case XED_ICLASS_VMOVLPD:
  case XED_ICLASS_VMOVLPS:
  case XED_ICLASS_VMOVMSKPD:
  case XED_ICLASS_VMOVMSKPS:
  case XED_ICLASS_VMOVNTDQ:
  case XED_ICLASS_VMOVNTDQA:
  case XED_ICLASS_VMOVNTPD:
  case XED_ICLASS_VMOVNTPS:
  case XED_ICLASS_VMOVQ:
  case XED_ICLASS_VMOVSD:
  case XED_ICLASS_VMOVSHDUP:
  case XED_ICLASS_VMOVSLDUP:
  case XED_ICLASS_VMOVSS:
  case XED_ICLASS_VMOVUPD:
  case XED_ICLASS_VMOVUPS:
    //    translateVMOVUPS(&xedd);
    break;

  case XED_ICLASS_VMPSADBW:
  case XED_ICLASS_VMPTRLD:
  case XED_ICLASS_VMPTRST:
  case XED_ICLASS_VMREAD:
  case XED_ICLASS_VMRESUME:
  case XED_ICLASS_VMRUN:
  case XED_ICLASS_VMSAVE:
  case XED_ICLASS_VMULPD:
  case XED_ICLASS_VMULPS:
  case XED_ICLASS_VMULSD:
  case XED_ICLASS_VMULSS:
  case XED_ICLASS_VMWRITE:
  case XED_ICLASS_VMXOFF:
  case XED_ICLASS_VMXON:
  case XED_ICLASS_VORPD:
  case XED_ICLASS_VORPS:
  case XED_ICLASS_VP2INTERSECTD:
  case XED_ICLASS_VP2INTERSECTQ:
  case XED_ICLASS_VP4DPWSSD:
  case XED_ICLASS_VP4DPWSSDS:
  case XED_ICLASS_VPABSB:
  case XED_ICLASS_VPABSD:
  case XED_ICLASS_VPABSQ:
  case XED_ICLASS_VPABSW:
  case XED_ICLASS_VPACKSSDW:
  case XED_ICLASS_VPACKSSWB:
  case XED_ICLASS_VPACKUSDW:
  case XED_ICLASS_VPACKUSWB:
  case XED_ICLASS_VPADDB:
  case XED_ICLASS_VPADDD:
  case XED_ICLASS_VPADDQ:
  case XED_ICLASS_VPADDSB:
  case XED_ICLASS_VPADDSW:
  case XED_ICLASS_VPADDUSB:
  case XED_ICLASS_VPADDUSW:
  case XED_ICLASS_VPADDW:
  case XED_ICLASS_VPALIGNR:
  case XED_ICLASS_VPAND:
  case XED_ICLASS_VPANDD:
  case XED_ICLASS_VPANDN:
  case XED_ICLASS_VPANDND:
  case XED_ICLASS_VPANDNQ:
  case XED_ICLASS_VPANDQ:
  case XED_ICLASS_VPAVGB:
  case XED_ICLASS_VPAVGW:
  case XED_ICLASS_VPBLENDD:
  case XED_ICLASS_VPBLENDMB:
  case XED_ICLASS_VPBLENDMD:
  case XED_ICLASS_VPBLENDMQ:
  case XED_ICLASS_VPBLENDMW:
  case XED_ICLASS_VPBLENDVB:
  case XED_ICLASS_VPBLENDW:
  case XED_ICLASS_VPBROADCASTB:
  case XED_ICLASS_VPBROADCASTD:
  case XED_ICLASS_VPBROADCASTMB2Q:
  case XED_ICLASS_VPBROADCASTMW2D:
  case XED_ICLASS_VPBROADCASTQ:
  case XED_ICLASS_VPBROADCASTW:
  case XED_ICLASS_VPCLMULQDQ:
  case XED_ICLASS_VPCMOV:
  case XED_ICLASS_VPCMPB:
  case XED_ICLASS_VPCMPD:
  case XED_ICLASS_VPCMPEQB:
  case XED_ICLASS_VPCMPEQD:
  case XED_ICLASS_VPCMPEQQ:
  case XED_ICLASS_VPCMPEQW:
  case XED_ICLASS_VPCMPESTRI:
  case XED_ICLASS_VPCMPESTRI64:
  case XED_ICLASS_VPCMPESTRM:
  case XED_ICLASS_VPCMPESTRM64:
  case XED_ICLASS_VPCMPGTB:
  case XED_ICLASS_VPCMPGTD:
  case XED_ICLASS_VPCMPGTQ:
  case XED_ICLASS_VPCMPGTW:
  case XED_ICLASS_VPCMPISTRI:
  case XED_ICLASS_VPCMPISTRI64:
  case XED_ICLASS_VPCMPISTRM:
  case XED_ICLASS_VPCMPQ:
  case XED_ICLASS_VPCMPUB:
  case XED_ICLASS_VPCMPUD:
  case XED_ICLASS_VPCMPUQ:
  case XED_ICLASS_VPCMPUW:
  case XED_ICLASS_VPCMPW:
  case XED_ICLASS_VPCOMB:
  case XED_ICLASS_VPCOMD:
  case XED_ICLASS_VPCOMPRESSB:
  case XED_ICLASS_VPCOMPRESSD:
  case XED_ICLASS_VPCOMPRESSQ:
  case XED_ICLASS_VPCOMPRESSW:
  case XED_ICLASS_VPCOMQ:
  case XED_ICLASS_VPCOMUB:
  case XED_ICLASS_VPCOMUD:
  case XED_ICLASS_VPCOMUQ:
  case XED_ICLASS_VPCOMUW:
  case XED_ICLASS_VPCOMW:
  case XED_ICLASS_VPCONFLICTD:
  case XED_ICLASS_VPCONFLICTQ:
  case XED_ICLASS_VPDPBUSD:
  case XED_ICLASS_VPDPBUSDS:
  case XED_ICLASS_VPDPWSSD:
  case XED_ICLASS_VPDPWSSDS:
  case XED_ICLASS_VPERM2F128:
  case XED_ICLASS_VPERM2I128:
  case XED_ICLASS_VPERMB:
  case XED_ICLASS_VPERMD:
  case XED_ICLASS_VPERMI2B:
  case XED_ICLASS_VPERMI2D:
  case XED_ICLASS_VPERMI2PD:
  case XED_ICLASS_VPERMI2PS:
  case XED_ICLASS_VPERMI2Q:
  case XED_ICLASS_VPERMI2W:
  case XED_ICLASS_VPERMIL2PD:
  case XED_ICLASS_VPERMIL2PS:
  case XED_ICLASS_VPERMILPD:
  case XED_ICLASS_VPERMILPS:
  case XED_ICLASS_VPERMPD:
  case XED_ICLASS_VPERMPS:
  case XED_ICLASS_VPERMQ:
  case XED_ICLASS_VPERMT2B:
  case XED_ICLASS_VPERMT2D:
  case XED_ICLASS_VPERMT2PD:
  case XED_ICLASS_VPERMT2PS:
  case XED_ICLASS_VPERMT2Q:
  case XED_ICLASS_VPERMT2W:
  case XED_ICLASS_VPERMW:
  case XED_ICLASS_VPEXPANDB:
  case XED_ICLASS_VPEXPANDD:
  case XED_ICLASS_VPEXPANDQ:
  case XED_ICLASS_VPEXPANDW:
  case XED_ICLASS_VPEXTRB:
  case XED_ICLASS_VPEXTRD:
  case XED_ICLASS_VPEXTRQ:
  case XED_ICLASS_VPEXTRW:
  case XED_ICLASS_VPEXTRW_C5:
  case XED_ICLASS_VPGATHERDD:
  case XED_ICLASS_VPGATHERDQ:
  case XED_ICLASS_VPGATHERQD:
  case XED_ICLASS_VPGATHERQQ:
  case XED_ICLASS_VPHADDBD:
  case XED_ICLASS_VPHADDBQ:
  case XED_ICLASS_VPHADDBW:
  case XED_ICLASS_VPHADDD:
  case XED_ICLASS_VPHADDDQ:
  case XED_ICLASS_VPHADDSW:
  case XED_ICLASS_VPHADDUBD:
  case XED_ICLASS_VPHADDUBQ:
  case XED_ICLASS_VPHADDUBW:
  case XED_ICLASS_VPHADDUDQ:
  case XED_ICLASS_VPHADDUWD:
  case XED_ICLASS_VPHADDUWQ:
  case XED_ICLASS_VPHADDW:
  case XED_ICLASS_VPHADDWD:
  case XED_ICLASS_VPHADDWQ:
  case XED_ICLASS_VPHMINPOSUW:
  case XED_ICLASS_VPHSUBBW:
  case XED_ICLASS_VPHSUBD:
  case XED_ICLASS_VPHSUBDQ:
  case XED_ICLASS_VPHSUBSW:
  case XED_ICLASS_VPHSUBW:
  case XED_ICLASS_VPHSUBWD:
  case XED_ICLASS_VPINSRB:
  case XED_ICLASS_VPINSRD:
  case XED_ICLASS_VPINSRQ:
  case XED_ICLASS_VPINSRW:
  case XED_ICLASS_VPLZCNTD:
  case XED_ICLASS_VPLZCNTQ:
  case XED_ICLASS_VPMACSDD:
  case XED_ICLASS_VPMACSDQH:
  case XED_ICLASS_VPMACSDQL:
  case XED_ICLASS_VPMACSSDD:
  case XED_ICLASS_VPMACSSDQH:
  case XED_ICLASS_VPMACSSDQL:
  case XED_ICLASS_VPMACSSWD:
  case XED_ICLASS_VPMACSSWW:
  case XED_ICLASS_VPMACSWD:
  case XED_ICLASS_VPMACSWW:
  case XED_ICLASS_VPMADCSSWD:
  case XED_ICLASS_VPMADCSWD:
  case XED_ICLASS_VPMADD52HUQ:
  case XED_ICLASS_VPMADD52LUQ:
  case XED_ICLASS_VPMADDUBSW:
  case XED_ICLASS_VPMADDWD:
  case XED_ICLASS_VPMASKMOVD:
  case XED_ICLASS_VPMASKMOVQ:
  case XED_ICLASS_VPMAXSB:
  case XED_ICLASS_VPMAXSD:
  case XED_ICLASS_VPMAXSQ:
  case XED_ICLASS_VPMAXSW:
  case XED_ICLASS_VPMAXUB:
  case XED_ICLASS_VPMAXUD:
  case XED_ICLASS_VPMAXUQ:
  case XED_ICLASS_VPMAXUW:
  case XED_ICLASS_VPMINSB:
  case XED_ICLASS_VPMINSD:
  case XED_ICLASS_VPMINSQ:
  case XED_ICLASS_VPMINSW:
  case XED_ICLASS_VPMINUB:
  case XED_ICLASS_VPMINUD:
  case XED_ICLASS_VPMINUQ:
  case XED_ICLASS_VPMINUW:
  case XED_ICLASS_VPMOVB2M:
  case XED_ICLASS_VPMOVD2M:
  case XED_ICLASS_VPMOVDB:
  case XED_ICLASS_VPMOVDW:
  case XED_ICLASS_VPMOVM2B:
  case XED_ICLASS_VPMOVM2D:
  case XED_ICLASS_VPMOVM2Q:
  case XED_ICLASS_VPMOVM2W:
  case XED_ICLASS_VPMOVMSKB:
  case XED_ICLASS_VPMOVQ2M:
  case XED_ICLASS_VPMOVQB:
  case XED_ICLASS_VPMOVQD:
  case XED_ICLASS_VPMOVQW:
  case XED_ICLASS_VPMOVSDB:
  case XED_ICLASS_VPMOVSDW:
  case XED_ICLASS_VPMOVSQB:
  case XED_ICLASS_VPMOVSQD:
  case XED_ICLASS_VPMOVSQW:
  case XED_ICLASS_VPMOVSWB:
  case XED_ICLASS_VPMOVSXBD:
  case XED_ICLASS_VPMOVSXBQ:
  case XED_ICLASS_VPMOVSXBW:
  case XED_ICLASS_VPMOVSXDQ:
  case XED_ICLASS_VPMOVSXWD:
  case XED_ICLASS_VPMOVSXWQ:
  case XED_ICLASS_VPMOVUSDB:
  case XED_ICLASS_VPMOVUSDW:
  case XED_ICLASS_VPMOVUSQB:
  case XED_ICLASS_VPMOVUSQD:
  case XED_ICLASS_VPMOVUSQW:
  case XED_ICLASS_VPMOVUSWB:
  case XED_ICLASS_VPMOVW2M:
  case XED_ICLASS_VPMOVWB:
  case XED_ICLASS_VPMOVZXBD:
  case XED_ICLASS_VPMOVZXBQ:
  case XED_ICLASS_VPMOVZXBW:
  case XED_ICLASS_VPMOVZXDQ:
  case XED_ICLASS_VPMOVZXWD:
  case XED_ICLASS_VPMOVZXWQ:
  case XED_ICLASS_VPMULDQ:
  case XED_ICLASS_VPMULHRSW:
  case XED_ICLASS_VPMULHUW:
  case XED_ICLASS_VPMULHW:
  case XED_ICLASS_VPMULLD:
  case XED_ICLASS_VPMULLQ:
  case XED_ICLASS_VPMULLW:
  case XED_ICLASS_VPMULTISHIFTQB:
  case XED_ICLASS_VPMULUDQ:
  case XED_ICLASS_VPOPCNTB:
  case XED_ICLASS_VPOPCNTD:
  case XED_ICLASS_VPOPCNTQ:
  case XED_ICLASS_VPOPCNTW:
  case XED_ICLASS_VPOR:
  case XED_ICLASS_VPORD:
  case XED_ICLASS_VPORQ:
  case XED_ICLASS_VPPERM:
  case XED_ICLASS_VPROLD:
  case XED_ICLASS_VPROLQ:
  case XED_ICLASS_VPROLVD:
  case XED_ICLASS_VPROLVQ:
  case XED_ICLASS_VPRORD:
  case XED_ICLASS_VPRORQ:
  case XED_ICLASS_VPRORVD:
  case XED_ICLASS_VPRORVQ:
  case XED_ICLASS_VPROTB:
  case XED_ICLASS_VPROTD:
  case XED_ICLASS_VPROTQ:
  case XED_ICLASS_VPROTW:
  case XED_ICLASS_VPSADBW:
  case XED_ICLASS_VPSCATTERDD:
  case XED_ICLASS_VPSCATTERDQ:
  case XED_ICLASS_VPSCATTERQD:
  case XED_ICLASS_VPSCATTERQQ:
  case XED_ICLASS_VPSHAB:
  case XED_ICLASS_VPSHAD:
  case XED_ICLASS_VPSHAQ:
  case XED_ICLASS_VPSHAW:
  case XED_ICLASS_VPSHLB:
  case XED_ICLASS_VPSHLD:
  case XED_ICLASS_VPSHLDD:
  case XED_ICLASS_VPSHLDQ:
  case XED_ICLASS_VPSHLDVD:
  case XED_ICLASS_VPSHLDVQ:
  case XED_ICLASS_VPSHLDVW:
  case XED_ICLASS_VPSHLDW:
  case XED_ICLASS_VPSHLQ:
  case XED_ICLASS_VPSHLW:
  case XED_ICLASS_VPSHRDD:
  case XED_ICLASS_VPSHRDQ:
  case XED_ICLASS_VPSHRDVD:
  case XED_ICLASS_VPSHRDVQ:
  case XED_ICLASS_VPSHRDVW:
  case XED_ICLASS_VPSHRDW:
  case XED_ICLASS_VPSHUFB:
  case XED_ICLASS_VPSHUFBITQMB:
  case XED_ICLASS_VPSHUFD:
  case XED_ICLASS_VPSHUFHW:
  case XED_ICLASS_VPSHUFLW:
  case XED_ICLASS_VPSIGNB:
  case XED_ICLASS_VPSIGND:
  case XED_ICLASS_VPSIGNW:
  case XED_ICLASS_VPSLLD:
  case XED_ICLASS_VPSLLDQ:
  case XED_ICLASS_VPSLLQ:
  case XED_ICLASS_VPSLLVD:
  case XED_ICLASS_VPSLLVQ:
  case XED_ICLASS_VPSLLVW:
  case XED_ICLASS_VPSLLW:
  case XED_ICLASS_VPSRAD:
  case XED_ICLASS_VPSRAQ:
  case XED_ICLASS_VPSRAVD:
  case XED_ICLASS_VPSRAVQ:
  case XED_ICLASS_VPSRAVW:
  case XED_ICLASS_VPSRAW:
  case XED_ICLASS_VPSRLD:
  case XED_ICLASS_VPSRLDQ:
  case XED_ICLASS_VPSRLQ:
  case XED_ICLASS_VPSRLVD:
  case XED_ICLASS_VPSRLVQ:
  case XED_ICLASS_VPSRLVW:
  case XED_ICLASS_VPSRLW:
  case XED_ICLASS_VPSUBB:
  case XED_ICLASS_VPSUBD:
  case XED_ICLASS_VPSUBQ:
  case XED_ICLASS_VPSUBSB:
  case XED_ICLASS_VPSUBSW:
  case XED_ICLASS_VPSUBUSB:
  case XED_ICLASS_VPSUBUSW:
  case XED_ICLASS_VPSUBW:
  case XED_ICLASS_VPTERNLOGD:
  case XED_ICLASS_VPTERNLOGQ:
  case XED_ICLASS_VPTEST:
  case XED_ICLASS_VPTESTMB:
  case XED_ICLASS_VPTESTMD:
  case XED_ICLASS_VPTESTMQ:
  case XED_ICLASS_VPTESTMW:
  case XED_ICLASS_VPTESTNMB:
  case XED_ICLASS_VPTESTNMD:
  case XED_ICLASS_VPTESTNMQ:
  case XED_ICLASS_VPTESTNMW:
  case XED_ICLASS_VPUNPCKHBW:
  case XED_ICLASS_VPUNPCKHDQ:
  case XED_ICLASS_VPUNPCKHQDQ:
  case XED_ICLASS_VPUNPCKHWD:
  case XED_ICLASS_VPUNPCKLBW:
  case XED_ICLASS_VPUNPCKLDQ:
  case XED_ICLASS_VPUNPCKLQDQ:
  case XED_ICLASS_VPUNPCKLWD:
  case XED_ICLASS_VPXOR:
  case XED_ICLASS_VPXORD:
  case XED_ICLASS_VPXORQ:
  case XED_ICLASS_VRANGEPD:
  case XED_ICLASS_VRANGEPS:
  case XED_ICLASS_VRANGESD:
  case XED_ICLASS_VRANGESS:
  case XED_ICLASS_VRCP14PD:
  case XED_ICLASS_VRCP14PS:
  case XED_ICLASS_VRCP14SD:
  case XED_ICLASS_VRCP14SS:
  case XED_ICLASS_VRCP28PD:
  case XED_ICLASS_VRCP28PS:
  case XED_ICLASS_VRCP28SD:
  case XED_ICLASS_VRCP28SS:
  case XED_ICLASS_VRCPPS:
  case XED_ICLASS_VRCPSS:
  case XED_ICLASS_VREDUCEPD:
  case XED_ICLASS_VREDUCEPS:
  case XED_ICLASS_VREDUCESD:
  case XED_ICLASS_VREDUCESS:
  case XED_ICLASS_VRNDSCALEPD:
  case XED_ICLASS_VRNDSCALEPS:
  case XED_ICLASS_VRNDSCALESD:
  case XED_ICLASS_VRNDSCALESS:
  case XED_ICLASS_VROUNDPD:
  case XED_ICLASS_VROUNDPS:
  case XED_ICLASS_VROUNDSD:
  case XED_ICLASS_VROUNDSS:
  case XED_ICLASS_VRSQRT14PD:
  case XED_ICLASS_VRSQRT14PS:
  case XED_ICLASS_VRSQRT14SD:
  case XED_ICLASS_VRSQRT14SS:
  case XED_ICLASS_VRSQRT28PD:
  case XED_ICLASS_VRSQRT28PS:
  case XED_ICLASS_VRSQRT28SD:
  case XED_ICLASS_VRSQRT28SS:
  case XED_ICLASS_VRSQRTPS:
  case XED_ICLASS_VRSQRTSS:
  case XED_ICLASS_VSCALEFPD:
  case XED_ICLASS_VSCALEFPS:
  case XED_ICLASS_VSCALEFSD:
  case XED_ICLASS_VSCALEFSS:
  case XED_ICLASS_VSCATTERDPD:
  case XED_ICLASS_VSCATTERDPS:
  case XED_ICLASS_VSCATTERPF0DPD:
  case XED_ICLASS_VSCATTERPF0DPS:
  case XED_ICLASS_VSCATTERPF0QPD:
  case XED_ICLASS_VSCATTERPF0QPS:
  case XED_ICLASS_VSCATTERPF1DPD:
  case XED_ICLASS_VSCATTERPF1DPS:
  case XED_ICLASS_VSCATTERPF1QPD:
  case XED_ICLASS_VSCATTERPF1QPS:
  case XED_ICLASS_VSCATTERQPD:
  case XED_ICLASS_VSCATTERQPS:
  case XED_ICLASS_VSHUFF32X4:
  case XED_ICLASS_VSHUFF64X2:
  case XED_ICLASS_VSHUFI32X4:
  case XED_ICLASS_VSHUFI64X2:
  case XED_ICLASS_VSHUFPD:
  case XED_ICLASS_VSHUFPS:
  case XED_ICLASS_VSQRTPD:
  case XED_ICLASS_VSQRTPS:
  case XED_ICLASS_VSQRTSD:
  case XED_ICLASS_VSQRTSS:
  case XED_ICLASS_VSTMXCSR:
  case XED_ICLASS_VSUBPD:
  case XED_ICLASS_VSUBPS:
  case XED_ICLASS_VSUBSD:
  case XED_ICLASS_VSUBSS:
  case XED_ICLASS_VTESTPD:
  case XED_ICLASS_VTESTPS:
  case XED_ICLASS_VUCOMISD:
  case XED_ICLASS_VUCOMISS:
  case XED_ICLASS_VUNPCKHPD:
  case XED_ICLASS_VUNPCKHPS:
  case XED_ICLASS_VUNPCKLPD:
  case XED_ICLASS_VUNPCKLPS:
  case XED_ICLASS_VXORPD:
  case XED_ICLASS_VXORPS:
  case XED_ICLASS_VZEROALL:
  case XED_ICLASS_VZEROUPPER:
  case XED_ICLASS_WBINVD:
  case XED_ICLASS_WBNOINVD:
  case XED_ICLASS_WRFSBASE:
  case XED_ICLASS_WRGSBASE:
  case XED_ICLASS_WRMSR:
  case XED_ICLASS_WRPKRU:
  case XED_ICLASS_WRSSD:
  case XED_ICLASS_WRSSQ:
  case XED_ICLASS_WRUSSD:
  case XED_ICLASS_WRUSSQ:
  case XED_ICLASS_XABORT:
  case XED_ICLASS_XADD:
  case XED_ICLASS_XADD_LOCK:
  case XED_ICLASS_XBEGIN:
  case XED_ICLASS_XCHG:
  case XED_ICLASS_XEND:
  case XED_ICLASS_XGETBV:
  case XED_ICLASS_XLAT:
  case XED_ICLASS_XOR:
  case XED_ICLASS_XORPD:
  case XED_ICLASS_XORPS:
  case XED_ICLASS_XOR_LOCK:
  case XED_ICLASS_XRSTOR:
  case XED_ICLASS_XRSTOR64:
  case XED_ICLASS_XRSTORS:
  case XED_ICLASS_XRSTORS64:
  case XED_ICLASS_XSAVE:
  case XED_ICLASS_XSAVE64:
  case XED_ICLASS_XSAVEC:
  case XED_ICLASS_XSAVEC64:
  case XED_ICLASS_XSAVEOPT:
  case XED_ICLASS_XSAVEOPT64:
  case XED_ICLASS_XSAVES:
  case XED_ICLASS_XSAVES64:
  case XED_ICLASS_XSETBV:
  case XED_ICLASS_XSTORE:
  case XED_ICLASS_XTEST:
  case XED_ICLASS_LAST:
  default:
    xed_assert(0);
    break;
  }

  return true;
}

void print_memops(xed_decoded_inst_t *xedd) {
  unsigned int i, memops = xed_decoded_inst_number_of_memory_operands(xedd);

  printf("Memory Operands\n");

  for (i = 0; i < memops; i++) {
    xed_bool_t r_or_w = 0;
    xed_reg_enum_t seg;
    xed_reg_enum_t base;
    xed_reg_enum_t indx;
    printf("  %u ", i);
    if (xed_decoded_inst_mem_read(xedd, i)) {
      printf("   read ");
      r_or_w = 1;
    }
    if (xed_decoded_inst_mem_written(xedd, i)) {
      printf("written ");
      r_or_w = 1;
    }
    if (!r_or_w) {
      printf("   agen "); // LEA instructions
    }
    seg = xed_decoded_inst_get_seg_reg(xedd, i);
    if (seg != XED_REG_INVALID) {
      printf("SEG= %s ", xed_reg_enum_t2str(seg));
    }
    base = xed_decoded_inst_get_base_reg(xedd, i);
    if (base != XED_REG_INVALID) {
      printf("BASE= %3s/%3s ", xed_reg_enum_t2str(base),
             xed_reg_class_enum_t2str(xed_reg_class(base)));
    }
    indx = xed_decoded_inst_get_index_reg(xedd, i);
    if (i == 0 && indx != XED_REG_INVALID) {
      printf("INDEX= %3s/%3s ", xed_reg_enum_t2str(indx),
             xed_reg_class_enum_t2str(xed_reg_class(indx)));
      if (xed_decoded_inst_get_scale(xedd, i) != 0) {
        // only have a scale if the index exists.
        printf("SCALE= %u ", xed_decoded_inst_get_scale(xedd, i));
      }
    }

    if (xed_operand_values_has_memory_displacement(xedd)) {
      xed_uint_t disp_bits =
          xed_decoded_inst_get_memory_displacement_width(xedd, i);
      if (disp_bits) {
        xed_int64_t disp;
        printf("DISPLACEMENT_BYTES= %u ", disp_bits);
        disp = xed_decoded_inst_get_memory_displacement(xedd, i);
        printf("0x" XED_FMT_LX16 " base10=" XED_FMT_LD, disp, disp);
      }
    }
    printf(" ASZ%u=%u\n", i, xed_decoded_inst_get_memop_address_width(xedd, i));
  }
  printf("  MemopBytes = %u\n",
         xed_decoded_inst_get_memory_operand_length(xedd, 0));
}

void print_operands(xed_decoded_inst_t *xedd) {
  unsigned int i, noperands;
#define TBUFSZ 90
  char tbuf[TBUFSZ];
  const xed_inst_t *xi = xed_decoded_inst_inst(xedd);
  xed_operand_action_enum_t rw;
  xed_uint_t bits;

  printf("Operands\n");
  noperands = xed_inst_noperands(xi);
  printf("#   TYPE               DETAILS        VIS  RW       OC2 BITS BYTES "
         "NELEM ELEMSZ   ELEMTYPE   REGCLASS\n");
  printf("#   ====               =======        ===  ==       === ==== ===== "
         "===== ======   ========   ========\n");
  tbuf[0] = 0;
  for (i = 0; i < noperands; i++) {
    const xed_operand_t *op = xed_inst_operand(xi, i);
    xed_operand_enum_t op_name = xed_operand_name(op);
    printf("%u %6s ", i, xed_operand_enum_t2str(op_name));

    switch (op_name) {
    case XED_OPERAND_AGEN:
    case XED_OPERAND_MEM0:
    case XED_OPERAND_MEM1:
      // we print memops in a different function
      xed_strcpy(tbuf, "(see below)");
      break;
    case XED_OPERAND_PTR:     // pointer (always in conjunction with a IMM0)
    case XED_OPERAND_RELBR: { // branch displacements
      xed_uint_t disp_bits =
          xed_decoded_inst_get_branch_displacement_width(xedd);
      if (disp_bits) {
        xed_int32_t disp = xed_decoded_inst_get_branch_displacement(xedd);
#if defined(_WIN32) && !defined(PIN_CRT)
        _snprintf_s(tbuf, TBUFSZ, TBUFSZ, "BRANCH_DISPLACEMENT_BYTES= %d %08x",
                    disp_bits, disp);
#else
        snprintf(tbuf, TBUFSZ, "BRANCH_DISPLACEMENT_BYTES= %d %08x", disp_bits,
                 disp);
#endif
      }
    } break;

    case XED_OPERAND_IMM0: { // immediates
      char buf[64];
      const unsigned int no_leading_zeros = 0;
      xed_uint_t ibits;
      const xed_bool_t lowercase = 1;
      ibits = xed_decoded_inst_get_immediate_width_bits(xedd);
      if (xed_decoded_inst_get_immediate_is_signed(xedd)) {
        xed_uint_t rbits = ibits ? ibits : 8;
        xed_int32_t x = xed_decoded_inst_get_signed_immediate(xedd);
        xed_uint64_t y = XED_STATIC_CAST(
            xed_uint64_t,
            xed_sign_extend_arbitrary_to_64((xed_uint64_t)x, ibits));
        xed_itoa_hex_ul(buf, y, rbits, no_leading_zeros, 64, lowercase);
      } else {
        xed_uint64_t x = xed_decoded_inst_get_unsigned_immediate(xedd);
        xed_uint_t rbits = ibits ? ibits : 16;
        xed_itoa_hex_ul(buf, x, rbits, no_leading_zeros, 64, lowercase);
      }
#if defined(_WIN32) && !defined(PIN_CRT)
      _snprintf_s(tbuf, TBUFSZ, TBUFSZ, "0x%s(%db)", buf, ibits);
#else
      snprintf(tbuf, TBUFSZ, "0x%s(%db)", buf, ibits);
#endif
      break;
    }
    case XED_OPERAND_IMM1: { // 2nd immediate is always 1 byte.
      xed_uint8_t x = xed_decoded_inst_get_second_immediate(xedd);
#if defined(_WIN32) && !defined(PIN_CRT)
      _snprintf_s(tbuf, TBUFSZ, TBUFSZ, "0x%02x", (int)x);
#else
      snprintf(tbuf, TBUFSZ, "0x%02x", (int)x);
#endif

      break;
    }

    case XED_OPERAND_REG0:
    case XED_OPERAND_REG1:
    case XED_OPERAND_REG2:
    case XED_OPERAND_REG3:
    case XED_OPERAND_REG4:
    case XED_OPERAND_REG5:
    case XED_OPERAND_REG6:
    case XED_OPERAND_REG7:
    case XED_OPERAND_REG8:
    case XED_OPERAND_BASE0:
    case XED_OPERAND_BASE1: {

      xed_reg_enum_t r = xed_decoded_inst_get_reg(xedd, op_name);
#if defined(_WIN32) && !defined(PIN_CRT)
      _snprintf_s(tbuf, TBUFSZ, TBUFSZ, "%s=%s",
                  xed_operand_enum_t2str(op_name), xed_reg_enum_t2str(r));
#else
      snprintf(tbuf, TBUFSZ, "%s=%s", xed_operand_enum_t2str(op_name),
               xed_reg_enum_t2str(r));
#endif
      break;
    }
    default:
      printf("need to add support for printing operand: %s",
             xed_operand_enum_t2str(op_name));
      assert(0);
    }
    printf("%21s", tbuf);

    rw = xed_decoded_inst_operand_action(xedd, i);

    printf(
        " %10s %3s %9s",
        xed_operand_visibility_enum_t2str(xed_operand_operand_visibility(op)),
        xed_operand_action_enum_t2str(rw),
        xed_operand_width_enum_t2str(xed_operand_width(op)));

    bits = xed_decoded_inst_operand_length_bits(xedd, i);
    printf("  %3u", bits);
    /* rounding, bits might not be a multiple of 8 */
    printf("  %4u", (bits + 7) >> 3);

    printf("    %2u", xed_decoded_inst_operand_elements(xedd, i));
    printf("    %3u", xed_decoded_inst_operand_element_size_bits(xedd, i));

    printf(" %10s", xed_operand_element_type_enum_t2str(
                        xed_decoded_inst_operand_element_type(xedd, i)));
    printf(" %10s\n", xed_reg_class_enum_t2str(xed_reg_class(
                          xed_decoded_inst_get_reg(xedd, op_name))));
  }
}
