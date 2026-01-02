#ifndef TRANSLATION_UNIT_H
#define TRANSLATION_UNIT_H
#define MAX_MEMORY_SIZE 1024

#define STARTING_ADDRESS 100

/**
 * Structure representing the entire program during both passes of the assembler.
 * Holds all memory images, symbol metadata, and output tracking structures.
 */
struct translation_unit {
       int code_image[MAX_MEMORY_SIZE];                     /** Holds encoded instruction words */
       int IC;                              /** Instruction Counter starting at 100 */
       int data_image[MAX_MEMORY_SIZE];                     /** Holds encoded .data and .string values */
       int DC;                              /** Data Counter */
       struct symbol *symbol_table;        /** Symbol table with labels and their attributes */
       int symCount;                       /** Number of defined symbols */
       int symCapacity;                    /** Capacity of the symbol table */
       struct ext *externals;              /** External symbol references */
       int extCount;                       /** Number of externals */
       int extCapacity;                    /** Capacity of the externals array */
       struct symbol **entries;            /** Pointers to symbols marked as entry */
       int entries_count;                  /** Number of entries */
       int entries_capacity;               /** Capacity of the entries array */
};

/**
 * Represents a symbol (label) in the program.
 * Each symbol has a name, type (code/data/entry/extern), and address.
 */
struct symbol {
       char symName[32];  /** Name of the symbol (max 31 chars + null) */
       enum {
              symExtern,
              symEntry,
              symCode,
              symData,
              symEntryCode,
              symEntryData
       } symType;
       int address;       /** Address in memory */
};

/**
 * Represents an external symbol usage and all addresses referencing it.
 */
struct ext {
       char *externalName;       /** Name of the external symbol */
       int addresses[MAX_MEMORY_SIZE];      /** Memory locations where it is used */
       int address_count;        /** Number of times it was used */
};



#endif
