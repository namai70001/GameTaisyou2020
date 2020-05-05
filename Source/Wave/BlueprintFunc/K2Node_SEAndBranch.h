// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_SEAndBranch.generated.h"

/**
 *
 */
UCLASS()
class UK2Node_SEAndBranch : public UK2Node
{
	GENERATED_BODY()
public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual bool CanEverInsertExecutionPin() const override { return true; }
	virtual bool CanEverRemoveExecutionPin() const override { return true; }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	// End UK2Node interface

public:
	FName GetPinNameGivenIndex(int32 Index) const;           // ���O��Condition�s�����̎擾
	FName GetUniquePinName() const;                          // ���g�p�ԍ���Condition�s�����̎擾
	UEdGraphPin* AddUniqueConditionPin();                    // ���g�p�ԍ���Condition�s���̒ǉ�
	void RemovePin(UEdGraphPin* TargetPin);                  // �w��s���̍폜
	bool IsHaveUnlinkConditionPin() const;                   // �m�[�h�������N�̂Ȃ�Condition�s���������Ă��邩�擾
	bool IsConditionPin(UEdGraphPin* TargetPin) const;       // �w��s����Condition�s�����擾

	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetElsePin() const;
	UEdGraphPin* GetConditionPinGivenIndex(const int32 Index);
};