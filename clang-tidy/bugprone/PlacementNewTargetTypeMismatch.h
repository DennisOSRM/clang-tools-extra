//===--- PlacementNewTargetTypeMismatch.h - clang-tidy ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BUGPRONE_PLACEMENTNEWTARGETTYPEMISMATCHCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BUGPRONE_PLACEMENTNEWTARGETTYPEMISMATCHCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace bugprone {

/// Finds placement-new calls where the pointer type of the adress mismatches
/// the type of the created value.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/bugprone-placement-new-target-type-mismatch.html
class PlacementNewTargetTypeMismatch : public ClangTidyCheck {
public:
  PlacementNewTargetTypeMismatch(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace bugprone
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BUGPRONE_PLACEMENTNEWTARGETTYPEMISMATCHCHECK_H
