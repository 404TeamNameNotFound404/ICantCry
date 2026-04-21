// Fill out your copyright notice in the Description page of Project Settings.


#include "CircularBulletBuffer.h"

#include "ICantCry/ICC/Debug/DebugHelper.h"

UCircularBulletBuffer::UCircularBulletBuffer()
{
   
}

void UCircularBulletBuffer::Initialize(int32 Size)
{
    Buffer.SetNumZeroed(Size);
    Capacity = Size;
    Head = 0;
    Tail = 0;
    CurrentIndex = 0;
    bIsFull = false;
}

void UCircularBulletBuffer::AddBullet(UBulletData *Bullet)
{
    if(IsFull())
    {
        // // Overwrites the oldest bullet if the buffer is full
        // Buffer[Head] = Bullet;
        // Head = (Head + 1) % Capacity;
        // Tail = (Tail + 1) % Capacity;

        return;
    }
    
    Buffer[Head] = Bullet;
    Head = (Head + 1) % Capacity;
    bIsFull = (Head == Tail);
}

UBulletData *UCircularBulletBuffer::RemoveBullet()
{
    if(IsEmpty())
    {
        return nullptr;
    }

    UBulletData* Bullet = Buffer[Tail];
    Buffer[Tail] = nullptr;
    Tail = (Tail + 1) % Capacity;
    bIsFull = false;

    if (Head == Tail)
    {
        Head = 0;
        Tail = 0;
        CurrentIndex = 0;
    }
    
    return Bullet;
}

void UCircularBulletBuffer::RemoveAt(const int32& Index)
{
    if (Index < 0 || Index >= Capacity || IsEmpty())
        return;

    int32 Current = Index;
    int32 Next = (Current + 1) % Capacity;
    
    while (Next != Head)
    {
        Buffer[Current] = Buffer[Next];
        Current = Next;
        Next = (Next + 1) % Capacity;
    }
    Head = (Head - 1 + Capacity) % Capacity;
    
    Buffer[Head] = nullptr;
    bIsFull = false;
}

UBulletData *UCircularBulletBuffer::PeekCurrentBullet() const
{
    if (IsEmpty())
    {
        return nullptr;
    }
    
    return Buffer[CurrentIndex];
}

void UCircularBulletBuffer::MoveToNext()
{
    if (IsEmpty())
    {
        return;
    }

    CurrentIndex = (CurrentIndex + 1) % Capacity;
    while (Buffer[CurrentIndex] == nullptr && CurrentIndex != Head)
    {
        CurrentIndex = (CurrentIndex + 1) % Capacity;
    }
}

void UCircularBulletBuffer::MoveToPrevious()
{
    CurrentIndex = (CurrentIndex - 1 + Capacity) % Capacity;
    while (Buffer[CurrentIndex] == nullptr && CurrentIndex != Tail)
    {
        CurrentIndex = (CurrentIndex - 1 + Capacity) % Capacity;
    }
}

bool UCircularBulletBuffer::IsFull() const
{
    return bIsFull;
}

bool UCircularBulletBuffer::IsEmpty() const
{
    return (!bIsFull && (Head == Tail));
}

int32 UCircularBulletBuffer::GetCount() const
{

    if (IsEmpty()) 
    {
        return 0;
    }

    if (IsFull()) 
    {
        return Capacity;
    }
        
    //return (Head - Tail + Capacity) % Capacity;

    return (Head >= Tail) ? (Head - Tail) : (Capacity - Tail + Head);
}

UBulletData* UCircularBulletBuffer::PeekAt(int32 Index) const
{
    if (Index < 0 || Index >= Capacity)
    {
        return nullptr;
    }

    return Buffer[Index];
}

int32 UCircularBulletBuffer::GetCapacity() const
{
    return Capacity;
}

int32 UCircularBulletBuffer::GetTailIndex() const
{
    return Tail;
}

int32 UCircularBulletBuffer::GetHeadIndex() const
{
    return Head;
}

void UCircularBulletBuffer::SetAt(const int32& Index, UBulletData* Data)
{
    if (Index < 0 || Index >= Capacity)
        return;

    Buffer[Index] = Data;
    
    if (Data != nullptr)
    {
        bIsFull = (GetCount() == Capacity);
    }
}

TArray<UBulletData*> UCircularBulletBuffer::GetBulletsLeft()
{
    if (IsEmpty())
    {
        DebugHelper::LogMessage(20, FColor::White, "Empty Circular buffer");
        return TArray<UBulletData*>();
    }
    
    TArray<UBulletData*> Bullets;
    const int32 Count = GetCount();
    
    for (int32 Index = 0; Index < Count; Index++)
    {
        const int32 ActualIndex = (Tail + Index) % Capacity;
        if (Buffer[ActualIndex] == nullptr) continue;
        Bullets.Add(Buffer[ActualIndex]);
        DebugHelper::LogMessage(20, FColor::White, "Bullet left: " + Buffer[ActualIndex]->BulletName);
    }
    
    return Bullets;
}

void UCircularBulletBuffer::Clear()
{
    for (int32 i = 0; i < Capacity; ++i)
    {
        Buffer[i] = nullptr;
    }

    Head = 0;
    Tail = 0;
    CurrentIndex = 0;
    bIsFull = false;
}
