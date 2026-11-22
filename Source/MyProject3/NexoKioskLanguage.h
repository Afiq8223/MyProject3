#pragma once

#include "CoreMinimal.h"

enum class ENexoKioskLanguage : uint8 { English, Malay, Tamil, Chinese };

FString TranslateKioskText(FName WidgetName, const FString& English, ENexoKioskLanguage Language);
