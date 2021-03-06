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
package com.google.devtools.build.lib.util;

/**
 * Detects the running operating system and returns a describing enum value.
 */
public enum OS {
  DARWIN("osx", "Mac OS X"),
  LINUX("linux", "Linux"),
  WINDOWS("windows", "Windows"),
  UNKNOWN("", "");

  private final String canonicalName;
  private final String detectionName;

  OS(String canonicalName, String detectionName) {
    this.canonicalName = canonicalName;
    this.detectionName = detectionName;
  }

  /**
   * The current operating system.
   */
  public static OS getCurrent() {
    return HOST_SYSTEM;
  }

  public String getCanonicalName() {
    return canonicalName;
  }

  // We inject a the OS name through blaze.os, so we can have
  // some coverage for Windows specific code on Linux.
  private static OS determineCurrentOs() {
    String osName = System.getProperty("blaze.os");
    if (osName == null) {
      osName = System.getProperty("os.name");
    }

    for (OS os : OS.values()) {
      if (os.detectionName.equals(osName)) {
        return os;
      }
    }

    return OS.UNKNOWN;
  }

  private static final OS HOST_SYSTEM = determineCurrentOs();
}
