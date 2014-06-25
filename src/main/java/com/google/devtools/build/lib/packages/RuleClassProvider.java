// Copyright 2014 Google Inc. All rights reserved.
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

package com.google.devtools.build.lib.packages;

import com.google.common.collect.ImmutableMap;
import com.google.devtools.build.lib.syntax.Environment;
import com.google.devtools.build.lib.syntax.ValidationEnvironment;
import com.google.devtools.build.lib.vfs.Path;

import java.util.Map;

/**
 * The collection of the supported build rules. Provides an Environment for Skylark rule creation.
 */
public interface RuleClassProvider {
  /**
   * Returns a map from rule names to rule class objects.
   */
  Map<String, RuleClass> getRuleClassMap();

  /**
   * Returns a Skylark Environment for rule creation.
   */
  Environment getSkylarkRuleClassEnvironment(SkylarkRuleFactory ruleFactory, Path file);

  /**
   * Returns built in Java classes accessible from Skylark.
   */
  ImmutableMap<String, Class<?>> getSkylarkAccessibleJavaClasses();

  /**
   * Returns a validation environment for static analysis of skylark files.
   * The environment has to contain all built-in functions and objects.
   */
  ValidationEnvironment getSkylarkValidationEnvironment();
}