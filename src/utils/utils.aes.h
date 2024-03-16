/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-16 17:27:09
 * @ Modified time: 2024-03-16 17:36:25
 * @ Description:
 * 
 * A library that implements the advanced encryption standard (AES).
 */

#define AES_ROUND_COUNT 14
#define AES_KEY_LENGTH 256

typedef struct AES AES;

/**
 * This struct just stores some information about the algorithm.
 * It's not instantiable.
 * 
 * @struct
*/
struct AES {

  int dTable[4][4];                     // A substitution table
  int dCurrentBlock[4][4];              // The current block we're working on
  
  char *sKeySchedule[AES_ROUND_COUNT];  // The set of keys to be used for encryption
};

/**
 * Initializes some of the values we need to perform AES.
*/
void AES_init() {
  // ! Init the lookup table
}

void AES_exit() {
  // ! Free any allocated memory
}

/**
 * Creates the keys to be used for encryption from the main key.
*/
void AES_createKeySchedule() {
  
}

/**
 * Substitutes bytes within a block through the lookup table.
*/
void AES_subBytes() {

}

/**
 * Offsets the rows in a block.
*/
void AES_shiftRows() {
  
}

/**
 * Mutates the columns within a block.
*/
void AES_mixCols() {
  
}

/**
 * Encrypts a bunch of blocks by calling the helper functions above. 
*/
void AES_encryptBlob() {
  
}