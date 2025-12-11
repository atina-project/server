#include"core/utils/crypto.h"

#include<vector>

#include"sodium/core.h"
#include"sodium/crypto_pwhash.h"
#include"sodium/randombytes.h"
#include"sodium/utils.h"

using namespace atina::server::core;

int utils::crypto::init(){
    if (sodium_init() < 0)
    {
        _is_inited = false;
        return 1;
    }
    _is_inited = true;
    return 0;
}

bool utils::crypto::is_init(){
    return _is_inited;
}

std::string utils::crypto::get_random_str(unsigned int __bytes){
    if (!_is_inited)
    {
        return "";
    } // libsodium not inited

    std::vector<unsigned char> buf(__bytes);
    randombytes_buf(buf.data(), __bytes);

    std::string hex_str(__bytes * 2 + 1, '\0');
    sodium_bin2hex(&hex_str[0], hex_str.size(), buf.data(), __bytes);
    sodium_memzero(buf.data(), buf.size());
    hex_str.resize(__bytes * 2);

    return hex_str;
}

std::string utils::crypto::get_pswd_hash(const std::string& __cr_pswd, bool __clear_pswd){
    if (!_is_inited)
    {
        return "";
    } // libsodium not inited

    char out[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(
        out,
        __cr_pswd.c_str(), __cr_pswd.size(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE
    ) != 0)
    {
        return "";
    }

    if (__clear_pswd)
    {
        sodium_memzero(
            const_cast<char*>(__cr_pswd.data()),
            __cr_pswd.size()
        );
    } // overwrite pswd mem
    
    return std::string(out);
}

int utils::crypto::compare_pswd_hash(const std::string& __cr_stored_pswd_hash, const std::string& __cr_pswd, bool __clear_pswd){
    if (!_is_inited)
    {
        return -1;
    } // libsodium not inited

    int ret = crypto_pwhash_str_verify(
        __cr_stored_pswd_hash.c_str(),
        __cr_pswd.c_str(), __cr_pswd.size()
    );

    if (__clear_pswd)
    {
        sodium_memzero(
            const_cast<char*>(__cr_pswd.data()),
            __cr_pswd.size()
        );
    } // overwrite pswd mem

    return (ret == 0) ? 0 : 1;
}
