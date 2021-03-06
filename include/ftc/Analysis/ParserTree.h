#ifndef PARSERTREE_H
#define PARSERTREE_H

#include "ftc/Support/PtrVector.h"

#define VEC_SIZE 2

typedef struct ASTNode ASTNode;

typedef enum {
  DeclList = 0, TyDeclList, FunDeclList, ArgDeclList, ArgExprList, FieldExprList,

  /* Declarations */
  VarDecl, FunDecl, TyDecl, ArgDecl,
  IntTy, FloatTy, StringTy, AnswerTy, ContTy, StrConsumerTy, 
  IdTy, FunTy, ArrayTy, SeqTy, NilTy, RecordTy,

  /* Expressions */
  IntLit, FloatLit, StringLit, 
  IdLval, RecAccessLval, ArrAccessLval,
  SumOp, SubOp, MultOp, DivOp, EqOp, DiffOp, LtOp, LeOp, GtOp, GeOp, AndOp, OrOp,
  NegOp, LetExpr, IfStmtExpr, FunCallExpr, ArgExpr, ArrayExpr, RecordExpr, FieldExpr,
  NilExpr
} NodeKind;

struct ASTNode {
  NodeKind Kind;
  void *Value;
  unsigned Pos[2];
  int EscapedLevel;
  PtrVector Child;
};

ASTNode *createASTNode   (NodeKind, void*, int,...);
void     destroyASTNode  (void*);
void     addToASTNode    (ASTNode*, ASTNode*);
void     moveAllToASTNode(ASTNode*, PtrVector*);
void     setASTNodePos   (ASTNode*, unsigned, unsigned);

ASTNode *Root;

#endif
