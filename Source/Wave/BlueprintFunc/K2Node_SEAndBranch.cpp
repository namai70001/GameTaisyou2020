// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_SEAndBranch.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "GraphEditorSettings.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_Knot.h"

#define LOCTEXT_NAMESPACE "K2Node_SEAndBranch"

// �m�[�h�̃f�t�H���g�̃s����`
void UK2Node_SEAndBranch::AllocateDefaultPins()
{
	// ���͎��s�s��
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	// �o��Then���s�s��
	UEdGraphPin* TruePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	TruePin->PinFriendlyName = LOCTEXT("AllocateDefaultPins_True", "true");

	// �o��Else���s�s��
	UEdGraphPin* FalsePin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Else);
	FalsePin->PinFriendlyName = LOCTEXT("AllocateDefaultPins_False", "false");

	// ����Condition�s��
	AddUniqueConditionPin();

	Super::AllocateDefaultPins();
}

// �m�[�h�^�C�g���̐ݒ�
FText UK2Node_SEAndBranch::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GetNodeTitleDefault", "SE And Branch");
}

// �m�[�h�����̐ݒ�
FText UK2Node_SEAndBranch::GetTooltipText() const
{
	return LOCTEXT("GetTooltipText", "Evaluate from the smaller pin number.");
}

// �m�[�h�^�C�g���F�̐ݒ�
FLinearColor UK2Node_SEAndBranch::GetNodeTitleColor() const
{
	return GetDefault<UGraphEditorSettings>()->ExecBranchNodeTitleColor;
}

// �m�[�h�̃��j���[��ɂ�����J�e�S���̐ݒ�
FText UK2Node_SEAndBranch::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::FlowControl);
}

// �m�[�h�A�C�R���̐ݒ�
FSlateIcon UK2Node_SEAndBranch::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Branch_16x");
	return Icon;
}

// �ԍ���Condition�s���̖��O���擾
FName UK2Node_SEAndBranch::GetPinNameGivenIndex(int32 Index) const
{
	return *FString::Printf(TEXT("%s_%d"), *UEdGraphSchema_K2::PN_Condition.ToString(), Index);
}

// �w�肵���s����Condition�s�����擾
bool UK2Node_SEAndBranch::IsConditionPin(UEdGraphPin* TargetPin) const
{
	return TargetPin && TargetPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean;
}

// Then�s���̎擾�i���ExpandNode����ASpawnIntermediateNode��UK2Node_SEAndBranch���g����Ƃ��Ɏg�p����j
UEdGraphPin * UK2Node_SEAndBranch::GetThenPin() const
{
	UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_Then);
	check(Pin);
	return Pin;
}

// Else�s���̎擾�i���ExpandNode����ASpawnIntermediateNode��UK2Node_SEAndBranch���g����Ƃ��Ɏg�p����j
UEdGraphPin * UK2Node_SEAndBranch::GetElsePin() const
{
	UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_Else);
	check(Pin);
	return Pin;
}

// Condition�s���̎擾�i���ExpandNode����ASpawnIntermediateNode��UK2Node_SEAndBranch���g����Ƃ��Ɏg�p����j
UEdGraphPin * UK2Node_SEAndBranch::GetConditionPinGivenIndex(const int32 Index)
{
	return FindPin(GetPinNameGivenIndex(Index));
}

// ���C���[�̌q�����Ă��Ȃ�Condition�s�����܂ނ�
bool UK2Node_SEAndBranch::IsHaveUnlinkConditionPin() const
{
	for (const auto& Pin : Pins)
	{
		if (IsConditionPin(Pin) && Pin->LinkedTo.Num() == 0)
		{
			return true;
		}
	}
	return false;
}

// ���g�p�ԍ���Condition�s���̖��O���擾
FName UK2Node_SEAndBranch::GetUniquePinName() const
{
	FName NewPinName;
	for (int32 i = 0; true; i++)
	{
		NewPinName = GetPinNameGivenIndex(i);
		if (!FindPin(NewPinName))
		{
			break;
		}
	}
	return NewPinName;
}

// ���g�p�ԍ���Condition�s�����쐬����
UEdGraphPin * UK2Node_SEAndBranch::AddUniqueConditionPin()
{
	UEdGraphPin* Pin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, GetUniquePinName());
	Pin->DefaultValue = TEXT("true");
	return Pin;
}

