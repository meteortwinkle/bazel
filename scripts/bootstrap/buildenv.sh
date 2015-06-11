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

# General purpose method and values for bootstrapping bazel.

set -o errexit

DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
WORKSPACE_DIR="$(dirname $(dirname ${DIR}))"

JAVA_VERSION=${JAVA_VERSION:-1.8}
BAZELRC=${BAZELRC:-"/dev/null"}
PLATFORM="$(uname -s | tr 'A-Z' 'a-z')"

ATEXIT_=""
function atexit() {
  ATEXIT_="$1; ${ATEXIT_}"
  trap "{ ${ATEXIT_} }" EXIT
}

function tempdir() {
  local tmp=${TMPDIR:-/tmp}
  local DIR="$(mktemp -d ${tmp%%/}/bazel.XXXXXXXX)"
  mkdir -p "${DIR}"
  atexit "rm -fr ${DIR}"
  NEW_TMPDIR="${DIR}"
}
tempdir
OUTPUT_DIR=${NEW_TMPDIR}
errfile=${OUTPUT_DIR}/errors
atexit "if [ -f ${errfile} ]; then cat ${errfile} >&2; fi"
phasefile=${OUTPUT_DIR}/phase
atexit "if [ -f ${phasefile} ]; then echo >&2; cat ${phasefile} >&2; fi"

function run_silent() {
  echo "${@}" >${errfile}
  "${@}" >>${errfile} 2>&1
  rm ${errfile}
}

function fail() {
  echo >&2
  echo "$1" >&2
  exit 1
}

function display() {
  if [[ -z "${QUIETMODE}" ]]; then
    echo -e "$@" >&2
  fi
}

function log() {
  echo -n "." >&2
  echo "$1" >${phasefile}
}

function clear_log() {
  echo >&2
  rm -f ${phasefile}
}

LEAVES="\xF0\x9F\x8D\x83"
INFO="\033[32mINFO\033[0m:"

first_step=1
function new_step() {
  rm -f ${phasefile}
  local new_line=
  if [ -n "${first_step}" ]; then
    first_step=
  else
    new_line="\n"
  fi
  display -n "$new_line$LEAVES  $1"
}

function git_sha1() {
  if [ -x "$(which git || true)" ] && [ -d .git ]; then
    git rev-parse --short HEAD 2>/dev/null || true
  fi
}

if [[ ${PLATFORM} == "darwin" ]]; then
  function md5_file() {
    echo $(cat $1 | md5) $1
  }
else
  function md5_file() {
    md5sum $1
  }
fi
