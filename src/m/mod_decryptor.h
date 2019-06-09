#ifndef MOD_DECRYPTOR_H
#define MOD_DECRYPTOR_H

#include <StudioModel.h>

bool Mod_IsCSOEncryptedModel(const studiohdr_t *studiohdr);
void Mod_DecryptModel(const char *model_name, studiohdr_t *studiohdr);


#endif