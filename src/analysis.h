/*
 * Copyright (C) 2015 Olzhas Rakhimov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file analysis.h
/// Common facilities for all analysis classes.

#ifndef SCRAM_SRC_ANALYSIS_H_
#define SCRAM_SRC_ANALYSIS_H_

#include "settings.h"

namespace scram {

/// @class Analysis
/// Base abstract class for all analysis with settings.
class Analysis {
 public:
  /// @param[in] settings Analysis settings for all calculations.
  explicit Analysis(const Settings& settings);

  virtual ~Analysis() = 0;  ///< Abstract class.

  /// @returns Analysis settings.
  inline const Settings& settings() const { return kSettings_; }

 protected:
  const Settings kSettings_;  ///< All settings for analysis.
};

}  // namespace scram

#endif  // SCRAM_SRC_ANALYSIS_H_