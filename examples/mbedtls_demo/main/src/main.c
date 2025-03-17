/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <string.h>
#include <mbedtls/aes.h>

int main() {
    // Define AES key and input data for encryption
    const unsigned char key[16] = "1234567890abcdef"; // 128-bit key (16 bytes)
    unsigned char input[16] = "HelloAESExample!";    // 16-byte data to be encrypted
    unsigned char output[16];                        // Buffer to store encryption result
    unsigned char decrypted[16];                     // Buffer to store decryption result

    // Initialize AES encryption context
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    // Set the encryption key
    if (mbedtls_aes_setkey_enc(&aes, key, 128) != 0) {
        printf("Failed to set AES encryption key\n");
        return -1;
    }

    // Perform AES encryption
    if (mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input, output) != 0) {
        printf("AES encryption failed\n");
        return -1;
    }

    // Print the encrypted data
    printf("Encrypted data: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", output[i]);
    }
    printf("\n");

    // Set the decryption key
    if (mbedtls_aes_setkey_dec(&aes, key, 128) != 0) {
        printf("Failed to set AES decryption key\n");
        return -1;
    }

    // Perform AES decryption
    if (mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, output, decrypted) != 0) {
        printf("AES decryption failed\n");
        return -1;
    }

    // Print the decrypted data
    printf("Decrypted data: ");
    for (int i = 0; i < 16; i++) {
        printf("%c", decrypted[i]);
    }
    printf("\n");

    // Free AES context
    mbedtls_aes_free(&aes);

    return 0;
}
