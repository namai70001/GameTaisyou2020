// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
// �S�[���N���X

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Goal.generated.h"

UCLASS()
class WAVE_API AGoal : public AActor
{
	GENERATED_BODY()
private:

public:
	// Sets default values for this actor's properties
	AGoal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		USceneComponent* Scene;
	// �ו��̏Փ˂𔻒f����R���W����
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		USphereComponent* SphereComp;

	//OnComponentBeginOverlap
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// �h�A��߂�A�j���[�V�������u���[�v�����g���ō쐬
	UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		void PlayAnimationDoorClose();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "C++Library")
		bool GetIsGoal() const { return isGoal; }
private:
	bool isGoal;

};