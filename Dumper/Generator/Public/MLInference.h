#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "MLEncryption.h"

namespace ML
{
	/**
	 * ML model input/output data structure
	 */
	struct TensorData
	{
		std::vector<float> Data;
		std::vector<uint32_t> Shape; // Dimensions of the tensor
		std::string Name;

		TensorData() = default;

		TensorData(const std::vector<float>& InData, const std::vector<uint32_t>& InShape, const std::string& InName = "")
			: Data(InData), Shape(InShape), Name(InName)
		{
		}

		uint32_t GetTotalSize() const
		{
			uint32_t Size = 1;
			for (uint32_t Dim : Shape)
			{
				Size *= Dim;
			}
			return Size;
		}
	};

	/**
	 * ML model metadata
	 */
	struct ModelMetadata
	{
		std::string ModelName;
		std::string ModelVersion;
		std::string Framework; // "ONNX", "TensorFlow", etc.
		std::vector<std::string> InputNames;
		std::vector<std::string> OutputNames;
	};

	/**
	 * Encrypted ML model runtime
	 * Handles secure model loading and inference
	 */
	class EncryptedModelRuntime
	{
	private:
		EncryptedModelData EncryptedModel;
		std::vector<uint8_t> DecryptedModel;
		EncryptionKey Key;
		ModelMetadata Metadata;
		bool bIsLoaded;

	public:
		EncryptedModelRuntime()
			: bIsLoaded(false)
		{
		}

		/**
		 * Load encrypted model from data
		 * @param Model - Encrypted model data
		 * @param DecryptionKey - Key to decrypt the model
		 * @return True if model loaded successfully
		 */
		bool LoadModel(const EncryptedModelData& Model, const EncryptionKey& DecryptionKey)
		{
			if (!Security::VerifyModel(Model))
			{
				return false;
			}

			EncryptedModel = Model;
			Key = DecryptionKey;

			// Decrypt model data
			DecryptedModel = ModelEncryption::Decrypt(EncryptedModel, Key);

			// Verify decryption
			if (DecryptedModel.empty() || DecryptedModel.size() != EncryptedModel.OriginalSize)
			{
				return false;
			}

			bIsLoaded = true;
			return true;
		}

		/**
		 * Run inference on input data
		 * @param Input - Input tensor data
		 * @return Output tensor data
		 */
		TensorData RunInference(const TensorData& Input)
		{
			if (!bIsLoaded)
			{
				return TensorData();
			}

			// Placeholder for actual inference logic
			// In production, this would call into ML framework
			TensorData Output;
			Output.Name = "output";
			Output.Shape = Input.Shape;
			Output.Data = Input.Data;

			// Simple transformation example
			for (float& Value : Output.Data)
			{
				Value = Value * 0.5f; // Example transformation
			}

			return Output;
		}

		/**
		 * Run inference with multiple inputs
		 * @param Inputs - Vector of input tensors
		 * @return Vector of output tensors
		 */
		std::vector<TensorData> RunInference(const std::vector<TensorData>& Inputs)
		{
			std::vector<TensorData> Outputs;
			
			for (const auto& Input : Inputs)
			{
				Outputs.push_back(RunInference(Input));
			}

			return Outputs;
		}

		/**
		 * Get model metadata
		 */
		const ModelMetadata& GetMetadata() const
		{
			return Metadata;
		}

		/**
		 * Check if model is loaded and ready
		 */
		bool IsLoaded() const
		{
			return bIsLoaded;
		}

		/**
		 * Unload model and clear memory
		 */
		void Unload()
		{
			DecryptedModel.clear();
			DecryptedModel.shrink_to_fit();
			bIsLoaded = false;
		}
	};

	/**
	 * ML inference utilities for SDK
	 */
	namespace Inference
	{
		/**
		 * Create tensor from array
		 */
		template<typename T, size_t N>
		inline TensorData CreateTensor(const T(&Data)[N], const std::vector<uint32_t>& Shape, const std::string& Name = "")
		{
			std::vector<float> FloatData(Data, Data + N);
			return TensorData(FloatData, Shape, Name);
		}

		/**
		 * Create tensor from vector
		 */
		inline TensorData CreateTensor(const std::vector<float>& Data, const std::vector<uint32_t>& Shape, const std::string& Name = "")
		{
			return TensorData(Data, Shape, Name);
		}

		/**
		 * Normalize tensor data to [0, 1] range
		 */
		inline void NormalizeTensor(TensorData& Tensor, float Min = 0.0f, float Max = 1.0f)
		{
			float Range = Max - Min;
			if (Range == 0.0f)
			{
				return;
			}

			for (float& Value : Tensor.Data)
			{
				Value = (Value - Min) / Range;
			}
		}

		/**
		 * Denormalize tensor data from [0, 1] range
		 */
		inline void DenormalizeTensor(TensorData& Tensor, float Min = 0.0f, float Max = 1.0f)
		{
			float Range = Max - Min;

			for (float& Value : Tensor.Data)
			{
				Value = Value * Range + Min;
			}
		}
	}
}
