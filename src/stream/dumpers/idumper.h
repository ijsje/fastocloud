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

#pragma once

#include <common/file_system/path.h>

typedef struct _GstBin GstBin;

namespace fastocloud {
namespace stream {
namespace dumper {

class IDumper {
 public:
  virtual bool Dump(GstBin* pipeline, const common::file_system::ascii_file_string_path& path) = 0;
  virtual ~IDumper();
};

}  // namespace dumper
}  // namespace stream
}  // namespace fastocloud
