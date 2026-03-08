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
  AuthService(int threads_size) {
    for (auto it = rngs.begin(); it < rngs.end(); it++) rngs.emplace_back(it);
  }

  std::string generate_password_hash(const std::string& pass, const CryptoPP::byte* salt) {
    using namespace CryptoPP;

    const unsigned int iterations = 100000;
    const size_t derived_size = 64;

    PKCS5_PBKDF2_HMAC<SHA256> pbkdf;
    byte derived[derived_size];
    pbkdf.DeriveKey(
      derived, derived_size, 0,
      reinterpret_cast<const byte*>(pass.data()), pass.length(),
      salt, SALT_SIZE, iterations, 0.0f
    );

    std::string result;
    ArraySource source(derived, derived_size, new Base64Encoder(new StringSink(result)));

    return result;
  }

  std::expected<std::string, AuthError> encrypt_token(const unsigned int thread_num, const AuthToken* token) noexcept { // I JUST HOPE IT WON'T THROW EXCEPTIONS
    using namespace CryptoPP;

    std::string encrypted;
    byte iv[AES::BLOCKSIZE];
    rngs[thread_num].GenerateBlock(iv, sizeof(iv));

    try {
      CBC_Mode<AES>::Encryption aes_enc;
		  aes_enc.SetKeyWithIV(secret_key, SECRET_KEY_SIZE, iv);

      ArraySource source(reinterpret_cast<const byte*>(token), sizeof(*token), true,
        new StreamTransformationFilter(aes_enc, new Base64Encoder(new StringSink(encrypted)))
      );
    } catch (const CryptoPP::Exception) {
      return std::unexpected(AuthError::CryptoPPError);
    }

    encrypted.append(".");

    std::string iv_base64;
    ArraySource source(iv, sizeof(iv), true, new Base64Encoder(new StringSink(iv_base64))); // TODO: optimize it with ArraySink and static buffers
    encrypted.append(iv_base64);

    return encrypted;
  }

  std::expected<AuthToken, AuthError> decrypt_token(const std::string encrypted) noexcept {
    using namespace CryptoPP;

    size_t pos;
    if ((pos = encrypted.find('.')) == std::string::npos) {
      return std::unexpected(AuthError::InvalidToken);
    } else if (encrypted.size() == pos + 1) {
      return std::unexpected(AuthError::InvalidToken);
    }

    byte iv[AES::BLOCKSIZE];
    ArraySink iv_sink(iv, sizeof(iv));
    ArraySource(reinterpret_cast<const byte*>(encrypted.data() + pos + 1), encrypted.size() - pos - 1, true,
      new Base64Decoder(new Redirector(iv_sink))
    );
    if (iv_sink.TotalPutLength() != sizeof(iv)) return std::unexpected(AuthError::InvalidToken);

    AuthToken token;
    try {
      CBC_Mode<AES>::Decryption aes_dec;
		  aes_dec.SetKeyWithIV(secret_key, SECRET_KEY_SIZE, iv);

      ArraySink token_sink(reinterpret_cast<byte*>(&token), sizeof(token));
      ArraySource(reinterpret_cast<const byte*>(encrypted.data()), pos, true,
        new Base64Decoder(new StreamTransformationFilter(aes_dec, new Redirector(token_sink)))
      );
      if (token_sink.TotalPutLength() != sizeof(token)) return std::unexpected(AuthError::InvalidToken);
    } catch (const CryptoPP::Exception) {
      return std::unexpected(AuthError::CryptoPPError);
    }

    return token;
  }

  std::expected<std::string, AuthError> generate_token(const unsigned int thread_num, uint64_t user_id) noexcept {
    AuthToken token;
    token.user_id = user_id;
    rngs[thread_num].GenerateBlock(reinterpret_cast<CryptoPP::byte*>(&token.random), 8);

    return encrypt_token(thread_num, &token);
  };

private:
  const size_t secret_key_size = 32;
  const CryptoPP::byte secret_key[SECRET_KEY_SIZE + 1] = "OrX4QfIU4wz51hxTzaguX8mDGT3f5gVE";

  std::vector<CryptoPP::AutoSeededRandomPool> rngs;
};