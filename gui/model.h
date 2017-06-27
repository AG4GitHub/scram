/*
 * Copyright (C) 2017 Olzhas Rakhimov
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

/// @file model.h
/// Wrapper Model classes for the MEF data.

#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include "src/event.h"
#include "src/model.h"

namespace scram {
namespace gui {
namespace model {

/// The wrapper around the MEF Model.
class Model : public QObject
{
    Q_OBJECT

public:
    /// @param[in] model  The analysis model with all constructs.
    explicit Model(mef::Model *model, QObject *parent = nullptr);

    mef::Model *data() { return m_model; }

    void addHouseEvent(const mef::HouseEventPtr &houseEvent);

signals:
    void addedHouseEvent(mef::HouseEvent *houseEvent);

private:
    mef::Model *m_model;
};

class HouseEvent : public QObject
{
    Q_OBJECT

public:
    explicit HouseEvent(mef::HouseEvent *houseEvent);

    mef::HouseEvent *data() { return m_houseEvent; }

private:
    mef::HouseEvent *m_houseEvent;
};

} // namespace model
} // namespace gui
} // namespace scram

#endif // MODEL_H