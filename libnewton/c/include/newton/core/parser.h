#ifndef _LIBNEWTON_CORE_PARSER_H__
#define _LIBNEWTON_CORE_PARSER_H__

#include <stdint.h>

#ifdef __cplusplus
#warning "This is a C library, use cpplibnewton instead"
#endif

#include "instruction.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Parse a 16 byte integer (Little Endian) into a
 * \struct{PrismInstruction}
 *
 * @param byte Raw 16 bytes in LittleEndian format
 * @return PrismInstruction Parsed instruction
 */
static inline PrismInstruction Newton_ParseInstructionU16(uint16_t byte) {

  _NewtonRawInstruction rawInstruction = {
      .instruction = (uint8_t)((byte & 0xFC00) >> 10),
      .options = (uint8_t)((byte & 0x0300) >> 8),
      .value = (uint8_t)(byte & 0x00FF),
  };

  PrismInstruction parsedInstruction = {
      .instruction = (PrismInstructionSet)rawInstruction.instruction,
      .value = rawInstruction.value};

  uint8_t optionsValue = rawInstruction.options;
  memcpy(&parsedInstruction.options, &optionsValue, sizeof(uint8_t));

  return parsedInstruction;
}

/**
 * @brief Parse instruction into a 16-bit binary format (Little Endian)
 *
 * @param instruction Instruction to be parsed
 * @return uint16_t 16 bit result
 */
static inline uint16_t
Newton_WriteInstructionToU16(const PrismInstruction instruction) {

  const _NewtonRawInstruction rawInstruction = {
      .instruction = instruction.instruction,
      .options = *(uint8_t *)(&instruction.options),
      .value = instruction.value};

  uint16_t instructionBytes = 0x0000UL;

  instructionBytes |= ((uint16_t)rawInstruction.options << 8);
  instructionBytes |= (((uint16_t)rawInstruction.instruction << 10) & 0xFF00);
  instructionBytes |= rawInstruction.value;

  // Parse value into bits
  return instructionBytes;
}

/**
 * @brief Parse an instruction literal into a \struct{PrismInstruction}
 *
 * @param instruction Literal with the instruction without \\n character
 * @return PrismInstruction Built Parsed instruction
 */
