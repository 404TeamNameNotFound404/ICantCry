// Fill out your copyright notice in the Description page of Project Settings.


#include "CircularBulletBuffer.h"

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
        // Overwrites the oldest bullet if the buffer is full
        Buffer[Head] = Bullet;
        Head = (Head + 1) % Capacity;
        Tail = (Tail + 1) % Capacity;
    }
    else
    {
        Buffer[Head] = Bullet;
        Head = (Head + 1) % Capacity;
        bIsFull = (Head == Tail);
    }
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
    
    return Bullet;
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
