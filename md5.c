#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <Wincrypt.h>

#define MD5LEN 16

CHAR* get_md5(CHAR* text) {
    
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigits[] = "0123456789abcdef";

    // Get handle to the crypto provider
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        fprintf(stderr, "CryptAcquireContext failed: %lu\n", GetLastError()); 
        return NULL;
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        fprintf(stderr, "CryptAcquireContext failed: %lu\n", GetLastError()); 
        CryptReleaseContext(hProv, 0);
        return NULL;
    }

    // Crypt data
    if (!CryptHashData(hHash, (unsigned char*)text, strlen(text), 0)) {
        fprintf(stderr, "CryptHashData failed: %lu\n", GetLastError()); 
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        return NULL;
    }

    cbHash = MD5LEN;
    CHAR *str = NULL;

    // Get crypt data
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        if ((str = malloc(sizeof(CHAR) * 33)) != NULL) {
            str[0] = '\0';
            CHAR temp[3];
            for (DWORD i = 0; i < cbHash; i++) {
                sprintf(temp, "%c%c", rgbDigits[rgbHash[i] >> 4], rgbDigits[rgbHash[i] & 0xf]);
                strcat(str, temp);
            }
        }
    }
    else {
        fprintf(stderr, "CryptGetHashParam failed: %lu\n", GetLastError()); 
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return str;
}