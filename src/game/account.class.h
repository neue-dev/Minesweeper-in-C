/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-09 14:10:12
 * @ Modified time: 2024-03-16 17:26:56
 * @ Description:
 * 
 * Represents account objects we have during runtime
 */

#ifndef GAME_ACCOUNT_
#define GAME_ACCOUNT_

#include <stdlib.h>

typedef struct Account Account;

/**
 * A class that enables us to represent accounts during runtime.
 * The data associated with the accounts will be read from a text file.
 * 
 * @class
*/
struct Account {
  char *sAccountName;
  char *sPassword;
};

/**
 * Allocates memory for an instance of the Account class.
 * 
 * @return	{ Account * }		A pointer to the allocated memory.
*/
Account *Account_new() {
  Account *pAccount = calloc(1, sizeof(*pAccount));
  return pAccount;
}

/**
 * Initializes an instance of the Account class.
 * 
 * @param		{ Account * }		this	A pointer to the instance to initialize.
 * @return	{ Account * }					A pointer to the initialized instance.
*/
Account *Account_init(Account *this) {

  return this;
}

/**
 * Creates an initialized instance of the Account class.
 * 
 * @return	{ Account * }		A pointer to the newly created initialized instance.
*/
Account *Account_create() {
  return Account_init(Account_new());
}

/**
 * Deallocates the memory of an instance of the Account class.
 * 
 * @param		{ Account * }		this	A pointer to the instance to deallocate.
*/
void Account_kill(Account *this) {
  free(this);
}


#endif