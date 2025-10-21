#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

namespace ML
{
	/**
	 * Encryption key for ML model data
	 * Using AES-256 style key structure
	 */
	struct EncryptionKey
	{
		std::array<uint8_t, 32> Key;
		std::array<uint8_t, 16> IV; // Initialization Vector

		EncryptionKey()
		{
			Key.fill(0);
			IV.fill(0);
		}
	};

	/**
	 * Encrypted model data container
	 * Stores encrypted ML model with metadata
	 */
	struct EncryptedModelData
	{
		std::vector<uint8_t> EncryptedData;
		uint32_t OriginalSize;
		uint32_t EncryptedSize;
		uint32_t ModelVersion;
		std::string ModelName;

		EncryptedModelData()
			: OriginalSize(0), EncryptedSize(0), ModelVersion(1)
		{
		}
	};

	/**
	 * XOR-based encryption/decryption for model data
	 * Simple but effective for obfuscation
	 */
	class ModelEncryption
	{
	public:
		/**
		 * Encrypt model data using XOR encryption
		 * @param Data - Raw model data to encrypt
		 * @param Key - Encryption key
		 * @return Encrypted model data container
		 */
		static EncryptedModelData Encrypt(const std::vector<uint8_t>& Data, const EncryptionKey& Key)
		{
			EncryptedModelData Result;
			Result.OriginalSize = static_cast<uint32_t>(Data.size());
			Result.EncryptedData.resize(Data.size());

			// XOR encryption with key
			for (size_t i = 0; i < Data.size(); ++i)
			{
				uint8_t KeyByte = Key.Key[i % Key.Key.size()];
				uint8_t IVByte = Key.IV[i % Key.IV.size()];
				Result.EncryptedData[i] = Data[i] ^ KeyByte ^ IVByte;
			}

			Result.EncryptedSize = static_cast<uint32_t>(Result.EncryptedData.size());
			return Result;
		}

		/**
		 * Decrypt model data
		 * @param EncryptedData - Encrypted model data
		 * @param Key - Decryption key (same as encryption key)
		 * @return Decrypted raw data
		 */
		static std::vector<uint8_t> Decrypt(const EncryptedModelData& EncryptedData, const EncryptionKey& Key)
		{
			std::vector<uint8_t> Result(EncryptedData.OriginalSize);

			// XOR decryption (same operation as encryption)
			for (size_t i = 0; i < EncryptedData.EncryptedData.size() && i < Result.size(); ++i)
			{
				uint8_t KeyByte = Key.Key[i % Key.Key.size()];
				uint8_t IVByte = Key.IV[i % Key.IV.size()];
				Result[i] = EncryptedData.EncryptedData[i] ^ KeyByte ^ IVByte;
			}

			return Result;
		}

		/**
		 * Generate a random encryption key
		 * @return Generated encryption key
		 */
		static EncryptionKey GenerateKey()
		{
			EncryptionKey Key;
			
			// Simple pseudo-random key generation
			// In production, use proper cryptographic RNG
			for (size_t i = 0; i < Key.Key.size(); ++i)
			{
				Key.Key[i] = static_cast<uint8_t>((i * 7 + 13) ^ 0xAB);
			}
			for (size_t i = 0; i < Key.IV.size(); ++i)
			{
				Key.IV[i] = static_cast<uint8_t>((i * 11 + 17) ^ 0xCD);
			}

			return Key;
		}
	};

	/**
	 * ML model security utilities
	 */
	namespace Security
	{
		/**
		 * Verify model integrity using simple checksum
		 * @param Data - Model data to verify
		 * @return Checksum value
		 */
		inline uint32_t CalculateChecksum(const std::vector<uint8_t>& Data)
		{
			uint32_t Checksum = 0;
			for (uint8_t Byte : Data)
			{
				Checksum = (Checksum << 1) ^ Byte;
			}
			return Checksum;
		}

		/**
		 * Verify encrypted model integrity
		 * @param Model - Encrypted model to verify
		 * @return True if model appears valid
		 */
		inline bool VerifyModel(const EncryptedModelData& Model)
		{
			return Model.OriginalSize > 0 && 
			       Model.EncryptedSize > 0 && 
			       Model.EncryptedSize == Model.EncryptedData.size();
		}
	}
}
