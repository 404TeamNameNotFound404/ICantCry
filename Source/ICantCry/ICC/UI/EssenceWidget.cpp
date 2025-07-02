#include "EssenceWidget.h"

void UEssenceWidget::Setup(const FEssence &Required, int32 OwnedQuantity)
{
    check(EssenceName)
    
    if (EssenceName)
    {
        EssenceName->SetText(FText::FromString(Required.GetName(Required.EssenceType)));
    }

    check(EssenceQuantity)
    
    if (EssenceQuantity)
    {
        FString QuantityStr = FString::FromInt(OwnedQuantity);
        EssenceQuantity->SetText(FText::FromString(QuantityStr));
    }
}