static inline PrismInstruction
Newton_ParseInstructionLiteral(const char *instruction) {

  // If instruction pointer is null return exception
  if (instruction == NULL) {
    return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                              .options = {.NoOptions = {}}};
  }

  // Contains comment or empty line (Ignore instruction)
  if ((strncmp(instruction, "--", 2) == 0) || (strlen(instruction) <= 2)) {
    return (PrismInstruction){.instruction = PRISM_IGNORE_INSTRUCTION,
                              .options = {.NoOptions = {}}};
  }

  // Generate the instruction
  PrismInstruction parsedInstruction = (PrismInstruction){
      .instruction = PRISM_IGNORE_INSTRUCTION, .options = {.NoOptions = {}}};

  // Tokenize instructions
  char *token = strtok((char *)instruction, " ");

  /* Ignore instruction */
  if (strcmp(token, "NOP") == 0) {
    return parsedInstruction; /* Do nothing */
  }

  /* Update the buffer */
  else if (strcmp(token, "UPDT") == 0) {
    parsedInstruction.instruction = PRISM_INSTRUCTION_UPDATE;
    return parsedInstruction;
  }

  /* Clear the buffer */
  else if (strcmp(token, "CLR") == 0) {
    parsedInstruction.instruction = PRISM_INSTRUCTION_CLEAR;
    return parsedInstruction;
  }

  else if (strcmp(token, "LDX") == 0 || strcmp(token, "LDY") == 0) {

    // Set the instruction
    parsedInstruction.instruction = strcmp(token, "LDX") == 0
                                        ? PRISM_INSTRUCTION_LOADX
                                        : PRISM_INSTRUCTION_LOADY;
    // Next token is mode
    token = strtok(NULL, " ");

    switch (token[0]) {
    case '$':
      parsedInstruction.options =
          (PrismInstructionOptions){.LoadOptions = PRISM_OPTION_LOAD_VARIABLE};
      break;

    case 'R':
      parsedInstruction.options =
          (PrismInstructionOptions){.LoadOptions = PRISM_OPTION_LOAD_R};
      break;

    case 'G':
      parsedInstruction.options =
          (PrismInstructionOptions){.LoadOptions = PRISM_OPTION_LOAD_G};
      break;

    case 'B':
      parsedInstruction.options =
          (PrismInstructionOptions){.LoadOptions = PRISM_OPTION_LOAD_B};
      break;

      // Invalid token detected
    default:
      return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                                .options = {.NoOptions = {}}};
    }
  }

  /* Select a value from buffer */
  else if (strcmp(token, "SEL") == 0) {

    parsedInstruction.instruction = PRISM_INSTRUCTION_SELECT;
    // Next token is mode
    token = strtok(NULL, " ");

    switch (token[0]) {
      /* For relative selection */
    case '%':
      parsedInstruction.options = (PrismInstructionOptions){
          .SelectOptions = PRISM_OPTION_SELECT_RELATIVE};
      break;

      /* For absolute selection */
    case '#':
      parsedInstruction.options = (PrismInstructionOptions){
          .SelectOptions = PRISM_OPTION_SELECT_ABSOLUTE};
      break;

      // Invalid token detected
    default:
      return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                                .options = {.NoOptions = {}}};
    }
  }

  /* Select a range from buffer */
  else if (strcmp(token, "RAN") == 0) {

    parsedInstruction.instruction = PRISM_INSTRUCTION_RANGE;
    token = strtok(NULL, " ");

    if (strcmp(token, "%") == 0) {
      parsedInstruction.options = (PrismInstructionOptions){
          .RangeOptions = PRISM_OPTION_RANGE_RELATIVE_START};
    } else if (strcmp(token, "%%") == 0) {
      parsedInstruction.options = (PrismInstructionOptions){
          .RangeOptions = PRISM_OPTION_RANGE_RELATIVE_END};
    } else if (strcmp(token, "#") == 0) {
      parsedInstruction.options = (PrismInstructionOptions){
          .RangeOptions = PRISM_OPTION_RANGE_ABSOLUTE_START};
    } else if (strcmp(token, "##") == 0) {
      parsedInstruction.options = (PrismInstructionOptions){
          .RangeOptions = PRISM_OPTION_RANGE_ABSOLUTE_END};
    } else {

      // Invalid token detected
      return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                                .options = {.NoOptions = {}}};
    }
  }

  /* Fill a range with a color */
  else if (strcmp(token, "FILL") == 0) {

    parsedInstruction.instruction = PRISM_INSTRUCTION_FILL;
    token = strtok(NULL, " ");

    switch (token[0]) {
    case 'R':
      parsedInstruction.options =
          (PrismInstructionOptions){.ColorOptions = PRISM_OPTION_COLOR_RED};
      break;
    case 'G':
      parsedInstruction.options =
          (PrismInstructionOptions){.ColorOptions = PRISM_OPTION_COLOR_GREEN};
      break;
    case 'B':
      parsedInstruction.options =
          (PrismInstructionOptions){.ColorOptions = PRISM_OPTION_COLOR_BLUE};
      break;

      // Invalid token detected
    default:
      return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                                .options = {.NoOptions = {}}};
    }
  }

  /* Set the selected led with a color */
  else if (strcmp(token, "SET") == 0) {

    parsedInstruction.instruction = PRISM_INSTRUCTION_SET;
    token = strtok(NULL, " ");

    switch (token[0]) {
    case 'R':
      parsedInstruction.options =
          (PrismInstructionOptions){.ColorOptions = PRISM_OPTION_COLOR_RED};
      break;
    case 'G':
      parsedInstruction.options =
          (PrismInstructionOptions){.ColorOptions = PRISM_OPTION_COLOR_GREEN};
      break;
    case 'B':
      parsedInstruction.options =
          (PrismInstructionOptions){.ColorOptions = PRISM_OPTION_COLOR_BLUE};
      break;

      // Invalid token detected
    default:
      return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                                .options = {.NoOptions = {}}};
    }
  }

  /* Blur effect */
  else if (strcmp(token, "EBLR") == 0) {

    parsedInstruction.instruction = PRISM_INSTRUCTION_BLUR;
    token = strtok(NULL, " ");

    if (strcmp(token, "ALL") == 0) {
      parsedInstruction.options =
          (PrismInstructionOptions){.EffectOptions = PRISM_OPTION_APPLY_ALL};
    } else if (strcmp(token, "RAN") == 0) {
      parsedInstruction.options =
          (PrismInstructionOptions){.EffectOptions = PRISM_OPTION_APPLY_RANGE};
    } else {
      // Invalid token detected
      return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                                .options = {.NoOptions = {}}};
    }
  }

  /* Sleep for specified amount of time */
  else if (strcmp(token, "SLP") == 0) {

    parsedInstruction.instruction = PRISM_INSTRUCTION_SLEEP;
    token = strtok(NULL, " ");

    if (strcmp(token, "MS") == 0) {
      parsedInstruction.options = (PrismInstructionOptions){
          .TimeOptions =
#ifdef __cplusplus
              PrismInstructionOptions::
#endif
                  PRISM_OPTION_TIME_MS,
      };
    } else if (strcmp(token, "SEC") == 0) {
      parsedInstruction.options =
          (PrismInstructionOptions){.TimeOptions = PRISM_OPTION_TIME_SEC};
    } else if (strcmp(token, "MIN") == 0) {
      parsedInstruction.options = (PrismInstructionOptions){
          .TimeOptions =
#ifdef __cplusplus
              PrismInstructionOptions::
#endif
                  PRISM_OPTION_TIME_MIN,
      };
    } else {
      // Invalid token detected
      return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                                .options = {.NoOptions = {}}};
    }
  }

  /* Invalid instruction */
  else {
    // Invalid token detected
    return (PrismInstruction){.instruction = PRISM_EXCEPTION,
                              .options = {.NoOptions = {}}};
  }

  // Next token is value
  token = strtok(NULL, " ");
  parsedInstruction.value = (uint8_t)strtol(token, NULL, 16);

  // Return the parsed instruction
  return parsedInstruction;
}

#endif
