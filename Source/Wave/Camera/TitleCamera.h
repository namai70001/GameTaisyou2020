// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//�^�C�g���p�J�����N���X
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitleCamera.generated.h"
class USpringArmComponent;
class UCameraComponent;
class ATitleMemo;
UCLASS()
class WAVE_API ATitleCamera : public AActor
{
	GENERATED_BODY()
		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
private:
	ATitleMemo* TitleMemo;
	int MemoNum;//�X�e�[�W�I��`�[�̑������i�[
	bool IsSelectMap = false;
	void TitleInput();
public:	
	// Sets default values for this actor's properties
	ATitleCamera();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instanced")
		int MyStageNumber = 0;
	UFUNCTION(BlueprintCallable, Category = "C++Library")
		void GetAllTitleMemo();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};