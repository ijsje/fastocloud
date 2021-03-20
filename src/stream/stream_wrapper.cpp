/*  Copyright (C) 2014-2021 FastoGT. All right reserved.
    This file is part of fastocloud.
    fastocloud is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    fastocloud is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with fastocloud.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stream/stream_wrapper.h"

#include <unistd.h>

#include <memory>
#include <string>

#include <common/file_system/string_path_utils.h>

#include "base/config_fields.h"
#include "base/constants.h"

#include "stream/stream_controller.h"

namespace {

void on_signal(int signum) {
  if (signum == SIGTERM) {
    // ERROR_LOG() << "Force quit child!";
    _exit(EXIT_SUCCESS);
  }
}

const size_t kMaxSizeLogFile = 1024 * 1024;

int start_stream(const std::string& process_name,
                 const common::file_system::ascii_directory_string_path& feedback_dir,
                 const common::file_system::ascii_file_string_path& pyfastostream_path,
                 common::logging::LOG_LEVEL logs_level,
                 const fastocloud::StreamConfig& config_args,
                 fastotv::protocol::protocol_client_t* command_client,
                 const fastocloud::StreamInfo& sha) {
  auto log_file = feedback_dir.MakeFileStringPath(LOGS_FILE_NAME);
  if (log_file) {
    common::logging::INIT_LOGGER(process_name, log_file->GetPath(), logs_level,
                                 kMaxSizeLogFile);  // initialization of logging system
  }
  NOTICE_LOG() << "Running " PROJECT_VERSION_HUMAN;

  signal(SIGTERM, on_signal);

  for (auto it = config_args->begin(); it != config_args->end(); ++it) {
    common::Value* val = it->second;
    DEBUG_LOG() << it->first << ": " << *val;
  }

  const std::unique_ptr<fastocloud::StreamStruct> mem(new fastocloud::StreamStruct(sha));
  fastocloud::stream::StreamController proc(feedback_dir, pyfastostream_path, command_client, mem.get());
  common::Error err = proc.Init(config_args);
  if (err) {
    WARNING_LOG() << err->GetDescription();
    NOTICE_LOG() << "Quiting " PROJECT_VERSION_HUMAN;
    return EXIT_FAILURE;
  }

  int res = proc.Exec();
  NOTICE_LOG() << "Quiting " PROJECT_VERSION_HUMAN;
  return res;
}

}  // namespace

int stream_exec(const char* process_name, const void* args, void* command_client) {
  if (!process_name || !args || !command_client) {
    CRITICAL_LOG() << "Invalid arguments.";
    return EXIT_FAILURE;
  }

  const common::HashValue* vargs = static_cast<const common::HashValue*>(args);
  fastocloud::StreamConfig sargs(vargs->DeepCopy());
  fastocloud::StreamInfo sha;
  std::string feedback_dir;
  std::string data_dir;
  common::logging::LOG_LEVEL logs_level;
  common::ErrnoError err = fastocloud::MakeStreamInfo(sargs, true, &sha, &feedback_dir, &data_dir, &logs_level);
  if (err) {
    CRITICAL_LOG() << "Invalid config error: " << err->GetDescription();
    return EXIT_FAILURE;
  }

  if (feedback_dir.empty()) {
    CRITICAL_LOG() << "Define " FEEDBACK_DIR_FIELD " variable and make it valid";
    return EXIT_FAILURE;
  }

  std::string pyfastostream_path;
  common::Value* PyFastoStream_path_field = sargs->Find(PYFASTOSTREAM_PATH_FIELD);
  if (!PyFastoStream_path_field || !PyFastoStream_path_field->GetAsBasicString(&pyfastostream_path)) {
    CRITICAL_LOG() << "Define pyfastostream path variable and make it valid";
    return EXIT_FAILURE;
  }

  fastotv::protocol::protocol_client_t* client = static_cast<fastotv::protocol::protocol_client_t*>(command_client);
  return start_stream(process_name, common::file_system::ascii_directory_string_path(feedback_dir),
                      common::file_system::ascii_file_string_path(pyfastostream_path), logs_level, sargs, client, sha);
}
