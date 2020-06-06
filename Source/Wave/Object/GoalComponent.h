// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "GoalComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAVE_API UGoalComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoalComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Category = "C++Event", BlueprintCallable)
		void OnFloatActorCheck(class AActor* OtherActor);

	// �C�x���g�F�h�A��߂�A�j���[�V�������u���[�v�����g���ō쐬
	//UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		//void PlayAnimationDoorClose();

	// �C�x���g�F�Ƃ��j�󂳂��
	//UFUNCTION(Category = "C++Event", BlueprintImplementableEvent, BlueprintCallable)
		//void BreakHome();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "C++Library")
		bool GetIsGoal() const { return isGoal; }
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		bool GetIsExplotion()const { return isExplotion; };

private:
	bool isGoal;
	bool isExplotion; //�����Ă��܂����I�I
};
