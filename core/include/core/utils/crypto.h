#pragma once

#ifndef __ATINA_SERVER_CORE_UTILS_CRYPTO_H__
#define __ATINA_SERVER_CORE_UTILS_CRYPTO_H__

#include<string>

namespace atina::server::core::utils {

    class crypto {

        public:
            /**
             * Initialize libsodium features.
             * Call this function after program start.
             */
            static int init();
            static bool is_init();

            /**
             * Generate a hex-encoded random, crypto secured string.
             * Returns an empty string on error (not initialized).
             */
            static std::string get_random_str(unsigned int __bytes);

            /**
             * Get password argon2id hash (with opslimit and memlimit `_INTERACTIVE`).
             * Returns am empty string on error.
             * This function overwrites password in memory by default (with `sodium_memzero()`).
             */
            static std::string get_pswd_hash(std::string& __r_pswd, bool __clear_pswd = true);

            /**
             * Compare one password with a stored password hash.
             * Returns 0 on success, 1 on failed, -1 on error (not initialized).
             * This function overwrites password in memory by default (with `sodium_memzero()`) but will not overwrite stored password hash.
             */
            static int compare_pswd_hash(const std::string& __cr_stored_pswd_hash, std::string& __r_pswd, bool __clear_pswd = true);

            /**
             * Call `sodium_memzero()` to overwrite one string in memory.
             */
            static void memzero(std::string& __r_str);

        private:
            static inline bool _is_inited = false;

    }; // class crypto

} // namespace atina::server::core::utils

#endif // __ATINA_SERVER_CORE_UTILS_CRYPTO_H__
