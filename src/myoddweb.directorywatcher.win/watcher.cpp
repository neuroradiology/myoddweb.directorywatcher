// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include "stdafx.h"
#include "watcher.h"
#include "utils/MonitorsManager.h"

namespace myoddweb
{
  namespace directorywatcher
  {
    /**
     * \brief start a monitor and return a unique id to make sure we can stop it.
     * \param request the request.
     * \return long long the unique id.
     */
    long long Start( const Request& request )
    {
      return MonitorsManager::Start(request);
    }

    /**
     * \brief start a monitor and return a unique id to make sure we can stop it.
     * \param id the id we want to stop monitoring.
     * \return bool success or not.
     */
    bool Stop(const long long id )
    {
      return MonitorsManager::Stop(id);
    }
  }
}