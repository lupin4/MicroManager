#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

void Print(const FString& Message, const FColor& Color)
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,8.f,Color,Message);
	}
}

void PrintLog(const FString& Message)
{
	UE_LOG(LogTemp,Warning,TEXT("%s"),*Message);
}


// show a message box with the given type and message
EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = true)
{
	if (bShowMsgAsWarning)
	{
		FText MsgTitle = FText::FromString(TEXT("Warning"));
		return FMessageDialog::Open(MsgType, FText::FromString(Message), MsgTitle);
	}
	else
	{
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}

// creates a custom notification in the lower right corner to notify the users about something
inline void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo NotifyInfo(FText::FromString(Message));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.bFireAndForget = true;             // 🔧 Needed for auto-close
	NotifyInfo.FadeInDuration = 0.2f;
	NotifyInfo.FadeOutDuration = 0.5f;
	NotifyInfo.ExpireDuration = 4.0f;             // 🔧 Needed for auto-expire
	NotifyInfo.bUseSuccessFailIcons = true;
	NotifyInfo.bUseThrobber = false;

	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}
