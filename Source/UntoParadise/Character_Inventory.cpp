// Created by Snow Paw Games


#include "Character_Inventory.h"

// Sets default values for this component's properties
UCharacter_Inventory::UCharacter_Inventory()
{

}

void UCharacter_Inventory::AddToGeneralItems(const FName KeyID, const int ItemCount)
{
	if (GeneralItems.Contains(KeyID))
	{
		if (const int* CurrentItemCount = GeneralItems.Find(KeyID))
		{
			const int NewItemCount = *CurrentItemCount + ItemCount;
			GeneralItems[KeyID] = NewItemCount;
		}
	}
	else
	{
		GeneralItems.Add(KeyID, ItemCount);
	}
}

void UCharacter_Inventory::AddToKeyItems(const FName KeyID, const int ItemCount)
{
	 if (KeyItems.Contains(KeyID))
	 {
		 if (const int* CurrentItemCount = KeyItems.Find(KeyID))
		 {
			const int NewItemCount = *CurrentItemCount + ItemCount;
			KeyItems[KeyID] = NewItemCount;
		 }
	 }
	 else
	 {
	 	KeyItems.Add(KeyID, ItemCount);
	 }
}
