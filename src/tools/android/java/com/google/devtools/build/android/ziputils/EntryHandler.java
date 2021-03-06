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
package com.google.devtools.build.android.ziputils;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Entry handler to pass to {@link ZipIn#scanEntries(EntryHandler)}. Implementations,
 * typically perform actions such as selecting, copying, renaming, merging, entries,
 * and writing entries to one or more {@link ZipOut}.
 */
public interface EntryHandler {

  /**
   * Handles a zip file entry. Called by the {@code ZipIn} scanner, for each entry.
   *
   * @param in The {@code ZipIn} from which we're called.
   * @param header The header for the entry to handle.
   * @param dirEntry The directory entry corresponding to this entry.
   * @param data byte buffer containing the data of the entry. If the data size cannot be
   * determine from the header or directory entry (zip error), then the provided buffer
   * may not contain all of the data.
   * @throws IOException implementations may thrown an IOException to signal that an error occurred
   * handling an entry. An IOException may also be generated by certain methods that the
   * may invoke on the arguments passed to this method.
   */
  void handle(ZipIn in, LocalFileHeader header, DirectoryEntry dirEntry, ByteBuffer data)
      throws IOException;
}
