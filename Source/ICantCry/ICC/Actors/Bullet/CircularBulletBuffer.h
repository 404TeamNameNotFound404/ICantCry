// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Bullet/BulletData.h"
#include "CircularBulletBuffer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ICANTCRY_API UCircularBulletBuffer : public UObject
{
	GENERATED_BODY()
	


public:

	UCircularBulletBuffer();


	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
	void Initialize(int32 Size); // init the buffer with a specific size

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
	void AddBullet(UBulletData* Bullet); // add bullet to the buffer

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
	UBulletData* RemoveBullet();
	
	void RemoveAt(const int32& Index);
	
	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
	UBulletData* PeekCurrentBullet() const;  // get the current bullet without remove it

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
    void MoveToNext();

    UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
    void MoveToPrevious();

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
    bool IsFull() const;

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
    bool IsEmpty() const;

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")  // returns the number of bullets in the buffer
	int32 GetCount() const;

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")  // when you update the revolver UI, I want to display the bullets but not remove them so I use this method
	UBulletData* PeekAt(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
	int32 GetCapacity() const;

	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
    int32 GetTailIndex() const;
	
	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
	int32 GetHeadIndex() const;
	
	UFUNCTION(BlueprintCallable, Category = "Bullet Buffer")
	void SetAt(const int32& Index, UBulletData* Data);

	void Clear();



private:

	UPROPERTY()
	TArray<UBulletData*> Buffer;

	UPROPERTY() int32 Head = 0;
	UPROPERTY() int32 Tail = 0;
	UPROPERTY() int32 CurrentIndex = 0;
	UPROPERTY() int32 Capacity = 0;
	UPROPERTY() int32 BulletCount = 0;

	UPROPERTY() bool bIsFull = false;

};
