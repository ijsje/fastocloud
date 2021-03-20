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

#include "stream/elements/element.h"

namespace fastocloud {
namespace stream {
namespace elements {
namespace pay {

template <SupportedElements el>
class ElementRtpPay : public ElementEx<el> {
 public:
  typedef ElementEx<el> base_class;
  using base_class::base_class;

  void SetPt(guint pt) { ElementEx<el>::SetProperty("pt", pt); }
};

}  // namespace pay
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
