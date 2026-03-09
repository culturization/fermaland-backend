#pragma once
#include "cryptlib.h"
#include "pwdbased.h"
#include "sha.h"
#include "osrng.h"
#include "base64.h"
#include "aes.h"
#include "ccm.h"

#include <string>
#include <expected>
#include <memory>

#include "threads_manager.hpp"

#define SALT_SIZE 16
#define SECRET_KEY_SIZE 32

struct AuthToken {
  uint64_t user_id;
  uint64_t random;
};

enum class AuthError {
  None,
  InvalidToken,
  CryptoPPError,
};

class AuthService {
public:
  AuthService(int threads_size);

  void generate_salt(CryptoPP::byte* salt);
  std::string generate_password_hash(const std::string& pass, const CryptoPP::byte* salt);

  std::expected<std::string, AuthError> generate_token(uint64_t user_id);
  std::expected<std::string, AuthError> encrypt_token(const AuthToken* token);

  std::expected<AuthToken, AuthError> decrypt_token(const std::string encrypted_string);
  inline std::expected<AuthToken, AuthError> decrypt_token_first_part(
    const CryptoPP::byte* encrypted_buf, size_t encrypted_buf_size, const CryptoPP::byte* iv
  ) {
    using namespace CryptoPP;

    AuthToken token;
    try {
      CBC_Mode<AES>::Decryption aes_dec;
      aes_dec.SetKeyWithIV(secret_key, SECRET_KEY_SIZE, iv);

      ArraySink token_sink(reinterpret_cast<byte*>(&token), sizeof(token));
      ArraySource(encrypted_buf, encrypted_buf_size, true,
        new Base64Decoder(new StreamTransformationFilter(aes_dec, new Redirector(token_sink)))
      );
      if (token_sink.TotalPutLength() != sizeof(token)) return std::unexpected(AuthError::InvalidToken);
    } catch (const CryptoPP::Exception) {
      return std::unexpected(AuthError::CryptoPPError);
    }

    return token;
  }

private:
  const size_t secret_key_size = 32;
  const CryptoPP::byte secret_key[SECRET_KEY_SIZE + 1] = "OrX4QfIU4wz51hxTzaguX8mDGT3f5gVE";

  std::vector<std::unique_ptr<CryptoPP::AutoSeededRandomPool>> rngs;
};