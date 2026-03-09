#include "service.hpp"

AuthService::AuthService(int threads_size) {
  rngs.resize(threads_size);
  for (auto i = 0; i < threads_size; i++) {
    rngs[i] = std::make_unique<CryptoPP::AutoSeededRandomPool>();
  }
}

void AuthService::generate_salt(CryptoPP::byte* salt) {
  rngs[thread_num]->GenerateBlock(salt, SALT_SIZE);
}

std::string AuthService::generate_password_hash(const std::string& pass, const CryptoPP::byte* salt) {
  using namespace CryptoPP;

  const unsigned int iterations = 100000;
  const size_t derived_size = 32;
  const size_t derived_base64_size = 44; // ceil(32/3)*4

  PKCS5_PBKDF2_HMAC<SHA256> pbkdf;
  byte derived[derived_size];
  pbkdf.DeriveKey(
    derived, derived_size, 0,
    reinterpret_cast<const byte*>(pass.data()), pass.length(),
    salt, SALT_SIZE, iterations, 0.0f
  );

  std::string result;
  result.resize(derived_base64_size);
  ArraySource source(derived, derived_size, new Base64Encoder(
    new ArraySink((byte*)result.data(), derived_base64_size))
  );

  return result;
}

std::expected<std::string, AuthError> AuthService::generate_token(uint64_t user_id) {
  AuthToken token;
  token.user_id = user_id;
  rngs[thread_num]->GenerateBlock(reinterpret_cast<CryptoPP::byte*>(&token.random), 8);

  return encrypt_token(&token);
};

std::expected<std::string, AuthError> AuthService::encrypt_token(const AuthToken* token) {
  using namespace CryptoPP;

  std::string encrypted;
  byte iv[AES::BLOCKSIZE];
  rngs[thread_num]->GenerateBlock(iv, sizeof(iv));

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

std::expected<AuthToken, AuthError> AuthService::decrypt_token(const std::string encrypted_string) {
  using namespace CryptoPP;

  size_t first_part_size;
  if ((first_part_size = encrypted_string.find('.')) == std::string::npos) {
    return std::unexpected(AuthError::InvalidToken);
  } else if (encrypted_string.size() <= first_part_size + 1) {
    return std::unexpected(AuthError::InvalidToken);
  }

  const byte* encrypted_buf = reinterpret_cast<const byte*>(encrypted_string.data());
  const size_t encrypted_buf_size = encrypted_string.size();
  byte iv[AES::BLOCKSIZE];

  ArraySink iv_sink(iv, sizeof(iv));
  ArraySource(&encrypted_buf[first_part_size + 1], encrypted_buf_size - first_part_size - 1, true,
    new Base64Decoder(new Redirector(iv_sink))
  );
  if (iv_sink.TotalPutLength() != sizeof(iv)) return std::unexpected(AuthError::InvalidToken);

  return decrypt_token_first_part(encrypted_buf, first_part_size, iv);
}