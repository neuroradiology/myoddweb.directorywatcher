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
     * \return __int64 the unique id.
     */
    long long Start(const Request request)
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

    /**
     * \brief Get the latest events.
     * \param id the id of the monitor we would like the events for.
     * \param events the events we will be getting
     * \return the number of items or -ve in case of an error
     */
    long long GetEvents( const long long id, std::vector<Event>& events)
    {
      return MonitorsManager::GetEvents(id, events );
    }
  }
}