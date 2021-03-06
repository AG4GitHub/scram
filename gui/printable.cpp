/*
 * Copyright (C) 2017-2018 Olzhas Rakhimov
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

/// @file
/// Implementation of default print functionality.

#include "printable.h"

#include <QObject>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

namespace scram::gui {

void Printable::print()
{
    QPrinter printer;
    QPrintDialog dialog(&printer);
    if (dialog.exec() == QDialog::Accepted)
        this->doPrint(&printer);
}

void Printable::printPreview()
{
    QPrinter printer;
    QPrintPreviewDialog preview(&printer);
    QObject::connect(&preview, &QPrintPreviewDialog::paintRequested,
                     [this, &printer] { this->doPrint(&printer); });
    preview.exec();
}

} // namespace scram::gui
