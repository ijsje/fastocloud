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

#include <string>

#include "base/link_generator/pyfastostream.h"

#include <common/sprintf.h>

namespace {
static const char* kStreams[9] = {"best", "1080p60", "720p60", "720p", "480p", "360p", "240p", "144p", "worst"};

bool GetTrueUrl(const std::string& path,
                const common::uri::GURL& url,
                const fastotv::PyFastoStream& link,
                common::uri::GURL* generated_url,
                size_t rec = 0) {
  if (rec >= SIZEOFMASS(kStreams)) {
    return false;
  }

  if (!generated_url) {
    return false;
  }

  std::string cmd_line = path + " --url";
  const auto http = link.GetHttp();
  if (http) {
    cmd_line += " --http-proxy=" + http->spec();
  }

  const auto https = link.GetHttps();
  if (https) {
    cmd_line += " --https-proxy=" + https->spec();
  }

  cmd_line += common::MemSPrintf(" \'%s\' --quality %s --prefer %d", url.spec(), kStreams[rec], link.GetPrefer());
  FILE* fp = popen(cmd_line.c_str(), "r");
  if (!fp) {
    return false;
  }

  char true_url[1024] = {0};
  char* res = fgets(true_url, sizeof(true_url) - 1, fp);
  int closed = pclose(fp);
  if (WIFEXITED(closed)) {
    if (WEXITSTATUS(closed) == EXIT_FAILURE) {
      return GetTrueUrl(path, url, link, generated_url, ++rec);
    }
  }

  if (!res) {
    return false;
  }

  size_t ln = strlen(true_url) - 1;
  if (true_url[ln] == '\n') {
    true_url[ln] = 0;
  }

  *generated_url = common::uri::GURL(true_url);
  return true;
}
}  // namespace

namespace fastocloud {
namespace link_generator {

PyFastoPyFastoStreamGenerator::PyFastoPyFastoStreamGenerator(
    const common::file_system::ascii_file_string_path& script_path)
    : script_path_(script_path) {}

bool PyFastoPyFastoStreamGenerator::Generate(const InputUri& src, InputUri* out) const {
  if (!out) {
    return false;
  }

  const auto str = src.GetPyFastoStream();
  if (!str) {
    return false;
  }

  if (!script_path_.IsValid()) {
    return false;
  }

  common::uri::GURL gen;
  if (!GetTrueUrl(script_path_.GetPath(), src.GetUrl(), *str, &gen)) {
    return false;
  }

  *out = src;
  out->SetUrl(gen);
  return true;
}

}  // namespace link_generator
}  // namespace fastocloud
