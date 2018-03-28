#include "CNTKLibrary.h"

namespace CNTK
{
	MomentumAsTimeConstantSchedule::MomentumAsTimeConstantSchedule(double value)
		: TrainingParameterSchedule<double>::TrainingParameterSchedule(value, UnitType::Sample)
	{
		ConvertToPerSampleValues();
	}

	MomentumAsTimeConstantSchedule::MomentumAsTimeConstantSchedule(const std::vector<double>& schedule, size_t epochSize)
		: TrainingParameterSchedule<double>::TrainingParameterSchedule(schedule, UnitType::Sample, epochSize)
	{
		ConvertToPerSampleValues();
	}

	MomentumAsTimeConstantSchedule::MomentumAsTimeConstantSchedule(const std::vector<std::pair<size_t, double>>& schedule, size_t epochSize)
		: TrainingParameterSchedule<double>::TrainingParameterSchedule(schedule, UnitType::Sample, epochSize)
	{
		ConvertToPerSampleValues();
	}
	///
	/// Create a string representation of 'this' NDShape for display/printing purposes
	///
	std::wstring NDShape::AsString() const
	{
		if (IsUnknown())
		{
			return L"[???]";
		}
		else
		{
			bool reverseShape = Internal::IsReversingTensorShapesInErrorMessagesEnabled();
			auto displayShape = *this;
			if (reverseShape)
			{
				for (size_t i = 0, j = Rank() - 1; i < Rank(); ++i, --j)
					displayShape[i] = (*this)[j];
			}

			std::wstringstream wStrStream;
			wStrStream << L"[";
			for (size_t i = 0; i < Rank(); i++)
			{
				if (i != 0)
					wStrStream << L" x ";

				if (displayShape[i] == InferredDimension)
					wStrStream << "?";
				else if (displayShape[i] == FreeDimension)
					wStrStream << "*";
				else
					wStrStream << displayShape[i];
			}
			wStrStream << L"]";
			return wStrStream.str();
		}
	}
}