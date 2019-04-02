//===--- PlacementNewTargetTypeMismatch.cpp - clang-tidy ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "PlacementNewTargetTypeMismatch.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecordLayout.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bugprone {
namespace {
AST_MATCHER(Expr, isPlacementNewExpr) {
  const auto *NewExpr = dyn_cast<CXXNewExpr>(&Node);
  return NewExpr && NewExpr->getNumPlacementArgs() != 0;
}
} // namespace

void PlacementNewTargetTypeMismatch::registerMatchers(MatchFinder *Finder) {
  // We only want the records that call 'new' with an adress parameter.
  Finder->addMatcher(expr(isPlacementNewExpr()).bind("NewExpr"), this);
}

void PlacementNewTargetTypeMismatch::check(
    const MatchFinder::MatchResult &Result) {
  const auto *NewExpr = Result.Nodes.getNodeAs<CXXNewExpr>("NewExpr");
  assert(NewExpr && "Matched node bound by 'NewExpr' shoud be a 'CXXNewExpr'");
  assert(NewExpr->getNumPlacementArgs() != 0 && "");

  // Fetch the cast from the Expr of the placement argument, if it exists.
  const Expr *PlacementExpr = NewExpr->getPlacementArg(0);
  assert(PlacementExpr != nullptr && "PlacementExpr should not be null");
  const CastExpr *Cast = dyn_cast<CastExpr>(PlacementExpr);
  if (Cast == nullptr)
    return;

  QualType SubExprType = Cast->getSubExprAsWritten()->getType();

  if ((!SubExprType->isPointerType() && !SubExprType->isArrayType()) ||
      SubExprType->isVoidPointerType())
    return;

  QualType PlacementParameterType =
      SubExprType->getPointeeOrArrayElementType()->getCanonicalTypeInternal();
  QualType AllocatedType = NewExpr->getAllocatedType().getCanonicalType();

  if (AllocatedType->isIncompleteType() ||
      PlacementParameterType->isIncompleteType())
    return;

  if (PlacementParameterType != AllocatedType) {
    diag(PlacementExpr->getExprLoc(),
         "placement new parameter and allocated type mismatch");
  }
}

} // namespace bugprone
} // namespace tidy
} // namespace clang
