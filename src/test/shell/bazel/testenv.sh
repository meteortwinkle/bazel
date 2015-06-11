#!/bin/bash
#
# Copyright 2015 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Setting up the environment for Bazel integration tests.
#

[ -z "$TEST_SRCDIR" ] && { echo "TEST_SRCDIR not set!" >&2; exit 1; }

# Load the unit-testing framework
source "${TEST_SRCDIR}/src/test/shell/unittest.bash" || \
  { echo "Failed to source unittest.bash" >&2; exit 1; }

# Bazel
bazel_tree="${TEST_SRCDIR}/src/test/shell/bazel/doc-srcs.zip"
bazel_path="${TEST_SRCDIR}/src"
bazel_data="${TEST_SRCDIR}"

# Java
jdk_dir="${TEST_SRCDIR}/external/local-jdk"
langtools="${TEST_SRCDIR}/src/test/shell/bazel/langtools.jar"

# Tools directory location
tools_dir="${TEST_SRCDIR}/tools"
EXTRA_BAZELRC="build --java_langtools=//tools/jdk:test-langtools"

# Java tooling
javabuilder_path="${TEST_SRCDIR}/src/java_tools/buildjar/JavaBuilder_deploy.jar"
singlejar_path="${TEST_SRCDIR}/src/java_tools/singlejar/SingleJar_deploy.jar"
ijar_path="${TEST_SRCDIR}/third_party/ijar/ijar"

# Third-party
PLATFORM="$(uname -s | tr 'A-Z' 'a-z')"
case "${PLATFORM}" in
  darwin)
    protoc_compiler="${TEST_SRCDIR}/third_party/protobuf/protoc-osx-x86_32.exe"
    ;;
  *)
    protoc_compiler="${TEST_SRCDIR}/third_party/protobuf/protoc-linux-x86_32.exe"
    ;;
esac
protoc_jar="${TEST_SRCDIR}/third_party/protobuf/protobuf-*.jar"
junit_jar="${TEST_SRCDIR}/third_party/junit/junit-*.jar"
hamcrest_jar="${TEST_SRCDIR}/third_party/hamcrest/hamcrest-*.jar"

# This function copies the tools directory from Bazel.
function copy_tools_directory() {
  cp -RL ${tools_dir}/* tools
  # To support custom langtools
  cp ${langtools} tools/jdk/langtools.jar
  cat >>tools/jdk/BUILD <<'EOF'
filegroup(name = "test-langtools", srcs = ["langtools.jar"])
EOF

  chmod -R +w .
  mkdir -p tools/defaults
  touch tools/defaults/BUILD
}

# Report whether a given directory name corresponds to a tools directory.
function is_tools_directory() {
  case "$1" in
    tools)
      true
      ;;
    *)
      false
      ;;
  esac
}

# Copy the examples of the base workspace
function copy_examples() {
  EXAMPLE="$TEST_SRCDIR/examples"
  cp -RL ${EXAMPLE} .
  chmod -R +w .
}

#
# Find a random unused TCP port
#
pick_random_unused_tcp_port () {
    perl -MSocket -e '
sub CheckPort {
  my ($port) = @_;
  socket(TCP_SOCK, PF_INET, SOCK_STREAM, getprotobyname("tcp"))
    || die "socket(TCP): $!";
  setsockopt(TCP_SOCK, SOL_SOCKET, SO_REUSEADDR, 1)
    || die "setsockopt(TCP): $!";
  return 0 unless bind(TCP_SOCK, sockaddr_in($port, INADDR_ANY));
  socket(UDP_SOCK, PF_INET, SOCK_DGRAM, getprotobyname("udp"))
    || die "socket(UDP): $!";
  return 0 unless bind(UDP_SOCK, sockaddr_in($port, INADDR_ANY));
  return 1;
}
for (1 .. 128) {
  my ($port) = int(rand() * 27000 + 32760);
  if (CheckPort($port)) {
    print "$port\n";
    exit 0;
  }
}
print "NO_FREE_PORT_FOUND\n";
exit 1;
'
}

#
# A uniform SHA-256 commands that works accross platform
#
case "${PLATFORM}" in
  darwin)
    function sha256sum() {
      cat "$1" | shasum -a 256 | cut -f 1 -d " "
    }
    ;;
  *)
    # Under linux sha256sum should exists
    ;;
esac
