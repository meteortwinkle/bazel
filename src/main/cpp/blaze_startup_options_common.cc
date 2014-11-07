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
#include "blaze_startup_options.h"

#include <cassert>
#include <cstdlib>

#include "blaze_exit_code.h"
#include "blaze_util_platform.h"
#include "blaze_util.h"
#include "util/numbers.h"
#include "util/strings.h"

namespace blaze {

void BlazeStartupOptions::Init() {
  bool testing = getenv("TEST_TMPDIR") != NULL;
  if (testing) {
    output_root = MakeAbsolute(getenv("TEST_TMPDIR"));
  } else {
    output_root = GetOutputRoot();
  }

  output_user_root = output_root + "/_blaze_" + GetUserName();
  block_for_lock = true;
  host_jvm_debug = false;
  host_javabase = "";
  use_blaze64 = false;
  batch = false;
  batch_cpu_scheduling = false;
  allow_configurable_attributes = false;
  fatal_event_bus_exceptions = false;
  io_nice_level = -1;
  // 3 hours (but only 5 seconds if used within a test)
  max_idle_secs = testing ? 5 : (3 * 3600);
  webstatus_port = 0;
  watchfs = false;
}

string BlazeStartupOptions::GetHostJavabase() {
  if (host_javabase.empty()) {
    host_javabase = GetDefaultHostJavabase();
  }
  return host_javabase;
}

void BlazeStartupOptions::Copy(
    const BlazeStartupOptions &rhs, BlazeStartupOptions *lhs) {
  assert(lhs);

  lhs->output_base = rhs.output_base;
  lhs->install_base = rhs.install_base;
  lhs->output_root = rhs.output_root;
  lhs->output_user_root = rhs.output_user_root;
  lhs->block_for_lock = rhs.block_for_lock;
  lhs->host_jvm_debug = rhs.host_jvm_debug;
  lhs->host_jvm_profile = rhs.host_jvm_profile;
  lhs->host_javabase = rhs.host_javabase;
  lhs->host_jvm_args = rhs.host_jvm_args;
  lhs->use_blaze64 = rhs.use_blaze64;
  lhs->batch = rhs.batch;
  lhs->batch_cpu_scheduling = rhs.batch_cpu_scheduling;
  lhs->io_nice_level = rhs.io_nice_level;
  lhs->max_idle_secs = rhs.max_idle_secs;
  lhs->skyframe = rhs.skyframe;
  lhs->skygraph = rhs.skygraph;
  lhs->webstatus_port = rhs.webstatus_port;
  lhs->watchfs = rhs.watchfs;
  lhs->allow_configurable_attributes = rhs.allow_configurable_attributes;
  lhs->fatal_event_bus_exceptions = rhs.fatal_event_bus_exceptions;
  lhs->option_sources = rhs.option_sources;
}

void BlazeStartupOptions::InitDefaults(const string& argv0) {
  use_blaze64 = blaze_util::ends_with(argv0, "blaze64") ||
      blaze_util::ends_with(argv0, "blaze64.rc");
}

bool BlazeStartupOptions::ProcessArg(const string& argstr,
                                     const string& next_argstr,
                                     const string& rcfile) {
  // We have to parse a specific option syntax, so GNU getopts won't do.  All
  // options begin with "--" or "-". Values are given together with the option
  // delimited by '=' or in the next option.
  const char* arg = argstr.c_str();
  const char* next_arg = next_argstr.empty() ? NULL : next_argstr.c_str();
  const char* value = NULL;

  if ((value = GetUnaryOption(arg, next_arg, "--output_base")) != NULL) {
    output_base = MakeAbsolute(value);
    option_sources["output_base"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg,
                                     "--install_base")) != NULL) {
    install_base = MakeAbsolute(value);
    option_sources["install_base"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg,
                                     "--output_user_root")) != NULL) {
    output_user_root = MakeAbsolute(value);
    option_sources["output_user_root"] = rcfile;
  } else if (GetNullaryOption(arg, "--noblock_for_lock")) {
    block_for_lock = false;
    option_sources["block_for_lock"] = rcfile;
  } else if (GetNullaryOption(arg, "--host_jvm_debug")) {
    host_jvm_debug = true;
    option_sources["host_jvm_debug"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg,
                                     "--host_jvm_profile")) != NULL) {
    host_jvm_profile = value;
    option_sources["host_jvm_profile"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg,
                                     "--host_javabase")) != NULL) {
    // TODO(bazel-team): Consider examining the javabase, and in case of
    // architecture mismatch, treating this option like --blaze_cpu
    // and re-execing.
    host_javabase = MakeAbsolute(value);
    option_sources["host_javabase"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg,
                                     "--host_jvm_args")) != NULL) {
    if (host_jvm_args.empty()) {
      host_jvm_args = value;
    } else {
      host_jvm_args = host_jvm_args + " " + value;
    }
    option_sources["host_jvm_args"] = rcfile;  // NB: This is incorrect
  } else if ((value = GetUnaryOption(arg, next_arg, "--blaze_cpu")) != NULL) {
    if (strcmp(value, "k8") == 0) {
      use_blaze64 = true;
    } else if (strcmp(value, "piii") == 0) {
      use_blaze64 = false;
    } else {
      die(blaze_exit_code::BAD_ARGV,
          "Invalid value '%s' for the --blaze_cpu option. "
          "Must be 'k8' or 'piii'.",
          value);
    }
    option_sources["blaze_cpu"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg, "--blazerc")) != NULL) {
    if (rcfile != "") {
      die(blaze_exit_code::BAD_ARGV,
          "Can't specify --blazerc in the .blazerc file.");
    }
  } else if (GetNullaryOption(arg, "--nomaster_blazerc") ||
             GetNullaryOption(arg, "--master_blazerc")) {
    if (rcfile != "") {
      die(blaze_exit_code::BAD_ARGV,
          "Can't specify --[no]master_blazerc in .blazerc file.");
    }
    option_sources["blazerc"] = rcfile;
  } else if (GetNullaryOption(arg, "--batch")) {
    batch = true;
    option_sources["batch"] = rcfile;
  } else if (GetNullaryOption(arg, "--nobatch")) {
    batch = false;
    option_sources["batch"] = rcfile;
  } else if (GetNullaryOption(arg, "--batch_cpu_scheduling")) {
    batch_cpu_scheduling = true;
    option_sources["batch_cpu_scheduling"] = rcfile;
  } else if (GetNullaryOption(arg, "--nobatch_cpu_scheduling")) {
    batch_cpu_scheduling = false;
    option_sources["batch_cpu_scheduling"] = rcfile;
  } else if (GetNullaryOption(arg, "--allow_configurable_attributes")) {
    allow_configurable_attributes = true;
    option_sources["allow_configurable_attributes"] = rcfile;
  } else if (GetNullaryOption(arg, "--noallow_configurable_attributes")) {
    allow_configurable_attributes = false;
    option_sources["allow_configurable_attributes"] = rcfile;
  } else if (GetNullaryOption(arg, "--fatal_event_bus_exceptions")) {
    fatal_event_bus_exceptions = true;
    option_sources["fatal_event_bus_exceptions"] = rcfile;
  } else if (GetNullaryOption(arg, "--nofatal_event_bus_exceptions")) {
    fatal_event_bus_exceptions = false;
    option_sources["fatal_event_bus_exceptions"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg,
                                     "--io_nice_level")) != NULL) {
    if (!blaze_util::safe_strto32(value, &io_nice_level) ||
        io_nice_level > 7) {
      die(blaze_exit_code::BAD_ARGV,
          "Invalid argument to --io_nice_level: '%s'. Must not exceed 7.",
          value);
    }
    option_sources["io_nice_level"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg,
                                     "--max_idle_secs")) != NULL) {
    if (!blaze_util::safe_strto32(value, &max_idle_secs) ||
        max_idle_secs < 0) {
      die(blaze_exit_code::BAD_ARGV,
          "Invalid argument to --max_idle_secs: '%s'.", value);
    }
    option_sources["max_idle_secs"] = rcfile;
  } else if ((value = GetUnaryOption(arg, next_arg,
              "--skyframe")) != NULL) {
    fprintf(stderr, "WARNING: The --skyframe startup option is now ignored "
            "and will be removed in a future release\n");
  } else if ((value = GetUnaryOption(arg, next_arg,
              "--skygraph")) != NULL) {
    fprintf(stderr, "WARNING: The --skygraph startup option is an internal "
            "flag and will be removed in a future release\n");
    skygraph = value;
    option_sources["skygraph"] = rcfile;
  } else if (GetNullaryOption(arg, "-x")) {
    fprintf(stderr, "WARNING: The -x startup option is now ignored "
            "and will be removed in a future release\n");
  } else if (GetNullaryOption(arg, "--watchfs")) {
    watchfs = true;
    option_sources["watchfs"] = rcfile;
  } else if ((value = GetUnaryOption(
      arg, next_arg, "--use_webstatusserver")) != NULL) {
    if (!blaze_util::safe_strto32(value, &webstatus_port) ||
        webstatus_port < 0 || webstatus_port > 65535) {
      die(blaze_exit_code::BAD_ARGV,
          "Invalid argument to --use_webstatusserver: '%s'. "
          "Must be a valid port number or 0 if server disabled.\n", value);
    }
    option_sources["webstatusserver"] = rcfile;
  } else if (!ProcessArgExtra(arg, next_arg, rcfile, &value)) {
    die(blaze_exit_code::BAD_ARGV,
        "Unknown Blaze startup option: '%s'.\n"
        "  For more info, run 'blaze help startup_options'.",
        arg);
  }

  return ((value == next_arg) && (value != NULL));
}

}  // namespace blaze
