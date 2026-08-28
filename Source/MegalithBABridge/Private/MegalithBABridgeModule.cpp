#include "Modules/ModuleManager.h"
#include "IMegalithGraphFormatter.h"
#include "MegalithBAFormatterImpl.h"
#include "MegalithSettings.h"

DEFINE_LOG_CATEGORY(LogMegalithBABridge);

class FMegalithBABridgeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		const UMegalithSettings* Settings = GetDefault<UMegalithSettings>();
		if (!Settings || !Settings->bEnableBlueprintAssist)
		{
			UE_LOG(LogMegalithBABridge, Log,
				TEXT("MegalithBABridge: Blueprint Assist integration disabled in settings"));
			return;
		}

#if WITH_BLUEPRINT_ASSIST
		Formatter = MakeUnique<FMegalithBAFormatterImpl>();
		IModularFeatures::Get().RegisterModularFeature(
			IMegalithGraphFormatter::GetModularFeatureName(),
			Formatter.Get());
		UE_LOG(LogMegalithBABridge, Log,
			TEXT("MegalithBABridge: Registered BA graph formatter"));
#else
		UE_LOG(LogMegalithBABridge, Log,
			TEXT("MegalithBABridge: Blueprint Assist not found at compile time, bridge inactive"));
#endif
	}

	virtual void ShutdownModule() override
	{
#if WITH_BLUEPRINT_ASSIST
		if (Formatter.IsValid())
		{
			IModularFeatures::Get().UnregisterModularFeature(
				IMegalithGraphFormatter::GetModularFeatureName(),
				Formatter.Get());
			Formatter.Reset();
		}
#endif
	}

private:
#if WITH_BLUEPRINT_ASSIST
	TUniquePtr<FMegalithBAFormatterImpl> Formatter;
#endif
};

IMPLEMENT_MODULE(FMegalithBABridgeModule, MegalithBABridge)
