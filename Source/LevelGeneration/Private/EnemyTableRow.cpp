// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTableRow.h"


void FEnemyTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);
}

void FEnemyTableRow::OnPostDataImport(const UDataTable* InDataTable, const FName InRowName,
	TArray<FString>& OutCollectedImportProblems)
{
	FTableRowBase::OnPostDataImport(InDataTable, InRowName, OutCollectedImportProblems);
}
