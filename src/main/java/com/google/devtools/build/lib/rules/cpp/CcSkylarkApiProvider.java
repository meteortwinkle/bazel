// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.google.devtools.build.lib.rules.cpp;

import com.google.devtools.build.lib.actions.Artifact;
import com.google.devtools.build.lib.collect.nestedset.NestedSet;
import com.google.devtools.build.lib.rules.SkylarkApiProvider;
import com.google.devtools.build.lib.syntax.SkylarkCallable;
import com.google.devtools.build.lib.syntax.SkylarkModule;

/**
 * A class that exposes the C++ providers to Skylark. It is intended to provide a
 * simple and stable interface for Skylark users.
 */
@SkylarkModule(
    name = "CcSkylarkApiProvider", doc = "Provides access to information about C++ rules")
public final class CcSkylarkApiProvider extends SkylarkApiProvider {
  /** The name of the field in Skylark used to access this class. */
  static final String NAME = "cc";

  @SkylarkCallable(
      name = "transitive_headers",
      structField = true,
      doc =
          "Returns the immutable set of headers that have been declared in the <code>src</code> "
              + "or <code>headers</code> attribute (possibly empty but never None).")
  public NestedSet<Artifact> getTransitiveHeaders() {
    CppCompilationContext ccContext = getInfo().getProvider(CppCompilationContext.class);
    return ccContext.getDeclaredIncludeSrcs();
  }
}
