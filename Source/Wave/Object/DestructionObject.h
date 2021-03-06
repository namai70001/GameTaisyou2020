// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "../WaterSurface/WaterSurface.h"
#include "DestructionObject.generated.h"

UCLASS()
class WAVE_API ADestructionObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructionObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void BreakObject();
	///UPROPERTY()
	///	USceneComponent* Scene;
	///
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		USphereComponent* sphere;
private:
	AWaterSurface* WaterSurface;

	UPROPERTY(EditAnywhere)
		float MinFloatWavePower = 0.1f;
	UPROPERTY(EditAnywhere)
		float FloatSpeed = 0.7f;
	UPROPERTY(EditAnywhere)
		float FloatScale = 40.0f;
public:
	FVector Velocity;

};
