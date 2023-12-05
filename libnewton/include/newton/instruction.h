#ifndef __LIBNEWTON_INSTRUCTION_H__
#define __LIBNEWTON_INSTRUCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef __BYTE_ORDER
#include <endian.h>
#endif

/* GNU extensions and C23 [[]] attributes support */
#if (__STDC_VERSION__ >= 202000L) && defined(__GNUC__)
#define __C23_ATTRIBUTES
#elif !defined(__GNUC__)
#error "GNU extensions are required"
#endif

/**
 * @brief Prism instruction set
 *
 */
typedef enum
#ifdef __C23_ATTRIBUTES
    [[gnu::packed]]
#else
    __attribute__((__packed__))
#endif
{
  /* Control instructions */
  PRISM_INSTRUCTION_UPDATE = 0x00,
  PRISM_INSTRUCTION_CLEAR = 0x01,
  PRISM_INSTRUCTION_SLEEP = 0x07,

  /* Selection instructions */
  PRISM_INSTRUCTION_SELECT = 0x02,
  PRISM_INSTRUCTION_RANGE = 0x03,

  /* Buffer alteration instructions */
  PRISM_INSTRUCTION_SET = 0x04,
  PRISM_INSTRUCTION_FILL = 0x05,

  /* Effects */
  PRISM_INSTRUCTION_BLUR = 0x06,

  /* Misc */
  PRISM_IGNORE_INSTRUCTION = 0xFE,
  PRISM_EXCEPTION = 0xFF,

} PrismInstructionSet;

typedef union
#ifdef __C23_ATTRIBUTES
    [[gnu::packed]]
#else
    __attribute__((__packed__))
#endif
{

  struct
#ifdef __C23_ATTRIBUTES
      [[gnu::packed]]
#else
      __attribute__((__packed__))
#endif
  {
    uint8_t _;
  } NoOptions;

  enum
#ifdef __C23_ATTRIBUTES
      [[gnu::packed]]
#else
      __attribute__((__packed__))
#endif
  {
    PRISM_OPTION_SELECT_ABSOLUTE = 0,
    PRISM_OPTION_SELECT_RELATIVE = 1
  } SelectOptions;

  enum
#ifdef __C23_ATTRIBUTES
      [[gnu::packed]]
#else
      __attribute__((__packed__))
#endif
  {
    PRISM_OPTION_RANGE_ABSOLUTE_START = 0,
    PRISM_OPTION_RANGE_RELATIVE_START = 1,
    PRISM_OPTION_RANGE_ABSOLUTE_END = 2,
    PRISM_OPTION_RANGE_RELATIVE_END = 3,
  } RangeOptions;

  enum
#ifdef __C23_ATTRIBUTES
      [[gnu::packed]]
#else
      __attribute__((__packed__))
#endif
  {
    PRISM_OPTION_COLOR_RED = 0,
    PRISM_OPTION_COLOR_GREEN = 1,
    PRISM_OPTION_COLOR_BLUE = 2,
  } ColorOptions;

  enum
#ifdef __C23_ATTRIBUTES
      [[gnu::packed]]
#else
      __attribute__((__packed__))
#endif
  {
    PRISM_OPTION_APPLY_ALL = 0,
    PRISM_OPTION_APPLY_RANGE = 1,
  } EffectOptions;

  enum
#ifdef __C23_ATTRIBUTES
      [[gnu::packed]]
#else
      __attribute__((__packed__))
#endif
  {
    PRISM_OPTION_TIME_US = 0,
    PRISM_OPTION_TIME_MS = 1,
    PRISM_OPTION_TIME_SEC = 2,
    PRISM_OPTION_TIME_MIN = 3,
  } TimeOptions;

} PrismInstructionOptions;

/**
 * @brief Instruction byte-fields definition
 * @note Endianess dependent
 */
typedef struct
#ifdef __C23_ATTRIBUTES
    [[gnu::packed, gnu::aligned(sizeof(uint8_t))]]
#else
    __attribute__((__packed__, aligned(sizeof(uint8_t))))
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
    __NewtonRawInstruction_BigEndian {
  uint8_t instruction : 6;
  uint8_t options : 2;
  uint8_t value;
}
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    __NewtonRawInstruction_LittleEndian {
  uint8_t value;
  uint8_t options : 2;
  uint8_t instruction : 6;
}
#else
#error "No byte order detected (missing 'endian.h' or undefined __BYTE_ORDER)"
#endif
_NewtonRawInstruction;

/**
 * @brief Prism parsed instruction
 *
 */
typedef struct {
  PrismInstructionSet instruction;
  PrismInstructionOptions options;
  uint8_t value;
} PrismInstruction;

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif
