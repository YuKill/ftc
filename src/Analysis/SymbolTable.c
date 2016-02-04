#include "ftc/Analysis/SymbolTable.h"

#include <stdio.h>

void toEscapedName(char *Dst, char *FunName) {
  sprintf(Dst, "escaped.%s", FunName);
}

void initSymTable(ASTNode* Owner, SymbolTable *St) {
  St->Owner      = Owner;

  initHash(&(St->Table));
  initPtrVector(&(St->Child), 0);
}

SymbolTable *createSymbolTable(ASTNode *Owner, SymbolTable *Parent) {
  SymbolTable *St = (SymbolTable*) malloc(sizeof(SymbolTable));
  initSymTable(Owner, St);

  if (Parent) { ptrVectorAppend(&(Parent->Child), St); }
  St->Parent = Parent;
  return St;
}

int ownerIsFunction(SymbolTable *St) {
  return St->Owner->Kind == FunDecl;
}

SymbolTable *symTableFindChild(SymbolTable *St, ASTNode *Owner) {
  PtrVector *V = &(St->Child);
  PtrVectorIterator I = beginPtrVector(V),
                    E = endPtrVector(V);
  for (; I != E; ++I) {
    SymbolTable *Child = (SymbolTable*) *I;
    if (Child->Owner == Owner) return Child;
  }
  return NULL;
}

void insertIfEscaped(SymbolTable *St, char *Key) {
  char Buf[NAME_MAX];
  int Exists;
  Hash *EscapedVar;
  SymbolTable *Ptr = St;
  while (!ownerIsFunction(Ptr)) 
    if (hashExists(&(Ptr->Table), Key)) return; // has not escaped.
    else Ptr = Ptr->Parent; 
  if (hashExists(&(Ptr->Table), Key)) return;   // has not escaped.

  Exists = 0;
  do {
    Exists |= hashExists(&(Ptr->Table), Key);
    if (!ownerIsFunction(Ptr) || !Exists) Ptr = Ptr->Parent;
    else break;
  } while (1);

  toEscapedName(Buf, Ptr->Owner->Value);
  EscapedVar = (Hash*) symTableFind(Ptr, Buf);
  hashInsert(EscapedVar, Key, NULL);
}

int symTableInsertLocal(SymbolTable *St, char *Key, void *Value) {
  SymbolTable *Ptr = St;
  while (Ptr->Parent && !ownerIsFunction(St)) Ptr = Ptr->Parent;
  return symTableInsert(Ptr, Key, Value);
}

int symTableExistsLocal(SymbolTable *St, char *Key) {
  SymbolTable *Ptr = St;
  while (Ptr->Parent && !ownerIsFunction(St)) Ptr = Ptr->Parent;
  return symTableExists(Ptr, Key);
}

void *symTableFindLocal(SymbolTable *St, char *Key) {
  SymbolTable *Ptr = St;
  while (Ptr->Parent && !ownerIsFunction(St)) Ptr = Ptr->Parent;
  return symTableFind(Ptr, Key);
}

int symTableInsertGlobal(SymbolTable *St, char *Key, void *Value) {
  SymbolTable *Ptr = St;
  while (Ptr->Parent) Ptr = Ptr->Parent;
  return symTableInsert(Ptr, Key, Value);
}

int symTableExistsGlobal(SymbolTable *St, char *Key) {
  SymbolTable *Ptr = St;
  while (Ptr->Parent) Ptr = Ptr->Parent;
  return symTableExists(Ptr, Key);
}

void *symTableFindGlobal(SymbolTable *St, char *Key) {
  SymbolTable *Ptr = St;
  while (Ptr->Parent) Ptr = Ptr->Parent;
  return symTableFind(Ptr, Key);
}

void symTableInsertOrChange(SymbolTable *St, char *Key, void *Value) {
  hashInsertOrChange(&(St->Table), Key, Value);
}

int symTableInsert(SymbolTable *St, char *Key, void *Value) {
  return hashInsert(&(St->Table), Key, Value);
}

void *symTableFind(SymbolTable *St, char *Key) {
  if (!St) return NULL;
  void *Value = NULL;
  if (!(Value = hashFind(&(St->Table), Key)))
    return symTableFind(St->Parent, Key);
  return (Type*) Value;
}

int symTableExists(SymbolTable *St, char *Key) {
  if (!St) return 0;
  int Exists = 1;
  if (!(Exists = hashExists(&(St->Table), Key)))
    return symTableExists(St->Parent, Key);
  return Exists;
}

PtrVectorIterator beginSymTable(SymbolTable *St) { return beginHash(&(St->Table)); }
PtrVectorIterator endSymTable(SymbolTable *St) { return endHash(&(St->Table)); }
