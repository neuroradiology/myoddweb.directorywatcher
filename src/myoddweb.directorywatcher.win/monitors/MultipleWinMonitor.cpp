//This file is part of Myoddweb.Directorywatcher.
//
//    Myoddweb.Directorywatcher is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Myoddweb.Directorywatcher is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Myoddweb.Directorywatcher.  If not, see<https://www.gnu.org/licenses/gpl-3.0.en.html>.
#include "MultipleWinMonitor.h"

namespace myoddweb
{
  namespace directorywatcher
  {
    MultipleWinMonitor::MultipleWinMonitor(const __int64 id, const Request& request) :
      MultipleWinMonitor(id, request, 0, 2 )
    {
    }

    MultipleWinMonitor::MultipleWinMonitor(const __int64 id, const Request& request, const int depth, const int maxDepth) :
      Monitor(id, request)
    {
      // use a standar monitor for non recursive items.
      if( !request.Recursive )
      {
        throw std::invalid_argument("The multiple monitor must be recursive.");
      }

      // try and create the list of monitors.
      CreateMonitors(_request, depth, maxDepth );
    }

    MultipleWinMonitor::~MultipleWinMonitor()
    {
      Delete();
    }

    /**
     * \brief Clear all the current data
     */
    void MultipleWinMonitor::Delete()
    {
      // stop everything
      Stop();

      try
      {
        // and the monitors.
        for (auto it = _monitors.begin(); it != _monitors.end(); ++it)
        {
          delete *it;
        }
        // all done
        _monitors.clear();
      }
      catch (...)
      {
        // we might as well clear everything now.
        _monitors.clear();
      }
    }

    /**
     * \brief Start monitoring
     */
    bool MultipleWinMonitor::Start()
    {
      // sstop and clear everything
      Stop();

      try
      {
        // and start them all.
        for (auto it = _monitors.begin(); it != _monitors.end(); ++it)
        {
          (*it)->Start();
        }
      }
      catch(...)
      {
        AddEventError(EventError::CannotStart);
        return false;

      }
      return false;
    }

    /**
     * \brief Start monitoring
     */
    void MultipleWinMonitor::Stop()
    {
      try
      {
        // and the monitors.
        for (auto it = _monitors.begin(); it != _monitors.end(); ++it)
        {
          (*it)->Stop();
        }
      }
      catch (...)
      {
      }
    }

    /**
     * \brief join 2 parts of a path
     * \param lhs the lhs folder.
     * \param rhs the rhs file/folder
     * \param rhsIsFile if the rhs is a file, then we will not add a trailling back-slash
     * \return all the sub-folders, (if any).
     */
    std::wstring MultipleWinMonitor::Join(const std::wstring& lhs, const std::wstring& rhs, const bool rhsIsFile)
    {
      const auto ll = lhs.length();
      if (ll > 0)
      {
        const auto c = lhs[ll - 1];
        if (c == L'\\' || c == L'/' )
        {
          return Join(lhs.substr(0, ll - 1), rhs, rhsIsFile);
        }
      }

      const auto lr = rhs.length();
      if (lr > 0)
      {
        const auto c = rhs[0];
        if (c == L'\\' || c == L'/')
        {
          return Join(lhs, rhs.substr(1, lr - 1), rhsIsFile);
        }
      }
      return lhs + L'\\' + rhs + (rhsIsFile ? L"" : L"\\");
    }

    /**
     * \brief Check if a given directory is a dot or double dot
     * \param directory the lhs folder.
     * \return if it is a dot directory or not
     */
    bool MultipleWinMonitor::IsDot(const std::wstring& directory)
    {
      return directory == L"." || directory == L"..";
    }

    /**
     * \brief Get all the sub folders of a given folder.
     * \param folder the starting folder.
     * \return all the sub-folders, (if any).
     */
    std::vector<std::wstring> MultipleWinMonitor::GetAllSubFolders(const std::wstring& folder)
    {
      std::vector<std::wstring> names;
      auto searchPath = Join( folder, L"/*.*", true );
      WIN32_FIND_DATA fd= {};
      const auto hFind = ::FindFirstFile(searchPath.c_str(), &fd);
      if (hFind != INVALID_HANDLE_VALUE) {
        do {
          // read all (real) files in current folder
          // , delete '!' read other 2 default folder . and ..
          if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) 
          {
            if (!IsDot(fd.cFileName))
            {
              names.emplace_back(Join(folder, fd.cFileName, false));
            }
          }
        } while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
      }
      return names;
    }

    /**
     * \brief fill the vector with all the values currently on record.
     * \param events the events we will be filling
     * \return the number of events we found.
     */
    long long MultipleWinMonitor::GetEvents(std::vector<Event>& events) const
    {
      long long count = 0;
      for (auto it = _monitors.begin(); it != _monitors.end(); ++it)
      {
        // get this directory events
        std::vector<Event> levents;
        const auto lcount = (*it)->GetEvents(levents);

        // if we got nothing we just move on.
        if( lcount == 0 )
        {
          continue;
        }

        // add them to our list of events.
        events.insert(events.end(), levents.begin(), levents.end() );

        // add count.
        count += lcount;
      }
      return count;
    }

    /**
     * \brief get the next available id.
     * \return the next usable id.
     */
    long MultipleWinMonitor::GetNextId() const
    {
      // the id does not really matter, but it will be
      // unique to our list of monitors.
      return static_cast<long>( _monitors.size() );
    }

    /**
     * \brief Create all the sub-requests for a prarent request.
     * \param parent the parent request itselft.
     * \param maxNumberOfChildren the maximum number of children we will allow
     */
    void MultipleWinMonitor::CreateMonitors(const Request& parent, const int depth, const int maxDepth)
    {
      // get the next id.
      const auto id = GetNextId();

      // if the parent was not recursive
      // then we do not need to go further.
      if( !parent.Recursive )
      {
        _monitors.push_back( new WinMonitor(id, parent) );
        return;
      }

      // look for all the sub-paths
      const auto subPaths = GetAllSubFolders(parent.Path);
      if( depth >= maxDepth || subPaths.empty())
      {
        // we will breach the depth
        _monitors.push_back(new WinMonitor(id, parent));
        return;
      }

      // adding all the sub-paths will not breach the limit.
      // so we can add the parent, but non-recuresive.
      _monitors.push_back( new WinMonitor(id, { parent.Path, false }) );

      // now try and add all the subpath
      for (const auto& path : subPaths)
      {
        const auto multiMonitor = new MultipleWinMonitor(GetNextId(), { path, true }, depth+1, maxDepth );
        _monitors.push_back(multiMonitor);
      }
    }
  }
}