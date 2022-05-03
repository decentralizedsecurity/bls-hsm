#ifndef import_keystore_h
#define import_kesytore_h
int get_decryption_key(char* keystore, char* password, unsigned char* decryption_key);
int verificate_password(char* keystore, unsigned char* decription_key);
int get_private_key(char* keystore, char* decription_key, char* private_key);
int import_from_keystore(char** keystores, char** passwords, int nKeys);
#endif
