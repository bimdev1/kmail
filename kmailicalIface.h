/*
    This file is part of KMail.
    Copyright (c) 2003 Steffen Hansen <steffen@klaralvdalens-datakonsult.se>
    Copyright (c) 2003 - 2004 Bo Thorsen <bo@klaralvdalens-datakonsult.se>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
#ifndef KMAILICALIFACE_H
#define KMAILICALIFACE_H

#include <dcopobject.h>
#include <qstringlist.h>

class KMailICalIface : virtual public DCOPObject
{
    K_DCOP
  k_dcop:
    virtual bool addIncidence( const QString& folder, const QString& uid, 
			       const QString& ical ) = 0;
    virtual bool deleteIncidence( const QString& folder, const QString& uid ) = 0;
    virtual QStringList incidences( const QString& folder ) = 0;

    // This saves the iCals/vCards in the entries in the folder.
    // The format in the string list is uid, entry, uid, entry...
    virtual bool update( const QString& folder,
			 const QStringList& entries ) = 0;

    // Update a single entry in the storage layer
    virtual bool update( const QString& folder, const QString& uid,
			 const QString& entry ) = 0;

  k_dcop_signals:
    void incidenceAdded( const QString& folder, const QString& ical );
    void incidenceDeleted( const QString& folder, const QString& uid );
};

#endif