// �s���̍폜
void UK2Node_SEAndBranch::RemovePin(UEdGraphPin * TargetPin)
{
	// �w�肵���s�����폜����
	DestroyPin(TargetPin);
	Pins.Remove(TargetPin);

	// Condition�s���̔ԍ����l�߂�
	int32 ThenIndex = 0;
	for (auto Pin : Pins)
	{
		if (IsConditionPin(Pin))
		{
			Pin->PinName = GetPinNameGivenIndex(ThenIndex++);
		}
	}
}

// �s���̒ǉ��ƍ폜�i�s���ƃ��C���[���ڑ����ꂽ��ؒf���ꂽ�Ƃ��ɌĂ΂��j
void UK2Node_SEAndBranch::PinConnectionListChanged(UEdGraphPin * Pin)
{
	if (IsConditionPin(Pin))
	{
		// Condition�s�����烏�C���[���ؒf���ꂽ�Ƃ��A�ؒf���ꂽ���C���[���폜
		if (Pin->LinkedTo.Num() == 0)
		{
			RemovePin(Pin);
		}

		// ���C���[�Ōq�������s�������Ȃ��Ƃ��A�s����ǉ�
		if (!IsHaveUnlinkConditionPin())
		{
			AddUniqueConditionPin();
		}

		// �m�[�h�O�ς��X�V����
		GetGraph()->NotifyGraphChanged();
	}

	Super::PinConnectionListChanged(Pin);
}

// �s���̍č\�z�i��ɂ��̃m�[�h������BlueprintGraph�����[�h���ꂽ�Ƃ��ɌĂ΂��֐��j
void UK2Node_SEAndBranch::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins); // ������AllocateDefaultPins()���Ă΂��

	const int32 AlreadyPinNum = Pins.Num();
	for (int i = 0; i < OldPins.Num() - AlreadyPinNum; i++)
	{
		AddUniqueConditionPin();
	}
}

// ���j���[�A�N�V�����̓o�^�i�R���e�L�X�g���j���[����m�[�h���X�|�[��������悤�ɂ���j
void UK2Node_SEAndBranch::GetMenuActions(FBlueprintActionDatabaseRegistrar & ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_SEAndBranch::ExpandNode(FKismetCompilerContext & CompilerContext, UEdGraph * SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	// Condition�s�����擾
	TArray<UEdGraphPin*> ConditionPins;
	for (int32 i = 0; true; i++)
	{
		// TODO
		// �K�v�ł���΃f�t�H���g�s���̒l�ɂ�鎩���ȃt���[������s��
		// �ETrue�f�t�H���g�s���́AConditionPins�Ɋ܂߂Ȃ�
		// �EFalse�f�t�H���g�s���́A���ʂ������i�K��Else�Ɏ��s�������j
		UEdGraphPin* ConditionPin = FindPin(GetPinNameGivenIndex(i));
		if (!ConditionPin)
		{
			break;
		}
		ConditionPins.Add(ConditionPin);
	}

	// Reroute�m�[�h���X�|�[������
	UK2Node_Knot* RerouteNode = CompilerContext.SpawnIntermediateNode<UK2Node_Knot>(this, SourceGraph);
	RerouteNode->AllocateDefaultPins();
	UEdGraphPin* RerouteExecPin = RerouteNode->GetInputPin();

	// Branch�m�[�h���X�|�[������
	TArray<UK2Node_IfThenElse*> BranchNodes;
	for (auto ConditionPin : ConditionPins)
	{
		UK2Node_IfThenElse* BranchNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
		BranchNode->AllocateDefaultPins();
		BranchNode->GetElsePin()->MakeLinkTo(RerouteExecPin);
		CompilerContext.MovePinLinksToIntermediate(*ConditionPin, *BranchNode->GetConditionPin());
		BranchNodes.Add(BranchNode);
	}

	// Branch�m�[�h���m��ڑ�����
	for (int32 i = 0; i < BranchNodes.Num() - 1; i++)
	{
		UEdGraphPin* PrevBranchThenPin = BranchNodes[i]->GetThenPin();
		UEdGraphPin* PostBranchExecPin = BranchNodes[i + 1]->GetExecPin();
		PrevBranchThenPin->MakeLinkTo(PostBranchExecPin);
	}

	// �s�������N�̈ړ�
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *BranchNodes[0]->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *BranchNodes.Last()->GetThenPin());
	CompilerContext.MovePinLinksToIntermediate(*GetElsePin(), *RerouteNode->GetOutputPin());

	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE