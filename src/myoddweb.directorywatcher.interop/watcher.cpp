// Licensed to Florent Guelfucci under one or more agreements.
// Florent Guelfucci licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#include "stdafx.h"
#include "watcher.h"
#include "core/Importer.h"
#include "core/Request.h"
#include <msclr\marshal_cppstd.h>

namespace myoddweb
{
  namespace directorywatcher
  {
    namespace Interop
    {
      Watcher::Watcher() :
      _coreWatcher(nullptr)
      {
        _coreWatcher = (Watcher1*)Importer(IID_IWatcher1);
      }

      Watcher::~Watcher()
      {
        this->!Watcher();
      }

      Watcher::!Watcher()
      {
        delete _coreWatcher;
        _coreWatcher = nullptr;
      }

      /**
       * \inheritdoc
       */
      long long Watcher::Start(myoddweb::directorywatcher::interfaces::IRequest^ request)
      {
        return _coreWatcher == nullptr ? -1 : _coreWatcher->Start(request);
      }

      /**
       * \inheritdoc
       */
      bool Watcher::Stop(long long id )
      {
        return _coreWatcher == nullptr ? -1 : _coreWatcher->Stop(id);
      }

      /**
       * \inheritdoc
       */
      long long Watcher::GetEvents(long long id, IList<myoddweb::directorywatcher::interfaces::IEvent^> ^% events )
      {
        return _coreWatcher == nullptr ? -1 : _coreWatcher->GetEvents(id, events );
      }
    }
  }
}